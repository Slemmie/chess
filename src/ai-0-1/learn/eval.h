#pragma once

#include <game.h>

#include <util/random.h>

#include <string>
#include <fstream>
#include <iostream>

namespace ai01 {
	
	class Learn_eval {
		
	public:
		
		Learn_eval() :
		m_piece_vals(),
		m_pst_white(),
		m_pst_black()
		{ }
		
		Learn_eval(const std::string& _filepath) {
			std::ifstream inf(_filepath);
			if (!inf.open()) {
				std::cerr << "failed to open '" << _filepath << "'" << std::endl;
				return;
			}
			m_piece_vals = Piece_values(inf);
			m_pst_white = Piece_square_table(inf);
			m_pst_black = Piece_square_table(inf);
			inf.close();
		}
		
		Learn_eval(const std::string& _piece_square_table_white_filepath,
		const std::string& _piece_square_table_black_filepath,
		const std::string& _piece_vals) :
		m_piece_vals(_piece_vals),
		m_pst_white(_piece_square_table_white_filepath),
		m_pst_black(_piece_square_table_black_filepath)
		{ }
		
		inline void write_binfile(std::ofstream& ouf) const {
			m_piece_vals.write_binfile(ouf);
			m_pst_white.write_binfile(ouf);
			m_pst_black.write_binfile(ouf);
		}
		
		inline void write_pst_white(const std::string& filepath) const {
			m_pst_white.write_binfile(filepath);
		}
		
		inline void write_pst_black(const std::string& filepath) const {
			m_pst_black.write_binfile(filepath);
		}
		
