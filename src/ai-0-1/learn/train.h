#pragma once

#include <game.h>
#include <ai-0-1/find_move.h>
#include <ai-0-1/learn/eval.h>
#include <util/random.h>

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>

namespace ai01 {
	
	class Train {
		
	public:
		
		Train(const std::string& _srcdir, int32_t _id) :
		m_srcdir(_srcdir + "/ai-0-1/learn/weights/" + std::to_string(_id)),
		m_id(_id)
		{
			if (std::filesystem::exists(m_srcdir)) {
				std::cout << "'" << m_srcdir << "' already exists" << std::endl;
				std::cout << "  (1): terminate initialization" << std::endl;
				std::cout << "  (2): override batch" << std::endl;
				std::cout << "  (3): continue batch" << std::endl;
				std::string inp;
				std::getline(std::cin, inp);
				while (inp != "1" && inp != "2" && inp != "3") {
					std::cout << "bad input" << std::endl;
					std::getline(std::cin, inp);
				}
				if (inp == "1") {
					std::cout << "failed to initialize ai01::Train" << std::endl;
					return;
				} else if (inp == "2") {
					std::cout << "overriding batch at '" << m_srcdir << "'" << std::endl;
					std::filesystem::remove_all(m_srcdir);
					std::filesystem::create_directories(m_srcdir);
					m_create_batch();
					m_continue_batch();
				} else if (inp == "3") {
					std::cout << "attempting to continue batch" << std::endl;
					m_continue_batch();
				} else {
					std::cout << "failed to initialize ai01::Train" << std::endl;
					return;
				}
				return;
			}
			std::cout << "creating batch at '" << m_srcdir << "'" << std::endl;
			std::filesystem::create_directories(m_srcdir);
			m_create_batch();
			m_continue_batch();
		}
		
	private:
		
		std::string& m_srcdir;
		int32_t m_id;
		
	private:
		
		bool m_is_int(const std::string& s) {
			if (s.empty()) {
				return false;
			}
			for (char c : s) {
				if (!isdigit(c)) {
					return false;
				}
			}
			return true;
		}
		
		bool m_is_float(const std::string& s) {
			if (s.empty()) {
				return false;
			}
			for (char c : s) {
				if (!isdigit(c) && c != '.') {
					return false;
				}
			}
			if (s.front() == '.' || s.back() == '.') {
				return false;
			}
			return true;
		}
		
		void m_create_batch() {
			std::ofstream ouf(m_srcdir + "/conf", std::ofstream::out | std::ofstream::trunc);
			if (!ouf.is_open()) {
				std::cerr << "failed to open '" << m_srcdir << "/conf'" << std::endl;
				std::cerr << "failed to create batch (" << m_id << ")" << std::endl;
				return;
			}
			std::string inp;
			std::cout << "batch size > ";
			std::cout.flush();
			std::getline(std::cin, inp);
			while (m_is_int(inp)) {
				std::cout << "bad input\nbatch size > ";
				std::cout.flush();
				std::getline(std::cin, inp);
			}
			ouf << "batch_size " << inp << "\n";
			std::cout << "mutation rate > ";
			std::cout.flush();
			std::getline(std::cin, inp);
			while (m_is_float(inp)) {
				std::cout << "bad input\nmutation rate > ";
				std::cout.flush();
				std::getline(std::cin, inp);
			}
			ouf << "mutation_rate " << inp << "\n";
			ouf << "generation 0\n";
			ouf.close();
		}
		
