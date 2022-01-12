#pragma once

#include <game.h>
#include <ai-0-1/ai.h>
#include <ai-0-1/learn/eval.h>
#include <util/random.h>
#include <util/concat.h>
#include <util/timer.h>
#include <util/thread_pool.h>

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <memory>

namespace ai01 {
	
	struct Launch_game {
		struct Game_data {
			AI <Learn_eval> white;
			AI <Learn_eval> black;
			uint64_t millis_per_move = 100;
		};
		void operator () (int id, Game_data game_data,
		std::atomic <int32_t>& score1, std::atomic <int32_t>& score2,
		std::atomic <int32_t>& completed_games, std::atomic <int32_t>& a_index) const {
			a_index++;
			AI <Learn_eval>& white = game_data.white;
			AI <Learn_eval>& black = game_data.black;
			Board board(DEFAULT_START_POS, false);
			Move_table move_table;
			while (true) {
				uint32_t hash = Board_hash::hash(board);
				move_table.push(hash);
				white.push_hash(hash);
				black.push_hash(hash);
				uint8_t gamestate = game_state(board);
				if (gamestate == GAME_STATE_ALIVE &&
				move_table.count(Board_hash::hash(board)) >= 3) {
					gamestate = GAME_STATE_DRAW;
				}
				// prevent games from being too long - can be removed
				if (gamestate == GAME_STATE_ALIVE &&
				board.fullmoves() > 200) {
					gamestate = GAME_STATE_DRAW;
				}
				if (gamestate != GAME_STATE_ALIVE) {
					if (gamestate == GAME_STATE_DRAW) {
						score1 = score2 = 1;
					}
					if (gamestate == GAME_STATE_WHITE_WINS) {
						score1 = 4;
					}
					if (gamestate == GAME_STATE_BLACK_WINS) {
						score2 = 5;
					}
					break;
				}
				if (board.turn() == COLOR_WHITE) {
					Move move = white.find_move(board, game_data.millis_per_move);
					board = Board(board, move);
				} else {
					Move move = black.find_move(board, game_data.millis_per_move);
					board = Board(board, move);
				}
			}
			completed_games++;
		}
	};
	
	class Train {
		
	public:
		
