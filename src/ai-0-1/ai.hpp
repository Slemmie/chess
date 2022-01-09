#include <util/timer.h>

#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

namespace ai01 {
	
	template <class Eval_class>
	AI <Eval_class>::AI(const Eval_class& _eval_class, const std::string& _book_filepath) :
	m_eval(_eval_class),
	m_move_table(),
	m_book(_book_filepath)
	{ }
	
	template <class Eval_class>
	AI <Eval_class>::AI(const std::string& _book_filepath) :
	m_eval(),
	m_move_table(),
	m_book(_book_filepath)
	{ }
	
	template <class Eval_class>
	Move AI <Eval_class>::find_move(const Board& board, uint64_t timeout_millis, bool log_info) {
		uint32_t hash = Board_hash::hash(board);
		assert(m_move_table.count(hash) < 3);
		
		if (board.fullmoves() < MAX_BOOK_DEPTH && m_book.count(hash)) {
			if (log_info) {
				std::cout << "book move found" << std::endl;
			}
			return m_book.find(hash);
		}
		
		m_dp_map.clear();
		if (m_q_dp_map.size() > size_t(1e6)) {
			m_q_dp_map.clear();
		}
		
		m_timer = Timer();
		m_timed_out = false;
		m_best_move = Move();
		m_best_eval = 0;
		
		Move result_move = m_best_move;
		double result_eval = m_best_eval;
		
		for (uint32_t depth = 1; !m_timed_out; depth++) {
			m_minmax(board, -1e8, 1e8, depth, 0, timeout_millis);
			if (m_timed_out) {
				break;
			}
			result_move = m_best_move;
			result_eval = m_best_eval * (board.turn() == COLOR_WHITE ? 1 : -1);
			if (log_info) {
				std::cout << "\r" << std::string(32, ' ');
				std::cout << "\rdepth: " << depth << " (";
				if (fabs(result_eval) >= 1e5 - 1e3) {
					int mate_in = 1e5 - fabs(result_eval);
					mate_in = (mate_in + 1) >> 1;
					std::cout << "M" << mate_in << ")";
				} else {
					std::cout << std::setprecision(2) << std::fixed << result_eval << ")";
				}
				std::cout.flush();
			}
			if (fabs(result_eval) >= 1e5 - 1e3) {
				break;
			}
		}
		
		return result_move;
	}
	
	template <class Eval_class>
	void AI <Eval_class>::push_hash(const uint32_t hash) {
		m_move_table.push(hash);
	}
	
	template <class Eval_class>
	double AI <Eval_class>::m_minmax(const Board& board, double alpha, double beta,
	uint32_t depth, uint32_t from_root, uint64_t timeout_millis) {
		if (m_timed_out |= m_timer.current() >= timeout_millis * 1000) {
			return 0;
		}
		
		uint32_t hash = Board_hash::hash(board);
		
		m_move_table.push(hash);
		
		if (from_root > 0) {
			if (m_move_table.count(hash) >= 3) {
				m_move_table.pop(hash);
				return 0;
			}
			alpha = std::max(alpha, -1e5 + from_root);
			beta = std::min(beta, 1e5 - from_root);
			if (alpha >= beta) {
				m_move_table.pop(hash);
				return alpha;
			}
		}
		
		if (m_dp_map.count(hash, depth)) {
			m_move_table.pop(hash);
			return m_dp_map.find(hash, depth);
		}
		
		if (depth == 0) {
			m_move_table.pop(hash);
			double eval = m_quiescence_minmax(board, alpha, beta, timeout_millis);
			return eval;
			//return m_eval.eval(board) * (board.turn() == COLOR_WHITE ? 1 : -1);
		}
		
		std::vector <Move> moves = m_sort_moves(board, board.moves_of_color(board.turn()));
		uint8_t gamestate = game_state(board, moves);
		if (gamestate != GAME_STATE_ALIVE) {
			m_move_table.pop(hash);
			if (gamestate == GAME_STATE_DRAW) {
				return 0;
			}
			return -(1e5 - from_root);
		}
		
		for (const Move& move : moves) {
			Board next_board = Board(board, move);
			double eval = -m_minmax(next_board, -beta, -alpha, depth - 1, from_root + 1, timeout_millis);
			if (eval >= beta) {
				m_dp_map.insert(beta, hash, depth);
				m_move_table.pop(hash);
				return beta;
			}
			if (eval > alpha) {
				alpha = eval;
				if (from_root == 0) {
					m_best_move = move;
					m_best_eval = eval;
				}
			}
		}
		
		m_dp_map.insert(alpha, hash, depth);
		
		m_move_table.pop(hash);
		
		return alpha;
	}
	
