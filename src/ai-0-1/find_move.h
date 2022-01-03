#pragma once

#include <game.h>
#include <ai-0-1/board_eval.h>
#include <ai-0-1/move_table.h>
#include <ai-0-1/timer.h>

#include <vector>
#include <cassert>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <fstream>
#include <string>
#include <vector>
#include <random>

namespace ai01 {
	
	class Find_move {
		
	public:
		
		uint8_t MAX_DEPTH = 3;
		
		double minmax_capturechains(const Board& board, double alpha = -1e8, double beta = 1e8,
		uint8_t depth = 0) {
			std::vector <Move> moves = board.moves_of_color(board.turn());
			uint8_t gamestate = game_state(board, moves);
			if (gamestate != GAME_STATE_ALIVE) {
				if (gamestate == GAME_STATE_DRAW) {
					return 0.0;
				}
				return gamestate == (gamestate == GAME_STATE_WHITE_WINS ? 1e5 : -1e5) * (1000.0 / (double)depth);
			}
			moves = m_sort_moves(board, moves);
			double result = Eval::eval(board);
			for (const Move& move : moves) {
				if (board[move.to].same_piece(PIECE_NONE)) {
					continue;
				}
				Board nxt = Board(board, move);
				uint32_t hash = Board_hash::hash(nxt);
				m_movetable.push(hash);
				if (m_movetable.count(hash) >= 3) {
					m_movetable.pop(hash);
					result = board.turn() == COLOR_WHITE ?
					std::max(result, 0.0) : std::min(result, 0.0);
					if (board.turn() == COLOR_WHITE && result >= beta) {
						break;
					} else if (board.turn() == COLOR_BLACK && result <= alpha) {
						break;
					}
					if (board.turn() == COLOR_WHITE) {
						alpha = std::max(alpha, result);
					} else {
						beta = std::min(beta, result);
					}
					continue;
				}
				result = board.turn() == COLOR_WHITE ?
				std::max(result, minmax_capturechains(nxt, alpha, beta, depth + 1)) :
				std::min(result, minmax_capturechains(nxt, alpha, beta, depth + 1));
				m_movetable.pop(hash);
				if (board.turn() == COLOR_WHITE && result >= beta) {
					break;
				} else if (board.turn() == COLOR_BLACK && result <= alpha) {
					break;
				}
				if (board.turn() == COLOR_WHITE) {
					alpha = std::max(alpha, result);
				} else {
					beta = std::min(beta, result);
				}
			}
			return result;
		}
		
		double minmax(const Board& board, uint8_t depth = 1, double alpha = -1e8, double beta = 1e8) {
			std::vector <Move> moves = board.moves_of_color(board.turn());
			uint8_t gamestate = game_state(board, moves);
			if (gamestate != GAME_STATE_ALIVE) {
				if (gamestate == GAME_STATE_DRAW) {
					return 0.0;
				}
				return (gamestate == GAME_STATE_WHITE_WINS ? 1e5 : -1e5) * (1000.0 / (double)depth);
			}
			if (depth > MAX_DEPTH) {
				return Eval::eval(board);
				//return minmax_capturechains(board, alpha, beta);
			}
			moves = m_sort_moves(board, moves);
			double result = board.turn() == COLOR_WHITE ? -1e8 : 1e8;
			bool ischeck = is_check(board, board.turn());
			for (const Move& move : moves) {
				if ((depth & 1) && board[move.from].same_piece(PIECE_KING) &&
				board[move.to].same_piece(PIECE_NONE) &&
				abs((signed int)move.from.file() - (signed int)move.to.file()) != 2 &&
				board.fullmoves() < 18 && !ischeck && (int)moves.size() < 10) {
					continue;
				}
				Board nxt = Board(board, move);
				uint32_t hash = Board_hash::hash(nxt);
				m_movetable.push(hash);
				if (m_movetable.count(hash) >= 3) {
					m_movetable.pop(hash);
					result = board.turn() == COLOR_WHITE ?
					std::max(result, 0.0) : std::min(result, 0.0);
					if (board.turn() == COLOR_WHITE && result >= beta) {
						break;
					} else if (board.turn() == COLOR_BLACK && result <= alpha) {
						break;
					}
					if (board.turn() == COLOR_WHITE) {
						alpha = std::max(alpha, result);
					} else {
						beta = std::min(beta, result);
					}
					continue;
				}
				double result_now = result;
				auto dp_it = m_dp_table.find(hash);
				if (dp_it != m_dp_table.end() && dp_it->second.lowest_depth <= depth + 1) {
					result_now = dp_it->second.result;
				} else {
					result_now = minmax(nxt, depth + 1, alpha, beta);
				}
				result = board.turn() == COLOR_WHITE ?
				std::max(result, result_now) : std::min(result, result_now);
				if (dp_it == m_dp_table.end() || dp_it->second.lowest_depth > depth + 1) {
					m_dp_table[hash] = Hash_move_value(depth + 1, result_now);
				}
				m_movetable.pop(hash);
				if (board.turn() == COLOR_WHITE && result >= beta) {
					break;
				} else if (board.turn() == COLOR_BLACK && result <= alpha) {
					break;
				}
				if (board.turn() == COLOR_WHITE) {
					alpha = std::max(alpha, result);
				} else {
					beta = std::min(beta, result);
				}
			}
			return result;
		}
		