		Train(const std::string& _srcdir, int32_t _id, std::shared_ptr <Book_moves> _book) :
		m_srcdir(concat(_srcdir, "/ai-0-1/learn/batches/", std::to_string(_id))),
		m_id(_id),
		m_book(_book)
		{
			if (std::filesystem::exists(m_srcdir)) {
				std::cout << "'" << m_srcdir << "' already exists" << std::endl;
				std::cout << "  (1): terminate initialization" << std::endl;
				std::cout << "  (2): override batch" << std::endl;
				std::cout << "  (3): continue batch" << std::endl;
				std::string inp;
				std::cin >> inp;
				while (inp != "1" && inp != "2" && inp != "3") {
					std::cout << "bad input" << std::endl;
					std::cin >> inp;
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
		
		std::string m_srcdir;
		int32_t m_id;
		
		std::shared_ptr <Book_moves> m_book;
		
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
			std::cin >> inp;
			while (!m_is_int(inp)) {
				std::cout << "bad input\nbatch size > ";
				std::cin >> inp;
			}
			ouf << "batch_size " << inp << "\n";
			std::cout << "mutation rate > ";
			std::cin >> inp;
			while (!m_is_float(inp)) {
				std::cout << "bad input\nmutation rate > ";
				std::cin >> inp;
			}
			ouf << "mutation_rate " << inp << "\n";
			std::cout << "millis per move > ";
			std::cin >> inp;
			while (!m_is_int(inp)) {
				std::cout << "bad input\nmillis per move > ";
				std::cin >> inp;
			}
			ouf << "millis_per_move " << inp << "\n";
			std::cout << "book depth > ";
			std::cin >> inp;
			while (!m_is_int(inp)) {
				std::cout << "book depth > ";
				std::cin >> inp;
			}
			ouf << "book_depth " << inp << "\n";
			ouf << "generation 0\n";
			ouf.close();
		}
		
		void m_continue_batch() {
			int32_t generation = 0; bool found_generation_field = false;
			int32_t batch_size = 0; bool found_batch_size_field = false;
			double mutation_rate = 0; bool found_mutation_rate_field = false;
			uint64_t millis_per_move = 0; bool found_millis_per_move_field = false;
			uint32_t book_depth = 0; bool found_book_depth_field = false;
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
					found_generation_field = true;
				} else if (line == "batch_size") {
					inf >> line;
					batch_size = std::stoi(line);
					found_batch_size_field = true;
				} else if (line == "mutation_rate") {
					inf >> line;
					mutation_rate = std::stod(line);
					found_mutation_rate_field = true;
				} else if (line == "millis_per_move") {
					inf >> line;
					millis_per_move = std::stoull(line);
					found_millis_per_move_field = true;
				} else if (line == "book_depth") {
					inf >> line;
					book_depth = std::stoi(line);
					found_book_depth_field = true;
				}
			}
			inf.close();
			if (!found_generation_field) {
				std::cerr << "missing field 'generation' from conf file" << std::endl;
				return;
			}
			if (!found_batch_size_field) {
				std::cerr << "missing field 'batch_size' from conf file" << std::endl;
				return;
			}
			if (!found_mutation_rate_field) {
				std::cerr << "missing field 'mutation_rate' from conf file" << std::endl;
				return;
			}
			if (!found_millis_per_move_field) {
				std::cerr << "missing field 'millis_per_move' from conf file" << std::endl;
				return;
			}
			if (!found_book_depth_field) {
				std::cerr << "missing field 'book_depth' from conf file" << std::endl;
				return;
			}
			Thread_pool tp;
			std::vector <std::future <void>> tp_res(batch_size * 2 * (int)sqrt(batch_size));
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
					int ind = str.size();
					str += std::string(50, '.');
					str += "]";
					for (int i = 0; i < 50; i++) {
						if (i < prc / 2) {
							str[ind + i] = '#';
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
					std::cout << "\r" << std::string(str.size() + 32, ' ');
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
					int32_t its = 2 * (int)sqrt(batch_size);
					int32_t games = batch_size * its;
					std::atomic <int32_t> completed_games(0);
					std::atomic <int32_t> a_index(0);
					write_progress(0, concat("running games (", completed_games, "/", games, ")"), is_err);
					std::vector <std::pair <int32_t, int32_t>> all_pairs;
					std::vector <std::pair <std::atomic <int32_t>, std::atomic <int32_t>>>
					all_results(its * batch_size);
					std::vector <Launch_game::Game_data> game_data;
					for (int32_t i = 0; i < its; i++) {
						std::vector <int32_t> pairings(batch_size);
						for (int32_t j = 0; j < batch_size; j++) {
							do {
								pairings[j] = Random::rs32(0, batch_size - 1);
							} while (pairings[j] == j && batch_size > 1);
							all_pairs.emplace_back(j, pairings[j]);
							all_results[i * batch_size + j].first = 0;
							all_results[i * batch_size + j].second = 0;
						}
						for (int32_t j = 0; j < batch_size; j++) {
							game_data.push_back({
							AI <Learn_eval> (levals[j], m_book),
							AI <Learn_eval> (levals[pairings[j]], m_book),
							millis_per_move });
							game_data.back().white.MAX_BOOK_DEPTH = book_depth;
							game_data.back().black.MAX_BOOK_DEPTH = book_depth;
						}
					}
					Launch_game lg;
					for (int32_t i = 0; i < its; i++) {
						for (int32_t j = 0; j < batch_size; j++) {
							tp_res[i * batch_size + j] =
							tp.push(std::ref(lg), game_data[i * batch_size + j],
							std::ref(all_results[i * batch_size + j].first),
							std::ref(all_results[i * batch_size + j].second),
							std::ref(completed_games), std::ref(a_index));
						}
					}
					while (completed_games < games) {
						write_progress((completed_games * 100) / games,
						concat("running games (", completed_games, "/", games, ")"), is_err);
						Timer timer;
						while (timer.current() < 100 * 1000);
					}
					for (int32_t i = 0; i < its; i++) {
						for (int32_t j = 0; j < batch_size; j++) {
							tp_res[i * batch_size + j].get();
						}
					}
					for (int32_t i = 0; i < its; i++) {
						for (int32_t j = 0; j < batch_size; j++) {
							ratings[all_pairs[i * batch_size + j].first].first +=
							all_results[i * batch_size + j].first;
							ratings[all_pairs[i * batch_size + j].second].first +=
							all_results[i * batch_size + j].second;
						}
					}
					write_progress(100 / games,
					concat("running games (", completed_games, "/", games, ")"), is_err);
				}
				// merge
				{
					write_progress(100, "creating next generation", is_err);
					std::sort(ratings.rbegin(), ratings.rend());
					std::vector <Learn_eval> nxt_levals;
					for (size_t i = 0; i < (levals.size() + 1) / 2; i++) {
						nxt_levals.emplace_back(levals[ratings[i].second]);
					}
					int left = 0, right = (int)nxt_levals.size() - 1;
					for (size_t i = nxt_levals.size(); i < levals.size(); i++) {
						std::string dir = m_srcdir + "/" + std::to_string(Random::rs32(1 << 10, 1 << 20));
						std::filesystem::create_directory(dir);
						nxt_levals.emplace_back(Learn_eval(
						nxt_levals[Random::rs32(left, right)],
						nxt_levals[Random::rs32(left, right)],
						mutation_rate,
						dir));
						std::filesystem::remove_all(dir);
					}
					std::swap(levals, nxt_levals);
				}
				// write to files
				{
					write_progress(100, "writing weights", is_err);
					for (int32_t i = 0; i < batch_size; i++) {
						std::ofstream ouf(m_srcdir + "/subject_" + std::to_string(i),
						std::ofstream::out | std::ofstream::trunc | std::ios::binary);
						if (!ouf.is_open()) {
							is_err = true;
							write_log("failed to write weights of subject " + std::to_string(i));
							continue;
						}
						levals[i].write_binfile(ouf);
						ouf.close();
					}
					write_progress(100, "updating conf file", is_err);
					std::ofstream ouf(m_srcdir + "/conf", std::ofstream::out | std::ofstream::trunc);
					if (!ouf.is_open()) {
						write_log("failed to update conf file (generation " +
						std::to_string(generation) + ")");
					} else {
						ouf << "batch_size " << batch_size << "\n";
						ouf << "mutation_rate " << mutation_rate << "\n";
						ouf << "millis_per_move " << millis_per_move << "\n";
						ouf << "book_depth " << book_depth << "\n";
						ouf << "generation " << generation << "\n";
						ouf.close();
					}
				}
				write_progress(100, "completed", is_err);
				write_log("completed");
				std::cout << std::endl;
			};
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
