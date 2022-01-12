#pragma once

#include <game.h>

#include <util/random.h>

#include <string>
#include <fstream>
#include <iostream>
#include <cassert>

namespace ai01 {
	
	class Learn_eval {
		
	public:
		
		Learn_eval() :
		m_piece_vals(),
		m_pst_white(),
		m_pst_black()
		{ }
		
		Learn_eval(const std::string& _filepath) {
			std::ifstream inf(_filepath, std::ios::binary);
			if (!inf.is_open()) {
				std::cerr << "failed to open '" << _filepath << "'" << std::endl;
				return;
			}
			m_piece_vals = Piece_values(inf);
			m_pst_white = Piece_square_table(inf);
			m_pst_black = Piece_square_table(inf);
			inf.close();
		}
		
		Learn_eval(std::ifstream& _stream) :
		m_piece_vals(_stream),
		m_pst_white(_stream),
		m_pst_black(_stream)
		{ }
		
		Learn_eval(const Learn_eval& p1, const Learn_eval& p2,
		const double mutation_rate, const std::string& tmp_file_dir) {
			std::ofstream p1_ouf(tmp_file_dir + "/p1", std::ios::binary);
			std::ofstream p2_ouf(tmp_file_dir + "/p2", std::ios::binary);
			p1.write_binfile(p1_ouf);
			p2.write_binfile(p2_ouf);
			p1_ouf.close();
			p2_ouf.close();
			std::ifstream p1_inf(tmp_file_dir + "/p1", std::ios::binary);
			std::ifstream p2_inf(tmp_file_dir + "/p2", std::ios::binary);
			std::ofstream re_ouf(tmp_file_dir + "/re", std::ios::binary);
			auto file_size = [] (std::ifstream& file) -> size_t {
				std::streampos size = file.tellg();
				file.seekg(0, std::ios::end);
				size = file.tellg() - size;
				file.seekg(0, std::ios::beg);
				return (size_t)size;
			};
			size_t size1 = file_size(p1_inf);
			size_t size2 = file_size(p2_inf);
			assert(size1 == size2);
			std::vector <int16_t> vp1(size1 / 2), vp2(size2 / 2);
			p1_inf.read(reinterpret_cast <char*> (&vp1[0]), sizeof(vp1.size()));
			p2_inf.read(reinterpret_cast <char*> (&vp2[0]), sizeof(vp2.size()));
			double cross_rate = mutation_rate / 64.0;
			int which = 0;
			for (size_t i = 0; i < vp1.size(); i++) {
				int16_t c1 = vp1[i];
				int16_t c2 = vp2[i];
				int16_t ce = 0;
				for (int b = 0; b < 15; b++) {
					if (which == 0) {
						ce |= (1 << b) & c1;
					} else {
						ce |= (1 << b) & c2;
					}
					if (Random::normal() <= mutation_rate / (2.0 * double(b + 1))) {
						ce ^= 1 << b;
					}
					if (Random::normal() <= cross_rate) {
						which ^= 1;
					}
				}
				if (abs(ce) <= 2000 &&
				Random::normal() <= mutation_rate / std::max(0.8, (double)abs(ce) / 1000.0)) {
					ce *= -1;
				}
				re_ouf.write(reinterpret_cast <const char*>
				(&ce), sizeof(int16_t));
			}
			p1_inf.close();
			p2_inf.close();
			re_ouf.close();
			std::ifstream inf(tmp_file_dir + "/re", std::ios::binary);
			m_piece_vals = Piece_values(inf);
			m_pst_white = Piece_square_table(inf);
			m_pst_black = Piece_square_table(inf);
			inf.close();
		}
		
		inline void write_binfile(std::ofstream& ouf) const {
			m_piece_vals.write_binfile(ouf);
			m_pst_white.write_binfile(ouf);
			m_pst_black.write_binfile(ouf);
		}
		