		void m_continue_batch() {
			int32_t generation = 0;
			int32_t batch_size = 0;
			double mutation_rate = 0;
			std::ifstream inf(m_srcdir + "/conf");
			if (!inf.is_open()) {
				std::cerr << "failed to open '" << m_srcdir << "/conf'" << std::endl;
				return;
			}
			std::string line;
			while (inf >> line) {
				if (line == "generation") {
					inf >> line;
					generation = std::stoi(line);
				} else if (line == "batch_size") {
					inf >> line;
					generation = std::stoi(line);
				} else if (line == "mutation_rate") {
					inf >> line;
					mutation_rate = std::stod(line);
				}
			}
			inf.close();
			std::vector <Learn_eval> levals;
			if (generation == 0) {
				for (int32_t i = 0; i < batch_size; i++) {
					levals.emplace_back(Learn_eval());
				}
			} else {
				for (int32_t i = 0; i < batch_size; i++) {
					levals.emplace_back(Learn_eval(m_srcdir + "/subject_" + std::to_string(i)));
				}
			}
			auto write_log = [&] (const std::string& message) -> void {
				std::ofstream ouf(m_srcdir + "/log", std::ios::app);
				if (!ouf.is_open()) {
					std::cerr << "failed to open '" << m_srcdir << "/log'" << std::endl;
					return;
				}
				ouf << message << "\n";
				ouf.close();
			};
			auto run_generation = [&] (std::atomic <bool>& run) -> void {
				auto write_progress = [&] (int prc, const std::string& message, bool is_err = false) -> void {
					std::string str = "generation: ";
					std::string num = std::to_string(generation);
					while ((int)num.size() < 6) {
						num.insert(num.begin(), '0');
					}
					str += num;
					str += " [";
					str += std::string(100, '.');
					str += "]";
					for (int i = 0; i < 100; i++) {
						if (i < prc) {
							str[i + 1] = '#';
						}
					}
					str += " ";
					str += message;
					if (is_err) {
						str += " (error found)";
					}
					if (!run.load()) {
						str += " (quitting)";
					}
					std::cout << "\r" << str;
					std::cout.flush();
				};
				bool is_err = false;
				generation++;
				write_log("--- generation " + std::to_string(generation) + " ---");
				std::vector <std::pair <int32_t, int32_t>> ratings(batch_size, { 0, 0 });
				for (int32_t i = 0; i < batch_size; i++) {
					ratings[i].second = i;
				}
				// run games
				{
					write_progress(0, "running games", is_err);
					int32_t its = 2 * sqrt(batch_size);
					int32_t games = batch_size * its;
					int32_t completed_games = 0;
					for (int32_t i = 0; i < its; i++) {
						std::vector <int32_t> pairings(batch_size);
						for (int32_t j = 0; j < batch_size; j++) {
							do {
								pairings[j] = Random::rs32(0, batch_size - 1);
							} while (pairings[j] == j && batch_size > 1);
						}
						for (int32_t j = 0; j < batch_size; j++) {
							Board board(DEFAULT_START_POS);
							Find_move white(levals[j]);
							white.ENABLE_LEARN_EVAL = true;
							Find_move black(levals[pairings[j]]);
							black.ENABLE_LEARN_EVAL = true;
							while (true) {
								white.push_hash(Board_hash::hash(board));
								black.push_hash(Board_hash::hash(board));
								uint8_t gamestate = game_state(board);
								if (gamestate == GAME_STATE_ALIVE &&
								white.movetable_count(Board_hash::hash(board)) >= 3) {
									gamestate = GAME_STATE_DRAW;
								}
								if (gamestate != GAME_STATE_ALIVE) {
									if (gamestate == GAME_STATE_DRAW) {
										ratings[j].first++;
										ratings[pairings[j]].first++;
									}
									if (gamestate == GAME_STATE_WHITE_WINS) {
										ratings[j].first += 4;
									}
									if (gamestate == GAME_STATE_BLACK_WINS) {
										ratings[pairings[j]].first += 5;
									}
									break;
								}
								if (board.turn() == COLOR_WHITE) {
									Move move = white.find_move(board);
									board = Board(board, move);
								} else {
									Move move = black.find_move(board);
									board = Board(board, move);
								}
							}
							write_progress((++completed_games * 100) / games, "running games", is_err);
						}
					}
				}
				// merge
				{
					write_progress(100, "creating next generation", is_err);
					
				}
				// write to files
				{
					write_progress(100, "writing weights", is_err);
					for (int32_t i = 0; i < batch_size; i++) {
						std::ofstream ouf(m_srcdir + "/subject_" + std::to_string(i),
						std::ofstream::out | std::ofstream::trunc);
						if (!ouf.is_open()) {
							is_err = true;
							write_log("failed to write weights of subject " + std::to_string(i));
							continue;
						}
						levals[i].write_binfile(ouf);
						ouf.close();
					}
				}
				write_log("completed");
				std::cout << std::endl;
			}
			auto wait_for_quit = [&] (std::atomic <bool>& run) -> void {
				std::string buf;
				while (run.load()) {
					std::cin >> buf;
					if (buf == "q" || buf == "quit") {
						run.store(false);
					}
				}
			};
			std::atomic <bool> run(true);
			std::thread wfq_thread(wait_for_quit, std::ref(run));
			while (run.load()) {
				run_generation(std::ref(run));
			}
			run.store(false);
			wfq_thread.join();
			std::cerr << "quitting..." << std::endl;
		}
		
	};
	
} /// namespace ai01
