#pragma once

#include <game-0-1/game.h>
#include <util/timer.h>
#include <ai-0-1/move_table.h>
#include <ai-0-1/book_moves.h>
#include <ai-0-1/dp_map.h>
#include <ai-0-1/default_eval.h>

#include <string>
#include <memory>

namespace ai01 {
	
	template <class Eval_class = Default_eval> class AI {
		
	public:
		
		uint32_t MAX_BOOK_DEPTH = 15;
		
		AI(const Eval_class& _eval_class, const std::string& _book_filepath);
		AI(const std::string& _book_filepath);
		AI(const Eval_class& _eval_class, std::shared_ptr <Book_moves> _book);
		AI(std::shared_ptr <Book_moves> _book);
		
		Move find_move(const Board& board, uint64_t timeout_millis = 5000, bool log_info = false);
		
		void push_hash(const uint32_t hash);
		
	private:
		
		Eval_class m_eval;
		
		Move_table m_move_table;
		
		std::shared_ptr <Book_moves> m_book;
		
		Dp_map m_dp_map;
		Dp_map m_q_dp_map;
		
	private:
		
		Timer m_timer;
		bool m_timed_out;
		Move m_best_move;
		double m_best_eval;
		std::vector <Move> m_last_evals, m_last_evals_this_it;
		double m_minmax(const Board& board, double alpha, double beta,
		uint32_t depth, uint32_t from_root, uint64_t timeout_millis);
		double m_quiescence_minmax(const Board& board, double alpha, double beta, uint64_t timeout_millis);
		
		std::vector <Move> m_sort_moves(const Board& board, const std::vector <Move>& moves) const;
		
	};
	
} /// namespace ai01

#include <ai-0-1/ai.hpp>
