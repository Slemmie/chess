#pragma once

#include <square_index.h>

#include <vector>
#include <string>

#define COLOR_NONE    (0 << 0)
#define COLOR_WHITE   (1 << 0)
#define COLOR_BLACK   (1 << 1)

#define PIECE_NONE    (1 << 2)
#define PIECE_PAWN    (2 << 2)
#define PIECE_ROOK    (3 << 2)
#define PIECE_KNIG    (4 << 2)
#define PIECE_BISH    (5 << 2)
#define PIECE_QUEE    (6 << 2)
#define PIECE_KING    (7 << 2)

class Piece {
	
public:
	
	Piece() :
	m_id(COLOR_NONE | PIECE_NONE)
	{ }
	
	Piece(uint8_t _piece, uint8_t _color) :
	m_id(_piece | _color)
	{ }
	
	uint8_t piece() const {
		return m_id;
	}
	
	std::string to_string() const {
		if (same_piece(PIECE_NONE)) {
			return "no_piece";
		}
		if (same_piece(PIECE_PAWN)) {
			return "pawn";
		}
		if (same_piece(PIECE_ROOK)) {
			return "rook";
		}
		if (same_piece(PIECE_KNIG)) {
			return "knight";
		}
		if (same_piece(PIECE_BISH)) {
			return "bishup";
		}
		if (same_piece(PIECE_QUEE)) {
			return "queen";
		}
		if (same_piece(PIECE_KING)) {
			return "king";
		}
		return "unknown";
	};
	
	inline bool same_color(uint8_t color) const {
		return m_id & color;
	}
	
	inline bool same_piece(uint8_t piece) const {
		return (m_id >> 2) == (piece >> 2);
	}
	
private:
	
	uint8_t m_id;
	
};