	template <class Eval_class>
	double AI <Eval_class>::m_quiescence_minmax(const Board& board, double alpha, double beta, uint64_t timeout_millis) {
		if (m_timed_out |= m_timer.current() >= timeout_millis * 1000) {
			return 0;
		}
		
		uint32_t hash = Board_hash::hash(board);
		
		if (m_q_dp_map.count(hash, 0)) {
			return m_q_dp_map.find(hash, 0);
		}
		
		double eval = m_eval.eval(board) * (board.turn() == COLOR_WHITE ? 1 : -1);
		if (eval >= beta) {
			return beta;
		}
		alpha = std::max(alpha, eval);
		
		std::vector <Move> moves = m_sort_moves(board, board.moves_of_color(board.turn()));
		for (const Move& move : moves) {
			if (board[move.to].same_piece(PIECE_NONE)) {
				continue;
			}
			eval = -m_quiescence_minmax(Board(board, move), -beta, -alpha, timeout_millis);
			if (eval >= beta) {
				m_q_dp_map.insert(beta, hash, 0);
				return beta;
			}
			alpha = std::max(alpha, eval);
		}
		
		m_q_dp_map.insert(alpha, hash, 0);
		
		return alpha;
	}
	
	template <class Eval_class>
	void AI <Eval_class>::m_update_alpha_beta(double value, uint8_t turn, double& alpha, double& beta) const {
		turn == COLOR_WHITE ? alpha = std::max(alpha, value) : beta = std::min(beta, value);
	}
	
	template <class Eval_class>
	std::vector <Move> AI <Eval_class>::m_sort_moves(const Board& board, const std::vector <Move>& moves) const {
		std::vector <std::pair <double, size_t>> order(moves.size(), { 0, 0 });
		
		for (size_t i = 0; i < moves.size(); i++) {
			order[i].second = i;
			
			if (!board[moves[i].to].same_piece(PIECE_NONE)) {
				double from_val = fabs(m_eval.piece_value(board[moves[i].from].piece()));
				double to_val = fabs(m_eval.piece_value(board[moves[i].to].piece()));
				order[i].first += 10.0 * to_val - from_val;
			}
			
			if (board[moves[i].from].same_piece(PIECE_PAWN) &&
			(moves[i].to.rank() == 0 || moves[i].to.rank() == 7)) {
				order[i].first += 2.0 * fabs(m_eval.piece_value(moves[i].promote_result.piece()));
			}
			
			uint8_t to_rank = moves[i].to.rank();
			uint8_t to_file = moves[i].to.file();
			bool attacked_by_pawn = false;
			if (board[moves[i].from].same_color(COLOR_BLACK) && to_rank != 7) {
				if (to_file != 0 && board[to_rank + 1][to_file - 1].same_piece(PIECE_PAWN) &&
				board[to_rank + 1][to_file - 1].same_color(COLOR_WHITE)) {
					attacked_by_pawn = true;
				} else if (to_file != 7 && board[to_rank + 1][to_file + 1].same_piece(PIECE_PAWN) &&
				board[to_rank + 1][to_file + 1].same_color(COLOR_WHITE)) {
					attacked_by_pawn = true;
				}
			} else if (board[moves[i].from].same_color(COLOR_WHITE) && to_rank != 0) {
				if (to_file != 0 && board[to_rank - 1][to_file - 1].same_piece(PIECE_PAWN) &&
				board[to_rank - 1][to_file - 1].same_color(COLOR_BLACK)) {
					attacked_by_pawn = true;
				} else if (to_file != 7 && board[to_rank - 1][to_file + 1].same_piece(PIECE_PAWN) &&
				board[to_rank - 1][to_file + 1].same_color(COLOR_BLACK)) {
					attacked_by_pawn = true;
				}
			}
			if (attacked_by_pawn) {
				order[i].first -= fabs(m_eval.piece_value(board[moves[i].from].piece()));
			}
			
			if (board[moves[i].from].same_piece(PIECE_KING) &&
			abs((signed int)moves[i].from.file() - (signed int)moves[i].to.file()) > 1) {
				order[i].first += 0.5;
			}
		}
		
		std::random_shuffle(order.begin(), order.end());
		std::sort(order.rbegin(), order.rend());
		std::vector <Move> result(moves.size());
		for (size_t i = 0; i < moves.size(); i++) {
			result[i] = moves[order[i].second];
		}
		
		return result;
	}
	
} /// namespace ai01