		Move find_move(const Board& board) {
			std::cout << std::setprecision(2) << std::fixed;
			m_dp_table.clear();
			Timer timer;
			std::vector <Move> moves = board.moves_of_color(board.turn());
			uint8_t gamestate = game_state(board, moves);
			assert(gamestate == GAME_STATE_ALIVE);
			if (board.fullmoves() < 15 &&
			m_book_map.find(Board_hash::hash(board)) != m_book_map.end()) {
				if (board.turn() == COLOR_WHITE) {
					Move move = m_book_map[Board_hash::hash(board)].move_by_score.rbegin()->second;
					bool exists = false;
					for (const Move& m : moves) {
						exists |= move == m;
					}
					if (exists) {
						std::cout << "book move found - evalutation: " <<
						Eval::eval(Board(board, move)) << std::endl;
						return move;
					}
				} else {
					Move move = m_book_map[Board_hash::hash(board)].move_by_score.begin()->second;
					bool exists = false;
					for (const Move& m : moves) {
						exists |= move == m;
					}
					if (exists) {
						std::cout << "book move found - evalutation: " <<
						Eval::eval(Board(board, move)) << std::endl;
						return move;
					}
				}
			}
			moves = m_sort_moves(board, moves);
			double result = board.turn() == COLOR_WHITE ? -1e8 : 1e8;
			Move best_move = moves[0];
			bool ischeck = is_check(board, board.turn());
			double alpha = -1e8, beta = 1e8;
			for (const Move& move : moves) {
				if (board[move.from].same_piece(PIECE_KING) &&
				board[move.to].same_piece(PIECE_NONE) &&
				abs((signed int)move.from.file() - (signed int)move.to.file()) != 2 &&
				board.fullmoves() < 18 && !ischeck && (int)moves.size() < 10) {
					continue;
				}
				Board nxt = Board(board, move);
				uint32_t hash = Board_hash::hash(nxt);
				m_movetable.push(hash);
				double value = 0.0;
				if (m_movetable.count(hash) < 3) {
					value = minmax(nxt, 1, alpha, beta);
					auto dp_it = m_dp_table.find(hash);
					if (dp_it == m_dp_table.end() || dp_it->second.lowest_depth > 1) {
						m_dp_table[hash] = Hash_move_value(1, value);
					}
				}
				if (board.turn() == COLOR_WHITE && value > result) {
					result = value;
					best_move = move;
				} else if (board.turn() == COLOR_BLACK && value < result) {
					result = value;
					best_move = move;
				}
				m_movetable.pop(hash);
				if (board.turn() == COLOR_WHITE) {
					alpha = std::max(alpha, result);
				} else {
					beta = std::min(beta, result);
				}
			}
			int64_t micro = timer.current();
			static int64_t prev_micro = 1LL << 60;
			if (micro < 1'500 * 1000 && prev_micro < 2'000 * 1000) {
				MAX_DEPTH++;
			}
			if (micro > 20'000 * 1000 && MAX_DEPTH > 1) {
				MAX_DEPTH--;
			}
			prev_micro = micro;
			std::cout << "result evalutation: " <<
			result << "    (in " << micro / 1000 << " ms)" << std::endl;
			return best_move;
		}
		
		inline void push_hash(uint32_t hash) {
			m_movetable.push(hash);
		}
		
		inline uint8_t movetable_count(uint32_t hash) {
			return m_movetable.count(hash);
		}
		
