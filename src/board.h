#pragma once

#include <square_index.h>
#include <move.h>
#include <piece.h>

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <string>

#define DEFAULT_START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

class Board {
	
public:
	
	Board();
	Board(const std::string& _FEN, bool log_fenstring = true);
	Board(const Board& _board, const Move& _move);
	
	std::vector <Move> moves_of_color(uint8_t color) const;
	std::vector <Move> moves_of_square(const Square_index& square) const;
	
	inline bool castle_BK() const {
		return m_castle_BK;
	}
	inline bool castle_BQ() const {
		return m_castle_BQ;
	}
	inline bool castle_WK() const {
		return m_castle_WK;
	}
	inline bool castle_WQ() const {
		return m_castle_WQ;
	}
	inline const Square_index& en_pessant_square() const {
		return m_en_pessant_square;
	}
	
	inline const std::vector <std::vector <Piece>> pieces() const {
		return m_pieces;
	}
	
	inline const uint32_t halfmoves() const {
		return m_halfmoves;
	}
	inline const uint32_t fullmoves() const {
		return m_fullmoves;
	}
	inline const uint8_t turn() const {
		return m_turn;
	}
	
	inline const std::vector <Piece>& operator [] (const uint8_t index) const {
		return m_pieces[index];
	}
	
	inline const Piece& operator [] (const Square_index& square) const {
		return m_pieces[square.rank()][square.file()];
	}
	
	friend std::ostream& operator << (std::ostream& stream, const Board& board) {
		static std::map <uint8_t, std::string> mp = {
			{ PIECE_NONE, "." },
			{ PIECE_PAWN | COLOR_WHITE, "\033[1;97mP\033[0m" },
			{ PIECE_PAWN | COLOR_BLACK, "\033[1;90mP\033[0m" },
			{ PIECE_ROOK | COLOR_WHITE, "\033[1;97mR\033[0m" },
			{ PIECE_ROOK | COLOR_BLACK, "\033[1;90mR\033[0m" },
			{ PIECE_KNIG | COLOR_WHITE, "\033[1;97mN\033[0m" },
			{ PIECE_KNIG | COLOR_BLACK, "\033[1;90mN\033[0m" },
			{ PIECE_BISH | COLOR_WHITE, "\033[1;97mB\033[0m" },
			{ PIECE_BISH | COLOR_BLACK, "\033[1;90mB\033[0m" },
			{ PIECE_QUEE | COLOR_WHITE, "\033[1;97mQ\033[0m" },
			{ PIECE_QUEE | COLOR_BLACK, "\033[1;90mQ\033[0m" },
			{ PIECE_KING | COLOR_WHITE, "\033[1;97mK\033[0m" },
			{ PIECE_KING | COLOR_BLACK, "\033[1;90mK\033[0m" },
		};
		
		for (int rank = 0; rank < 8; rank++) {
			for (int file = 0; file < 8; file++) {
				if (mp[board[rank][file].piece()] == ".") {
					stream << " \033[1;" << ((rank + file) & 1 ? "92" : "94") << "m.\033[0m" <<
					(file == 7 ? "" : "  ");
					continue;
				}
				stream << " " << mp[board[rank][file].piece()] << (file == 7 ? "" : "  ");
			}
			stream << "\n\n";
		}
		stream << "         " << (board.turn() == COLOR_WHITE ? "white to move\n" : "black to move\n");
		return stream;
	}
	
private:
	
	inline std::vector <Piece>& operator [] (const uint8_t index) {
		return m_pieces[index];
	}
	
	inline Piece& operator [] (const Square_index& square) {
		return m_pieces[square.rank()][square.file()];
	}
	
	std::vector <std::vector <Piece>> m_pieces;
	
	Square_index m_en_pessant_square;
	bool m_castle_BK, m_castle_BQ;
	bool m_castle_WK, m_castle_WQ;
	
	uint32_t m_halfmoves, m_fullmoves;
	uint8_t m_turn;
	
};
