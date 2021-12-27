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