		void read_book(const std::string& filepath) {
			std::ifstream inf(filepath);
			if (!inf.is_open()) {
				std::cerr << "error opening file '" << filepath << "'" << std::endl;
				std::cerr << "was not able to read book" << std::endl;
				return;
			}
			std::map <std::pair <uint32_t, std::string>, int32_t> mp;
			std::string line;
			while (std::getline(inf, line)) {
				Board board(DEFAULT_START_POS, false);
				std::vector <std::string> moves;
				std::string running;
				for (char c : line) {
					if (c == ' ') {
						moves.push_back(running);
						running.clear();
					} else {
						running += c;
					}
				}
				if ((int)moves.size() >= 2 && moves[0] == "e2e4" && moves[1] != "e7e5") {
					continue;
				}
				assert(running == "1-0" || running == "0-1" || running == "1/2-1/2");
				uint8_t match_result = 0;
				if (running == "1-0") {
					match_result = 1;
				} else if (running == "0-1") {
					match_result = 2;
				}
				for (const auto& move : moves) {
					uint32_t hash = Board_hash::hash(board);
					mp.insert({ std::make_pair(hash, move), 0 });
					if (match_result == 1) {
						mp[{ hash, move }]++;
					} else if (match_result == 2) {
						mp[{ hash, move }]--;
					}
					board = Board(board, Move(move));
				}
			}
			inf.close();
			for (auto p : mp) {
				static std::random_device device;
				static std::mt19937 generator(device());
				static std::uniform_int_distribution <int> distribution(0, 1 << 30);
				if (p.second > 8) {
					p.second += distribution(generator) % (p.second / 2) - (p.second / 4);
				}
				if (m_book_map[p.first.first].move_by_score.count(p.second)) {
					if (distribution(generator) % 2 == 0) {
						continue;
					}
				}
				m_book_map[p.first.first].move_by_score[p.second] = Move(p.first.second);
			}
		}
		
	private:
		
		Move_table m_movetable;
		
		struct Hash_move_value {
			
			Hash_move_value(uint8_t _lowest_depth = 255, double _result = 100.0) :
			lowest_depth(_lowest_depth),
			result(_result)
			{ }
			
			uint8_t lowest_depth;
			double result;
			
		};
		
		std::unordered_map <uint32_t, Hash_move_value> m_dp_table;
		
		struct Book_value {
			
			std::map <int32_t, Move> move_by_score;
			
		};
		
		std::map <uint32_t, Book_value> m_book_map;
		
		std::vector <Move> m_sort_moves(const Board& board, const std::vector <Move>& moves) {
			std::vector <std::pair <double, size_t>> order(moves.size(), { 0, 0 });
			for (size_t i = 0; i < moves.size(); i++) {
				order[i].second = i;
				if (!board[moves[i].to].same_piece(PIECE_NONE)) {
					double from_val = Eval::piece_value(board[moves[i].from].piece());
					double to_val = Eval::piece_value(board[moves[i].to].piece());
					from_val = fabs(from_val);
					to_val = fabs(to_val);
					order[i].first += 10.0 * to_val - from_val;
				}
				if (board[moves[i].from].same_piece(PIECE_PAWN) &&
				(moves[i].to.rank() == 0 || moves[i].to.rank() == 7)) {
					order[i].first += 2.0 * fabs(Eval::piece_value(moves[i].promote_result.piece()));
				}
				uint8_t rank = moves[i].to.rank();
				uint8_t file = moves[i].to.file();
				bool attacked_by_pawn = false;
				if (board[moves[i].from].same_color(COLOR_BLACK) && rank != 7) {
					if (file != 0 && board[rank + 1][file - 1].same_piece(PIECE_PAWN) &&
					board[rank + 1][file - 1].same_color(COLOR_WHITE)) {
						attacked_by_pawn = true;
					}
					if (file != 7 && board[rank + 1][file + 1].same_piece(PIECE_PAWN) &&
					board[rank + 1][file + 1].same_color(COLOR_WHITE)) {
						attacked_by_pawn = true;
					}
				} else if (board[moves[i].from].same_color(COLOR_WHITE) && rank != 0) {
					if (file != 0 && board[rank - 1][file - 1].same_piece(PIECE_PAWN) &&
					board[rank - 1][file - 1].same_color(COLOR_BLACK)) {
						attacked_by_pawn = true;
					}
					if (file != 7 && board[rank - 1][file + 1].same_piece(PIECE_PAWN) &&
					board[rank - 1][file + 1].same_color(COLOR_BLACK)) {
						attacked_by_pawn = true;
					}
				}
				if (attacked_by_pawn) {
					order[i].first -= fabs(Eval::piece_value(board[moves[i].from].piece()));
				}
				if (board[moves[i].from].same_piece(PIECE_KING) &&
				abs((signed int)moves[i].from.file() - (signed int)moves[i].to.file()) > 1) {
					order[i].first += 0.5;
				}
				order[i].first += Eval::piece_position_value(board,
				moves[i].from.rank(), moves[i].to.file(),
				moves[i].to.rank(), moves[i].to.file());
			}
			std::random_shuffle(order.begin(), order.end());
			std::sort(order.rbegin(), order.rend());
			std::vector <Move> result(moves.size());
			for (size_t i = 0; i < moves.size(); i++) {
				result[i] = moves[order[i].second];
			}
			return result;
		}
		
	};
	
} /// namespace ai01
