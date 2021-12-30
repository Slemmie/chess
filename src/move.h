#pragma once

#include <square_index.h>
#include <piece.h>

#include <iostream>

struct Move {
	
	Move(const Square_index& _from, const Square_index& _to, const Piece& _promote_result = Piece()) :
	from(_from),
	to(_to),
	promote_result(_promote_result)
	{ }
	
	Move(const std::string& str) {
		from = Square_index(str.substr(0, 2));
		to = Square_index(str.substr(2, 2));
		if ((int)str.size() == 5) {
			Piece piece = Piece();
			switch (str[5]) {
				case 'Q': piece = Piece(PIECE_QUEE, COLOR_WHITE);
				case 'q': piece = Piece(PIECE_QUEE, COLOR_BLACK);
				case 'R': piece = Piece(PIECE_ROOK, COLOR_WHITE);
				case 'r': piece = Piece(PIECE_ROOK, COLOR_BLACK);
				case 'B': piece = Piece(PIECE_BISH, COLOR_WHITE);
				case 'b': piece = Piece(PIECE_BISH, COLOR_BLACK);
				case 'N': piece = Piece(PIECE_KNIG, COLOR_WHITE);
				case 'n': piece = Piece(PIECE_KNIG, COLOR_BLACK);
			};
			promote_result = piece;
		} else {
			promote_result = Piece();
		}
	}
	
	Move() { }
	
	Square_index from, to;
	Piece promote_result;
	
	inline bool operator == (const Move& oth) {
		return from.rank() == oth.from.rank() &&
		from.file() == oth.from.file() &&
		to.rank() == oth.to.rank() &&
		to.file() == oth.to.file() &&
		promote_result.piece() == oth.promote_result.piece();
	}
	
	friend std::ostream& operator << (std::ostream& stream, const Move& move) {
		stream << move.from.to_string() << " -> " << move.to.to_string();
		if (!move.promote_result.same_piece(PIECE_NONE)) {
			stream << " (promotion: " << move.promote_result.to_string() << ")";
		}
		return stream;
	}
	
};