		double eval(const Board& board) const {
			double result = 0;
			double white_material_nopawns = 0;
			double black_material_nopawns = 0;
			for (uint8_t rank = 0; rank < 8; rank++) {
				for (uint8_t file = 0; file < 8; file++) {
					if (board[rank][file].same_piece(PIECE_NONE)) {
						continue;
					}
					if (board[rank][file].same_piece(PIECE_ROOK) ||
					board[rank][file].same_piece(PIECE_KNIG) ||
					board[rank][file].same_piece(PIECE_BISH) ||
					board[rank][file].same_piece(PIECE_QUEE)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							white_material_nopawns +=
							m_piece_vals.piece_values[board[rank][file].piece()];
						} else {
							black_material_nopawns -=
							m_piece_vals.piece_values[board[rank][file].piece()];
						}
					}
				}
			}
			double white_endgame_weight = m_endgame_weight(white_material_nopawns);
			double black_endgame_weight = m_endgame_weight(black_material_nopawns);
			for (int rank = 0; rank < 8; rank++) {
				for (int file = 0; file < 8; file++) {
					if (board[rank][file].same_piece(PIECE_NONE)) {
						continue;
					}
					if (board[rank][file].same_piece(PIECE_PAWN)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight * m_pst_white.pawn_late[rank][file] +
							(1.0 - white_endgame_weight) * m_pst_white.pawn_early[rank][file];
						} else {
							result -= black_endgame_weight * m_pst_black.pawn_late[rank][file] +
							(1.0 - black_endgame_weight) * m_pst_black.pawn_early[rank][file];
						}
					} else if (board[rank][file].same_piece(PIECE_ROOK)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight * m_pst_white.rook_late[rank][file] +
							(1.0 - white_endgame_weight) * m_pst_white.rook_early[rank][file];
						} else {
							result -= black_endgame_weight * m_pst_black.rook_late[rank][file] +
							(1.0 - black_endgame_weight) * m_pst_black.rook_early[rank][file];
						}
					} else if (board[rank][file].same_piece(PIECE_KNIG)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight * m_pst_white.knight_late[rank][file] +
							(1.0 - white_endgame_weight) * m_pst_white.knight_early[rank][file];
						} else {
							result -= black_endgame_weight * m_pst_black.knight_late[rank][file] +
							(1.0 - black_endgame_weight) * m_pst_black.knight_early[rank][file];
						}
					} else if (board[rank][file].same_piece(PIECE_BISH)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight * m_pst_white.bishup_late[rank][file] +
							(1.0 - white_endgame_weight) * m_pst_white.bishup_early[rank][file];
						} else {
							result -= black_endgame_weight * m_pst_black.bishup_late[rank][file] +
							(1.0 - black_endgame_weight) * m_pst_black.bishup_early[rank][file];
						}
					} else if (board[rank][file].same_piece(PIECE_QUEE)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight * m_pst_white.queen_late[rank][file] +
							(1.0 - white_endgame_weight) * m_pst_white.queen_early[rank][file];
						} else {
							result -= black_endgame_weight * m_pst_black.queen_late[rank][file] +
							(1.0 - black_endgame_weight) * m_pst_black.queen_early[rank][file];
						}
					}
					 else if (board[rank][file].same_piece(PIECE_KING)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight * m_pst_white.king_late[rank][file] +
							(1.0 - white_endgame_weight) * m_pst_white.king_early[rank][file];
						} else {
							result -= black_endgame_weight * m_pst_black.king_late[rank][file] +
							(1.0 - black_endgame_weight) * m_pst_black.king_early[rank][file];
						}
					}
				}
			}
			return result;
		}
		
		inline double piece_value(const uint8_t piece) const {
			return m_piece_vals[piece];
		}
		
	private:
		
		inline double m_endgame_weight(const double material_nopawns) const {
			static const double end_mult = 1.0 / (2.0 *
			m_piece_vals.piece_values[PIECE_ROOK | COLOR_WHITE] +
			m_piece_vals.piece_values[PIECE_BISH | COLOR_WHITE] +
			m_piece_vals.piece_values[PIECE_KNIG | COLOR_WHITE]);
			return 1.0 - std::min(1.0, material_nopawns * end_mult);
		}
		
		struct Piece_values {
			Piece_values() {
				for (uint8_t i = 0; i < 32; i++) {
					piece_values[i] = 15.0 * Random::negpos1();
				}
			}
			
			Piece_values(std::ifstream& inf) {
				inf.read(reinterpret_cast <char*> (&piece_values[0]), sizeof(double) * 32);
			}
			
			void write_binfile(std::ofstream& ouf) const {
				for (uint8_t i = 0; i < 8; i++) {
					ouf.write(reinterpret_cast <const char*>
					(&piece_values[i]), sizeof(piece_values[i]));
				}
			}
			
			double piece_values[32];
		} m_piece_vals;
		
		struct Piece_square_table {
			Piece_square_table() {
				for (uint8_t rank = 0; rank < 8; rank++) {
					for (uint8_t file = 0; file < 8; file++) {
						pawn_early[rank][file] = 2.0 * Random::negpos1();
						pawn_late[rank][file] = 2.0 * Random::negpos1();
						rook_early[rank][file] = 2.0 * Random::negpos1();
						rook_late[rank][file] = 2.0 * Random::negpos1();
						knight_early[rank][file] = 2.0 * Random::negpos1();
						knight_late[rank][file] = 2.0 * Random::negpos1();
						bishup_early[rank][file] = 2.0 * Random::negpos1();
						bishup_late[rank][file] = 2.0 * Random::negpos1();
						queen_early[rank][file] = 2.0 * Random::negpos1();
						queen_late[rank][file] = 2.0 * Random::negpos1();
						king_early[rank][file] = 2.0 * Random::negpos1();
						king_late[rank][file] = 2.0 * Random::negpos1();
					}
				}
			}
			
			Piece_square_table(std::ifstream& inf) {
				auto read_grid = [&] (double grid[8][8]) -> void {
					for (uint8_t rank = 0; rank < 8; rank++) {
						inf.read(reinterpret_cast <char*> (&grid[rank][0]), sizeof(double) * 8);
					}
				};
				read_grid(pawn_early);
				read_grid(pawn_late);
				read_grid(rook_early);
				read_grid(rook_late);
				read_grid(knight_early);
				read_grid(knight_late);
				read_grid(bishup_early);
				read_grid(bishup_late);
				read_grid(queen_early);
				read_grid(queen_late);
				read_grid(king_early);
				read_grid(king_late);
			}
			
			void write_binfile(std::ofstream& ouf) const {
				auto write_grid = [&] (double grid[8][8]) -> void {
					for (uint8_t rank = 0; rank < 8; rank++) {
						for (uint8_t file = 0; file < 8; file++) {
							ouf.write(reinterpret_cast <const char*>
							(&grid[rank][file]), sizeof(grid[rank][file]));
						}
					}
				};
				write_grid(pawn_early);
				write_grid(pawn_late);
				write_grid(rook_early);
				write_grid(rook_late);
				write_grid(knight_early);
				write_grid(knight_late);
				write_grid(bishup_early);
				write_grid(bishup_late);
				write_grid(queen_early);
				write_grid(queen_late);
				write_grid(king_early);
				write_grid(king_late);
			}
			
			double pawn_early[8][8];
			double pawn_late[8][8];
			double rook_early[8][8];
			double rook_late[8][8];
			double knight_early[8][8];
			double knight_late[8][8];
			double bishup_early[8][8];
			double bishup_late[8][8];
			double queen_early[8][8];
			double queen_late[8][8];
			double king_early[8][8];
			double king_late[8][8];
		} m_pst_white, m_pst_black;
		
	};
	
} /// namespace ai01
