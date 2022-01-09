#pragma once

#include <game.h>

namespace ai01 {
	
	class Default_eval {
		
	public:
		
		double eval(const Board& board) const {
			double result = 0;
			
			double white_material_nopawns = 0;
			double black_material_nopawns = 0;
			double white_material_nopawnsqueens = 0;
			double black_material_nopawnsqueens = 0;
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
							white_material_nopawns += m_piece_values[board[rank][file].piece()];
							if (!board[rank][file].same_piece(PIECE_QUEE)) {
								white_material_nopawnsqueens += m_piece_values[board[rank][file].piece()];
							}
						} else {
							black_material_nopawns -= m_piece_values[board[rank][file].piece()];
							if (!board[rank][file].same_piece(PIECE_QUEE)) {
								black_material_nopawnsqueens -= m_piece_values[board[rank][file].piece()];
							}
						}
					}
				}
			}
			double white_endgame_weight = m_endgame_weight(white_material_nopawns);
			double black_endgame_weight = m_endgame_weight(black_material_nopawns);
			
			bool white_check = is_check(board, COLOR_WHITE);
			bool black_check = is_check(board, COLOR_BLACK);
			double white_endgame_weight_noqueens = m_endgame_weight(white_material_nopawnsqueens);
			double black_endgame_weight_noqueens = m_endgame_weight(black_material_nopawnsqueens);
			result -= 2.5 * (white_endgame_weight_noqueens * white_endgame_weight_noqueens) * white_check;
			result += 2.5 * (black_endgame_weight_noqueens * black_endgame_weight_noqueens) * black_check;
			
			for (uint8_t rank = 0; rank < 8; rank++) {
				for (uint8_t file = 0; file < 8; file++) {
					if (board[rank][file].same_piece(PIECE_KING)) {
						static signed int dr_close[8] = {-1,-1,-1, 0, 1, 1, 1, 0 };
						static signed int df_close[8] = {-1, 0, 1, 1, 1, 0,-1,-1 };
						static signed int dr_far[16] =
						{-2,-2,-2,-2,-2,-1, 0, 1, 2, 2, 2, 2, 2, 1, 0,-1 };
						static signed int df_far[16] =
						{-2,-1, 0, 1, 2, 2, 2, 2, 2, 1, 0,-1,-2,-2,-2,-2 };
						double count = 0;
						for (int k = 0; k < 8; k++) {
							signed int r = (signed int)rank + dr_close[k];
							signed int f = (signed int)file + df_close[k];
							if (r < 0 || f < 0 || r >= 8 || f >= 8) {
								continue;
							}
							if (board[r][f].same_piece(PIECE_NONE)) {
								count -= 0.2;
								continue;
							}
							count += 0.3;
						}
						for (int k = 0; k < 16; k++) {
							signed int r = (signed int)rank + dr_far[k];
							signed int f = (signed int)file + df_far[k];
							if (r < 0 || f < 0 || r >= 8 || f >= 8) {
								continue;
							}
							if (board[r][f].same_piece(PIECE_NONE)) {
								count -= 0.08;
								continue;
							}
							count += 0.15;
						}
						count *= board[rank][file].same_color(COLOR_WHITE) ? 1 : -1;
						count *= 1.5 * (board[rank][file].same_color(COLOR_WHITE) ?
						white_endgame_weight : black_endgame_weight);
					}
					if (!board[rank][file].same_piece(PIECE_NONE)) {
						if (board[rank][file].same_piece(PIECE_KING)) {
							if (board[rank][file].same_color(COLOR_WHITE)) {
								result += white_endgame_weight *
								Piece_square_table_white::king_late[rank][file] +
								(1.0 - white_endgame_weight) * 
								Piece_square_table_white::king_early[rank][file];
							} else {
								result -= black_endgame_weight *
								Piece_square_table_black::king_late[rank][file] +
								(1.0 - black_endgame_weight) * 
								Piece_square_table_black::king_early[rank][file];
							}
						} else {
							result += m_piece_values[board[rank][file].piece()];
							if (board[rank][file].same_piece(PIECE_PAWN)) {
								if (board[rank][file].same_color(COLOR_WHITE)) {
									result += Piece_square_table_white::pawn[rank][file];
								} else {
									result -= Piece_square_table_black::pawn[rank][file];
								}
							} else if (board[rank][file].same_piece(PIECE_ROOK)) {
								if (board[rank][file].same_color(COLOR_WHITE)) {
									result += Piece_square_table_white::rook[rank][file];
								} else {
									result -= Piece_square_table_black::rook[rank][file];
								}
							} else if (board[rank][file].same_piece(PIECE_KNIG)) {
								if (board[rank][file].same_color(COLOR_WHITE)) {
									result += Piece_square_table_white::knight[rank][file];
								} else {
									result -= Piece_square_table_black::knight[rank][file];
								}
							} else if (board[rank][file].same_piece(PIECE_BISH)) {
								if (board[rank][file].same_color(COLOR_WHITE)) {
									result += Piece_square_table_white::bishup[rank][file];
								} else {
									result -= Piece_square_table_black::bishup[rank][file];
								}
							} else if (board[rank][file].same_piece(PIECE_QUEE)) {
								if (board[rank][file].same_color(COLOR_WHITE)) {
									result += Piece_square_table_white::queen[rank][file];
								} else {
									result -= Piece_square_table_black::queen[rank][file];
								}
							}
						}
					}
				}
			}
			
			result += m_can_castle_value * board.castle_WK();
			result += m_can_castle_value * board.castle_WQ();
			result -= m_can_castle_value * board.castle_BK();
			result -= m_can_castle_value * board.castle_BQ();
			
			return result;
		}
		
		inline double piece_value(const uint8_t piece) const {
			return m_piece_values[piece];
		}
		
		inline double piece_position_value(const Board& board,
		uint8_t from_rank, uint8_t from_file,
		uint8_t to_rank, uint8_t to_file) const {
			double result = 0;
			if (!board[from_rank][from_file].same_piece(PIECE_KING)) {
				if (board[from_rank][from_file].same_piece(PIECE_PAWN)) {
					if (board[from_rank][from_file].same_color(COLOR_WHITE)) {
						result += Piece_square_table_white::pawn[to_rank][to_file];
					} else {
						result += Piece_square_table_black::pawn[to_rank][to_file];
					}
				} else if (board[from_rank][from_file].same_piece(PIECE_ROOK)) {
					if (board[from_rank][from_file].same_color(COLOR_WHITE)) {
						result += Piece_square_table_white::rook[to_rank][to_file];
					} else {
						result += Piece_square_table_black::rook[to_rank][to_file];
					}
				} else if (board[from_rank][from_file].same_piece(PIECE_KNIG)) {
					if (board[from_rank][from_file].same_color(COLOR_WHITE)) {
						result += Piece_square_table_white::knight[to_rank][to_file];
					} else {
						result += Piece_square_table_black::knight[to_rank][to_file];
					}
				} else if (board[from_rank][from_file].same_piece(PIECE_BISH)) {
					if (board[from_rank][from_file].same_color(COLOR_WHITE)) {
						result += Piece_square_table_white::bishup[to_rank][to_file];
					} else {
						result += Piece_square_table_black::bishup[to_rank][to_file];
					}
				} else if (board[from_rank][from_file].same_piece(PIECE_QUEE)) {
					if (board[from_rank][from_file].same_color(COLOR_WHITE)) {
						result += Piece_square_table_white::queen[to_rank][to_file];
					} else {
						result += Piece_square_table_black::queen[to_rank][to_file];
					}
				}
			}
			return result;
		}
		
	private:
		
		// the bigger the value - the later in the game we are
		inline double m_endgame_weight(const double material_nopawns) const {
			static const double end_mult = 1.0 / (2.0 *
			m_piece_values[PIECE_ROOK | COLOR_WHITE] +
			m_piece_values[PIECE_BISH | COLOR_WHITE] +
			m_piece_values[PIECE_KNIG | COLOR_WHITE]);
			return 1.0 - std::min(1.0, material_nopawns * end_mult);
		}
		
	private:
		
		static inline double m_can_castle_value = 0.03;
		
		static inline double m_piece_values[] = {
			0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			+1.0, // 01001 , w pawn
			-1.0, // 01010 , b pawn
			0.0, // 01011 , ---
			0.0, // 01100 , ---
			+5.0, // 01101 , w rook
			-5.0, // 01110 , b rook
			0.0, // 01111 , ---
			0.0, // 10000 , ---
			+3.0, // 10001 , w knight
			-3.0, // 10010 , b knight
			0.0, // 10011 , ---
			0.0, // 10100 , ---
			+3.0, // 10101 , w bishup
			-3.0, // 10110 , b bishup
			0.0, // 10111 , ---
			0.0, // 11000 , ---
			+9.0, // 11001 , w queen
			-9.0, // 11010 , b queen
			0.0, // 11011 , ---
			0.0, // 11100 , ---
			+99.0, // 11101 , w king
			-99.0  // 11110 , b king
		};
		
	private:
		
		struct Piece_square_table_white {
			
			static inline double pawn[8][8] = {
				{ 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 },
				{ 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50 },
				{ 0.10, 0.10, 0.20, 0.30, 0.30, 0.20, 0.10, 0.10 },
				{ 0.05, 0.05, 0.10, 0.25, 0.25, 0.10, 0.05, 0.05 },
				{ 0.00, 0.00, 0.05, 0.20, 0.20, 0.05, 0.00, 0.00 },
				{ 0.05,-0.05,-0.10, 0.00, 0.00,-0.10,-0.05, 0.05 },
				{ 0.05, 0.10, 0.10,-0.10,-0.10, 0.10, 0.10, 0.05 },
				{ 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 }
			};
			
			static inline double rook[8][8] = {
				{ 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 },
				{ 0.05, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.05 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{ 0.00, 0.00, 0.00, 0.05, 0.05, 0.00, 0.00, 0.00 }
			};
			
			static inline double knight[8][8] = {
				{-0.50,-0.40,-0.30,-0.30,-0.30,-0.30,-0.40,-0.50 },
				{-0.40,-0.20, 0.00, 0.00, 0.00, 0.00,-0.20,-0.40 },
				{-0.30, 0.00, 0.10, 0.15, 0.15, 0.10, 0.00,-0.30 },
				{-0.30, 0.05, 0.15, 0.20, 0.20, 0.15, 0.05,-0.30 },
				{-0.30, 0.00, 0.15, 0.20, 0.20, 0.15, 0.00,-0.30 },
				{-0.30, 0.05, 0.10, 0.15, 0.15, 0.10, 0.05,-0.30 },
				{-0.40,-0.20, 0.00, 0.05, 0.05, 0.00,-0.20,-0.40 },
				{-0.50,-0.35,-0.30,-0.30,-0.30,-0.30,-0.35,-0.50 }
			};
			
			static inline double bishup[8][8] = {
				{-0.20,-0.10,-0.10,-0.10,-0.10,-0.10,-0.10,-0.20 },
				{-0.10, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.10 },
				{-0.10, 0.00, 0.05, 0.10, 0.10, 0.05, 0.00,-0.10 },
				{-0.10, 0.05, 0.05, 0.10, 0.10, 0.05, 0.05,-0.10 },
				{-0.10, 0.00, 0.10, 0.10, 0.10, 0.10, 0.00,-0.10 },
				{-0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10,-0.10 },
				{-0.10, 0.05, 0.00, 0.00, 0.00, 0.00, 0.05,-0.10 },
				{-0.20,-0.10,-0.10,-0.10,-0.10,-0.10,-0.10,-0.20 }
			};
			
			static inline double queen[8][8] = {
				{-0.20,-0.10,-0.10,-0.05,-0.05,-0.10,-0.10,-0.20 },
				{-0.10, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.10 },
				{-0.10, 0.00, 0.05, 0.05, 0.05, 0.05, 0.00,-0.10 },
				{-0.05, 0.00, 0.05, 0.05, 0.05, 0.05, 0.00,-0.05 },
				{ 0.00, 0.00, 0.05, 0.05, 0.05, 0.05, 0.00,-0.05 },
				{-0.10, 0.05, 0.05, 0.05, 0.05, 0.05, 0.00,-0.10 },
				{-0.10, 0.00, 0.05, 0.00, 0.00, 0.00, 0.00,-0.10 },
				{-0.20,-0.10,-0.10,-0.05,-0.05,-0.10,-0.10,-0.20 }
			};
			
			static inline double king_early[8][8] = {
				{-0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30 },
				{-0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30 },
				{-0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30 },
				{-0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30 },
				{-0.20,-0.30,-0.30,-0.40,-0.40,-0.30,-0.30,-0.20 },
				{-0.10,-0.20,-0.20,-0.20,-0.20,-0.20,-0.20,-0.10 },
				{ 0.20, 0.20,-0.30,-0.70,-0.70,-0.70, 0.20, 0.20 },
				{ 0.20, 0.30, 0.10,-0.50, 0.00,-0.10, 0.30, 0.20 }
			};
			
			static inline double king_late[8][8] = {
				{-0.50,-0.40,-0.30,-0.20,-0.20,-0.30,-0.40,-0.50 },
				{-0.30,-0.20,-0.10, 0.00, 0.00,-0.10,-0.20,-0.30 },
				{-0.30,-0.10, 0.20, 0.30, 0.30, 0.20,-0.10,-0.30 },
				{-0.30,-0.10,-0.30,-0.40,-0.40,-0.30,-0.10,-0.30 },
				{-0.30,-0.10,-0.30,-0.40,-0.40,-0.30,-0.10,-0.30 },
				{-0.30,-0.10, 0.20, 0.30, 0.30, 0.20,-0.10,-0.30 },
				{-0.30,-0.30, 0.00, 0.00, 0.00, 0.00,-0.30,-0.30 },
				{-0.50,-0.30,-0.30,-0.30,-0.30,-0.30,-0.30,-0.50 }
			};
			
		};
		
		struct Piece_square_table_black {
			
			static inline double pawn[8][8] = {
				{ 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 },
				{ 0.05, 0.10, 0.10,-0.10,-0.10, 0.10, 0.10, 0.05 },
				{ 0.05,-0.05,-0.10, 0.00, 0.00,-0.10,-0.05, 0.05 },
				{ 0.00, 0.00, 0.05, 0.20, 0.20, 0.05, 0.00, 0.00 },
				{ 0.05, 0.05, 0.10, 0.25, 0.25, 0.10, 0.05, 0.05 },
				{ 0.10, 0.10, 0.20, 0.30, 0.30, 0.20, 0.10, 0.10 },
				{ 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50 },
				{ 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 }
			};
			
			static inline double rook[8][8] = {
				{ 0.00, 0.00, 0.00, 0.05, 0.05, 0.00, 0.00, 0.00 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{-0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05 },
				{ 0.05, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.05 },
				{ 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 }
			};
			
			static inline double knight[8][8] = {
				{-0.50,-0.35,-0.30,-0.30,-0.30,-0.30,-0.35,-0.50 },
				{-0.40,-0.20, 0.00, 0.05, 0.05, 0.00,-0.20,-0.40 },
				{-0.30, 0.05, 0.10, 0.15, 0.15, 0.10, 0.05,-0.30 },
				{-0.30, 0.00, 0.15, 0.20, 0.20, 0.15, 0.00,-0.30 },
				{-0.30, 0.05, 0.15, 0.20, 0.20, 0.15, 0.05,-0.30 },
				{-0.30, 0.00, 0.10, 0.15, 0.15, 0.10, 0.00,-0.30 },
				{-0.40,-0.20, 0.00, 0.00, 0.00, 0.00,-0.20,-0.40 },
				{-0.50,-0.40,-0.30,-0.30,-0.30,-0.30,-0.40,-0.50 }
			};
			
			static inline double bishup[8][8] = {
				{-0.20,-0.10,-0.10,-0.10,-0.10,-0.10,-0.10,-0.20 },
				{-0.10, 0.05, 0.00, 0.00, 0.00, 0.00, 0.05,-0.10 },
				{-0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10,-0.10 },
				{-0.10, 0.00, 0.10, 0.10, 0.10, 0.10, 0.00,-0.10 },
				{-0.10, 0.05, 0.05, 0.10, 0.10, 0.05, 0.05,-0.10 },
				{-0.10, 0.00, 0.05, 0.10, 0.10, 0.05, 0.00,-0.10 },
				{-0.10, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.10 },
				{-0.20,-0.10,-0.10,-0.10,-0.10,-0.10,-0.10,-0.20 }
			};
			
			static inline double queen[8][8] = {
				{-0.20,-0.10,-0.10,-0.05,-0.05,-0.10,-0.10,-0.20 },
				{-0.10, 0.00, 0.00, 0.00, 0.00, 0.05, 0.00,-0.10 },
				{-0.10, 0.00, 0.05, 0.05, 0.05, 0.05, 0.05,-0.10 },
				{-0.05, 0.00, 0.05, 0.05, 0.05, 0.05, 0.00, 0.00 },
				{-0.05, 0.00, 0.05, 0.05, 0.05, 0.05, 0.00,-0.05 },
				{-0.10, 0.00, 0.05, 0.05, 0.05, 0.05, 0.00,-0.10 },
				{-0.10, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.10 },
				{-0.20,-0.10,-0.10,-0.05,-0.05,-0.10,-0.10,-0.20 }
			};
			
			static inline double king_early[8][8] = {
				{ 0.20, 0.30, 0.10,-0.50, 0.00,-0.10, 0.30, 0.20 },
				{ 0.20, 0.20,-0.30,-0.70,-0.70,-0.70, 0.20, 0.20 },
				{-0.10,-0.20,-0.20,-0.20,-0.20,-0.20,-0.20,-0.10 },
				{-0.20,-0.30,-0.30,-0.40,-0.40,-0.30,-0.30,-0.20 },
				{-0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30 },
				{-0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30 },
				{-0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30 },
				{-0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30 }
			};
			
			static inline double king_late[8][8] = {
				{-0.50,-0.30,-0.30,-0.30,-0.30,-0.30,-0.30,-0.50 },
				{-0.30,-0.30, 0.00, 0.00, 0.00, 0.00,-0.30,-0.30 },
				{-0.30,-0.10, 0.20, 0.30, 0.30, 0.20,-0.10,-0.30 },
				{-0.30,-0.10,-0.30,-0.40,-0.40,-0.30,-0.10,-0.30 },
				{-0.30,-0.10,-0.30,-0.40,-0.40,-0.30,-0.10,-0.30 },
				{-0.30,-0.10, 0.20, 0.30, 0.30, 0.20,-0.10,-0.30 },
				{-0.30,-0.20,-0.10, 0.00, 0.00,-0.10,-0.20,-0.30 },
				{-0.50,-0.40,-0.30,-0.20,-0.20,-0.30,-0.40,-0.50 }
			};
			
		};
		
	};
	
} /// namespace ai01