		double eval(const Board& board) const {
			double result = 0;
			int32_t white_material_nopawns = 0;
			int32_t black_material_nopawns = 0;
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
							result += white_endgame_weight *
							m_transform(m_pst_white.pawn_late[rank][file]) +
							(1.0 - white_endgame_weight) *
							m_transform(m_pst_white.pawn_early[rank][file]);
						} else {
							result -= black_endgame_weight *
							m_transform(m_pst_black.pawn_late[rank][file]) +
							(1.0 - black_endgame_weight) *
							m_transform(m_pst_black.pawn_early[rank][file]);
						}
					} else if (board[rank][file].same_piece(PIECE_ROOK)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight *
							m_transform(m_pst_white.rook_late[rank][file]) +
							(1.0 - white_endgame_weight) *
							m_transform(m_pst_white.rook_early[rank][file]);
						} else {
							result -= black_endgame_weight *
							m_transform(m_pst_black.rook_late[rank][file]) +
							(1.0 - black_endgame_weight) *
							m_transform(m_pst_black.rook_early[rank][file]);
						}
					} else if (board[rank][file].same_piece(PIECE_KNIG)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight *
							m_transform(m_pst_white.knight_late[rank][file]) +
							(1.0 - white_endgame_weight) *
							m_transform(m_pst_white.knight_early[rank][file]);
						} else {
							result -= black_endgame_weight *
							m_transform(m_pst_black.knight_late[rank][file]) +
							(1.0 - black_endgame_weight) *
							m_transform(m_pst_black.knight_early[rank][file]);
						}
					} else if (board[rank][file].same_piece(PIECE_BISH)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight *
							m_transform(m_pst_white.bishup_late[rank][file]) +
							(1.0 - white_endgame_weight) *
							m_transform(m_pst_white.bishup_early[rank][file]);
						} else {
							result -= black_endgame_weight *
							m_transform(m_pst_black.bishup_late[rank][file]) +
							(1.0 - black_endgame_weight) *
							m_transform(m_pst_black.bishup_early[rank][file]);
						}
					} else if (board[rank][file].same_piece(PIECE_QUEE)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight *
							m_transform(m_pst_white.queen_late[rank][file]) +
							(1.0 - white_endgame_weight) *
							m_transform(m_pst_white.queen_early[rank][file]);
						} else {
							result -= black_endgame_weight *
							m_transform(m_pst_black.queen_late[rank][file]) +
							(1.0 - black_endgame_weight) *
							m_transform(m_pst_black.queen_early[rank][file]);
						}
					} else if (board[rank][file].same_piece(PIECE_KING)) {
						if (board[rank][file].same_color(COLOR_WHITE)) {
							result += white_endgame_weight *
							m_transform(m_pst_white.king_late[rank][file]) +
							(1.0 - white_endgame_weight) *
							m_transform(m_pst_white.king_early[rank][file]);
						} else {
							result -= black_endgame_weight *
							m_transform(m_pst_black.king_late[rank][file]) +
							(1.0 - black_endgame_weight) *
							m_transform(m_pst_black.king_early[rank][file]);
						}
					}
				}
			}
			return result;
		}
		
		inline double piece_value(const uint8_t piece) const {
			return m_transform(m_piece_vals[piece]);
		}
		
	private:
		
		inline double m_transform(const int32_t value) const {
			return (double)value / 2000.0;
		}
		
		inline double m_endgame_weight(const int32_t material_nopawns) const {
			static const double end_mult = 1.0 / double(2.0 *
			m_piece_vals.piece_values[PIECE_ROOK | COLOR_WHITE] +
			m_piece_vals.piece_values[PIECE_BISH | COLOR_WHITE] +
			m_piece_vals.piece_values[PIECE_KNIG | COLOR_WHITE]);
			return 1.0 - std::min(1.0, (double)material_nopawns * end_mult);
		}
		
		struct Piece_values {
			Piece_values() {
				for (uint8_t i = 0; i < 32; i++) {
					piece_values[i] = Random::rs32(
					std::numeric_limits <int16_t>::min(),
					std::numeric_limits <int16_t>::max());
				}
			}
			
			Piece_values(std::ifstream& inf) {
				inf.read(reinterpret_cast <char*> (&piece_values[0]), sizeof(int16_t) * 32);
			}
			
			void write_binfile(std::ofstream& ouf) const {
				for (uint8_t i = 0; i < 8; i++) {
					ouf.write(reinterpret_cast <const char*>
					(&piece_values[i]), sizeof(piece_values[i]));
				}
			}
			
			inline int16_t operator [] (const size_t index) const {
				return piece_values[index];
			}
			
			int16_t piece_values[32];
		} m_piece_vals;
		
		struct Piece_square_table {
			Piece_square_table() {
				for (uint8_t rank = 0; rank < 8; rank++) {
					for (uint8_t file = 0; file < 8; file++) {
						pawn_early[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						pawn_late[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						rook_early[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						rook_late[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						knight_early[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						knight_late[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						bishup_early[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						bishup_late[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						queen_early[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						queen_late[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						king_early[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
						king_late[rank][file] = Random::rs32(
						std::numeric_limits <int16_t>::min(),
						std::numeric_limits <int16_t>::max()) / 4;
					}
				}
			}
			
			Piece_square_table(std::ifstream& inf) {
				auto read_grid = [&] (int16_t grid[8][8]) -> void {
					for (uint8_t rank = 0; rank < 8; rank++) {
						inf.read(reinterpret_cast <char*> (&grid[rank][0]), sizeof(int16_t) * 8);
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
				auto write_grid = [&] (const int16_t grid[8][8]) -> void {
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
			
			int16_t pawn_early[8][8];
			int16_t pawn_late[8][8];
			int16_t rook_early[8][8];
			int16_t rook_late[8][8];
			int16_t knight_early[8][8];
			int16_t knight_late[8][8];
			int16_t bishup_early[8][8];
			int16_t bishup_late[8][8];
			int16_t queen_early[8][8];
			int16_t queen_late[8][8];
			int16_t king_early[8][8];
			int16_t king_late[8][8];
		} m_pst_white, m_pst_black;
		
	};
	
} /// namespace ai01
