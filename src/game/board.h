// board class

#pragma once

#include <string>

#include "bit_board.h"

namespace game {
	
	class Board {
		
	public:
		
		// pass a fen string to initialize a board
		Board(const std::string& _fen);
		
		// reset the board to a given fen string
		void reset(const std::string& _fen);
		
		// get fen string of current position
		std::string fen() const;
		
	private:
		
		
		
	};
	
} /// namespace game
