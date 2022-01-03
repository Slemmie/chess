#include <get_moves.h>

#include <piece.h>

uint8_t game_state(const Board& board) {
	// get available moves and call game_state() with those moves
	std::vector <Move> result;
	for (uint8_t rank = 0; rank < 8; rank++) {
		for (uint8_t file = 0; file < 8; file++) {
			if (board[rank][file].same_color(board.turn())) {
				std::vector <Move> here = get_moves(board, Square_index(rank, file));
				result.insert(result.end(), here.begin(), here.end());
			}
		}
	}
	return game_state(board, result);
}

uint8_t game_state(const Board& board, const std::vector <Move>& avail_moves) {
	// insufficient material check
	bool wwb = false, wbb = false;
	bool bwb = false, bbb = false;
	bool brq = false;
	uint8_t wn = 0, bn = 0;
	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			if (board[file][rank].same_piece(PIECE_PAWN) ||
			board[file][rank].same_piece(PIECE_ROOK) ||
			board[file][rank].same_piece(PIECE_QUEE)) {
				brq = true;
			}
			if (board[file][rank].same_piece(PIECE_KNIG)) {
				wn += board[file][rank].same_color(COLOR_WHITE);
				bn += board[file][rank].same_color(COLOR_BLACK);
			}
			if (board[file][rank].same_piece(PIECE_BISH)) {
				if (board[file][rank].same_color(COLOR_WHITE)) {
					wwb |= !((rank + file) & 1);
					wbb |= (rank + file) & 1;
				} else if (board[file][rank].same_color(COLOR_BLACK)) {
					bwb |= !((rank + file) & 1);
					bbb |= (rank + file) & 1;
				}
			}
		}
	}
	bool w_ins = !((wwb & wbb) || (wn > 1) || ((wwb | wbb) & !!wn) || brq);
	bool b_ins = !((bwb & bbb) || (bn > 1) || ((bwb | bbb) & !!bn) || brq);
	if (w_ins & b_ins) {
		return GAME_STATE_DRAW;
	}
	// get moves for current colors turn - if no moves, check if in check and return lose or draw
	if (board.moves_of_color(board.turn()).empty()) {
		if (is_check(board, board.turn())) {
			return board.turn() == COLOR_WHITE ? GAME_STATE_BLACK_WINS : GAME_STATE_WHITE_WINS;
		}
		return GAME_STATE_DRAW;
	}
	// 100 halfmove draw check
	if (board.halfmoves() >= 100) {
		return GAME_STATE_DRAW;
	}
	// return alive state
	return GAME_STATE_ALIVE;
}

bool is_check(const Board& board, uint8_t king_color) {
	signed int rank = 0, file = 0;
	for (; rank < 8; rank++, file = 0) {
		bool flag = false;
		for (; file < 8; file++) {
			if (board[rank][file].same_piece(PIECE_KING) &&
			board[rank][file].same_color(king_color)) {
				flag = true;
				break;
			}
		}
		if (flag) {
			break;
		}
	}
	if (king_color == COLOR_BLACK && rank != 7) {
		if (file != 0 && board[rank + 1][file - 1].same_piece(PIECE_PAWN) &&
		board[rank + 1][file - 1].same_color(COLOR_WHITE)) {
			return true;
		}
		if (file != 7 && board[rank + 1][file + 1].same_piece(PIECE_PAWN) &&
		board[rank + 1][file + 1].same_color(COLOR_WHITE)) {
			return true;
		}
	}
	if (king_color == COLOR_WHITE && rank != 0) {
		if (file != 0 && board[rank - 1][file - 1].same_piece(PIECE_PAWN) &&
		board[rank - 1][file - 1].same_color(COLOR_BLACK)) {
			return true;
		}
		if (file != 7 && board[rank - 1][file + 1].same_piece(PIECE_PAWN) &&
		board[rank - 1][file + 1].same_color(COLOR_BLACK)) {
			return true;
		}
	}
	for (signed int r = rank - 1; r >= 0; r--) {
		if (!board[r][file].same_piece(PIECE_NONE)) {
			if (!board[r][file].same_color(king_color) && (
			board[r][file].same_piece(PIECE_QUEE) ||
			board[r][file].same_piece(PIECE_ROOK))) {
				return true;
			}
			break;
		}
	}
	for (signed int r = rank + 1; r < 8; r++) {
		if (!board[r][file].same_piece(PIECE_NONE)) {
			if (!board[r][file].same_color(king_color) && (
			board[r][file].same_piece(PIECE_QUEE) ||
			board[r][file].same_piece(PIECE_ROOK))) {
				return true;
			}
			break;
		}
	}
	for (signed int f = file - 1; f >= 0; f--) {
		if (!board[rank][f].same_piece(PIECE_NONE)) {
			if (!board[rank][f].same_color(king_color) && (
			board[rank][f].same_piece(PIECE_QUEE) ||
			board[rank][f].same_piece(PIECE_ROOK))) {
				return true;
			}
			break;
		}
	}
	for (signed int f = file + 1; f < 8; f++) {
		if (!board[rank][f].same_piece(PIECE_NONE)) {
			if (!board[rank][f].same_color(king_color) && (
			board[rank][f].same_piece(PIECE_QUEE) ||
			board[rank][f].same_piece(PIECE_ROOK))) {
				return true;
			}
			break;
		}
	}
	for (signed int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(king_color) && (
			board[r][f].same_piece(PIECE_QUEE) ||
			board[r][f].same_piece(PIECE_BISH))) {
				return true;
			}
			break;
		}
	}
	for (signed int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(king_color) && (
			board[r][f].same_piece(PIECE_QUEE) ||
			board[r][f].same_piece(PIECE_BISH))) {
				return true;
			}
			break;
		}
	}
	for (signed int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(king_color) && (
			board[r][f].same_piece(PIECE_QUEE) ||
			board[r][f].same_piece(PIECE_BISH))) {
				return true;
			}
			break;
		}
	}
	for (signed int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(king_color) && (
			board[r][f].same_piece(PIECE_QUEE) ||
			board[r][f].same_piece(PIECE_BISH))) {
				return true;
			}
			break;
		}
	}
	static signed int knight_dr[8] = { -2, -2, -1, 1, 2, 2, 1, -1 };
	static signed int knight_df[8] = { -1, 1, 2, 2, 1, -1, -2, -2 };
	for (uint8_t i = 0; i < 8; i++) {
		signed int r = rank + knight_dr[i];
		signed int f = file + knight_df[i];
		if (r >= 0 && r < 8 && f >= 0 && f < 8 && board[r][f].same_piece(PIECE_KNIG) &&
		!board[r][f].same_color(king_color)) {
			return true;
		}
	}
	return false;
}

std::vector <Move> get_moves(const Board& board, const Square_index& square) {
	uint8_t piece = board[square].piece() & (~0b11);
	switch (piece) {
		case PIECE_PAWN: return get_moves_pawn(board, square); break;
		case PIECE_ROOK: return get_moves_rook(board, square); break;
		case PIECE_KNIG: return get_moves_knight(board, square); break;
		case PIECE_BISH: return get_moves_bishup(board, square); break;
		case PIECE_QUEE: return get_moves_queen(board, square); break;
		case PIECE_KING: return get_moves_king(board, square); break;
		default: break;
	};
	return { };
}

std::vector <Move> validate_moves(const Board& board, const std::vector <Move>& moves) {
	std::vector <Move> result;
	for (const Move& move : moves) {
		if (!is_check(Board(board, move),
		board[move.from].same_color(COLOR_WHITE) ? COLOR_WHITE : COLOR_BLACK)) {
			result.emplace_back(move);
		}
	}
	return result;
}

std::vector <Move> get_moves_pawn(const Board& board, const Square_index& square) {
	std::vector <Move> result;
	if (board[square].same_color(COLOR_WHITE)) {
		if (square.rank() == 6) {
			if (board[square.rank() - 1][square.file()].same_piece(PIECE_NONE) &&
			board[square.rank() - 2][square.file()].same_piece(PIECE_NONE)) {
				result.emplace_back(Move(square, Square_index(square.rank() - 2, square.file())));
			}
		}
		if (board[square.rank() - 1][square.file()].same_piece(PIECE_NONE)) {
			result.emplace_back(Move(square, Square_index(square.rank() - 1, square.file())));
		}
		if (square.file() != 0 &&
		(board[square.rank() - 1][square.file() - 1].same_color(COLOR_BLACK) ||
		board.en_pessant_square() == Square_index(square.rank() - 1, square.file() - 1))) {
			result.emplace_back(Move(square, Square_index(square.rank() - 1, square.file() - 1)));
		}
		if (square.file() != 7 &&
		(board[square.rank() - 1][square.file() + 1].same_color(COLOR_BLACK) ||
		board.en_pessant_square() == Square_index(square.rank() - 1, square.file() + 1))) {
			result.emplace_back(Move(square, Square_index(square.rank() - 1, square.file() + 1)));
		}
	} else {
		if (square.rank() == 1) {
			if (board[square.rank() + 1][square.file()].same_piece(PIECE_NONE) &&
			board[square.rank() + 2][square.file()].same_piece(PIECE_NONE)) {
				result.emplace_back(Move(square, Square_index(square.rank() + 2, square.file())));
			}
		}
		if (board[square.rank() + 1][square.file()].same_piece(PIECE_NONE)) {
			result.emplace_back(Move(square, Square_index(square.rank() + 1, square.file())));
		}
		if (square.file() != 0 &&
		(board[square.rank() + 1][square.file() - 1].same_color(COLOR_WHITE) ||
		board.en_pessant_square() == Square_index(square.rank() + 1, square.file() - 1))) {
			result.emplace_back(Move(square, Square_index(square.rank() + 1, square.file() - 1)));
		}
		if (square.file() != 7 &&
		(board[square.rank() + 1][square.file() + 1].same_color(COLOR_WHITE) ||
		board.en_pessant_square() == Square_index(square.rank() + 1, square.file() + 1))) {
			result.emplace_back(Move(square, Square_index(square.rank() + 1, square.file() + 1)));
		}
	}
	std::vector <Move> other_en_pessant_results;
	uint8_t color = board[square].same_color(COLOR_WHITE) ? COLOR_WHITE : COLOR_BLACK;
	for (Move& move : result) {
		if (move.to.rank() == 0 || move.to.rank() == 7) {
			other_en_pessant_results.emplace_back(Move(move.from, move.to, Piece(PIECE_ROOK, color)));
			other_en_pessant_results.emplace_back(Move(move.from, move.to, Piece(PIECE_KNIG, color)));
			other_en_pessant_results.emplace_back(Move(move.from, move.to, Piece(PIECE_BISH, color)));
			move.promote_result = Piece(PIECE_QUEE, color);
		}
	}
	result.insert(result.end(), other_en_pessant_results.begin(), other_en_pessant_results.end());
	return validate_moves(board, result);
}

std::vector <Move> get_moves_rook(const Board& board, const Square_index& square) {
	std::vector <Move> result;
	uint8_t color = board[square].same_color(COLOR_WHITE) ? COLOR_WHITE : COLOR_BLACK;
	for (signed int r = (signed int)square.rank() - 1; r >= 0; r--) {
		if (!board[r][square.file()].same_piece(PIECE_NONE)) {
			if (!board[r][square.file()].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, square.file())));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, square.file())));
	}
	for (signed int r = square.rank() + 1; r < 8; r++) {
		if (!board[r][square.file()].same_piece(PIECE_NONE)) {
			if (!board[r][square.file()].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, square.file())));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, square.file())));
	}
	for (signed int f = (signed int)square.file() - 1; f >= 0; f--) {
		if (!board[square.rank()][f].same_piece(PIECE_NONE)) {
			if (!board[square.rank()][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(square.rank(), f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(square.rank(), f)));
	}
	for (signed int f = square.file() + 1; f < 8; f++) {
		if (!board[square.rank()][f].same_piece(PIECE_NONE)) {
			if (!board[square.rank()][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(square.rank(), f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(square.rank(), f)));
	}
	return validate_moves(board, result);
}

std::vector <Move> get_moves_knight(const Board& board, const Square_index& square) {
	std::vector <Move> result;
	uint8_t color = board[square].same_color(COLOR_WHITE) ? COLOR_WHITE : COLOR_BLACK;
	static signed int knight_dr[8] = { -2, -2, -1, 1, 2, 2, 1, -1 };
	static signed int knight_df[8] = { -1, 1, 2, 2, 1, -1, -2, -2 };
	for (uint8_t i = 0; i < 8; i++) {
		signed int r = (signed int)square.rank() + knight_dr[i];
		signed int f = (signed int)square.file() + knight_df[i];
		if (r >= 0 && r < 8 && f >= 0 && f < 8 && !board[r][f].same_color(color)) {
			result.emplace_back(Move(square, Square_index(r, f)));
		}
	}
	return validate_moves(board, result);
}

std::vector <Move> get_moves_bishup(const Board& board, const Square_index& square) {
	std::vector <Move> result;
	uint8_t color = board[square].same_color(COLOR_WHITE) ? COLOR_WHITE : COLOR_BLACK;
	for (signed int r = (signed int)square.rank() - 1, f = square.file() - 1; r >= 0 && f >= 0; r--, f--) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, f)));
	}
	for (signed int r = (signed int)square.rank() - 1, f = square.file() + 1; r >= 0 && f < 8; r--, f++) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, f)));
	}
	for (signed int r = square.rank() + 1, f = (signed int)square.file() - 1; r < 8 && f >= 0; r++, f--) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, f)));
	}
	for (signed int r = square.rank() + 1, f = square.file() + 1; r < 8 && f < 8; r++, f++) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, f)));
	}
	return validate_moves(board, result);
}

std::vector <Move> get_moves_queen(const Board& board, const Square_index& square) {
	std::vector <Move> result;
	uint8_t color = board[square].same_color(COLOR_WHITE) ? COLOR_WHITE : COLOR_BLACK;
	for (signed int r = (signed int)square.rank() - 1; r >= 0; r--) {
		if (!board[r][square.file()].same_piece(PIECE_NONE)) {
			if (!board[r][square.file()].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, square.file())));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, square.file())));
	}
	for (signed int r = square.rank() + 1; r < 8; r++) {
		if (!board[r][square.file()].same_piece(PIECE_NONE)) {
			if (!board[r][square.file()].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, square.file())));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, square.file())));
	}
	for (signed int f = (signed int)square.file() - 1; f >= 0; f--) {
		if (!board[square.rank()][f].same_piece(PIECE_NONE)) {
			if (!board[square.rank()][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(square.rank(), f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(square.rank(), f)));
	}
	for (signed int f = square.file() + 1; f < 8; f++) {
		if (!board[square.rank()][f].same_piece(PIECE_NONE)) {
			if (!board[square.rank()][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(square.rank(), f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(square.rank(), f)));
	}
	for (signed int r = (signed int)square.rank() - 1, f = (signed int)square.file() - 1; r >= 0 && f >= 0; r--, f--) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, f)));
	}
	for (signed int r = (signed int)square.rank() - 1, f = square.file() + 1; r >= 0 && f < 8; r--, f++) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, f)));
	}
	for (signed int r = square.rank() + 1, f = (signed int)square.file() - 1; r < 8 && f >= 0; r++, f--) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, f)));
	}
	for (signed int r = square.rank() + 1, f = square.file() + 1; r < 8 && f < 8; r++, f++) {
		if (!board[r][f].same_piece(PIECE_NONE)) {
			if (!board[r][f].same_color(color)) {
				result.emplace_back(Move(square, Square_index(r, f)));
			}
			break;
		}
		result.emplace_back(Move(square, Square_index(r, f)));
	}
	return validate_moves(board, result);
}

std::vector <Move> get_moves_king(const Board& board, const Square_index& square) {
	std::vector <Move> result;
	uint8_t color = board[square].same_color(COLOR_WHITE) ? COLOR_WHITE : COLOR_BLACK;
	static signed int king_dr[8] = { -1,-1,-1, 0, 1, 1, 1, 0 };
	static signed int king_df[8] = { -1, 0, 1, 1, 1, 0,-1,-1 };
	auto kings_adj = [&] (signed int rank, signed int file) -> bool {
		for (uint8_t i = 0; i < 8; i++) {
			signed int r = rank + king_dr[i];
			signed int f = file + king_df[i];
			if (r >= 0 && r < 8 && f >= 0 && f < 8 && board[r][f].same_piece(PIECE_KING) &&
			!board[r][f].same_color(color)) {
				return true;
			}
		}
		return false;
	};
	for (uint8_t i = 0; i < 8; i++) {
		signed int r = (signed int)square.rank() + king_dr[i];
		signed int f = (signed int)square.file() + king_df[i];
		if (r >= 0 && r < 8 && f >= 0 && f < 8 && !board[r][f].same_color(color) &&
		!kings_adj(r, f)) {
			result.emplace_back(Move(square, Square_index(r, f)));
		}
	}
	bool check_here = is_check(board, color);
	if (color == COLOR_WHITE) {
		if (board.castle_WK() &&
		board[square.rank()][square.file() + 1].same_piece(PIECE_NONE) &&
		board[square.rank()][square.file() + 2].same_piece(PIECE_NONE) &&
		!check_here &&
		!is_check(Board(board, Move(square.rank(), square.file() + 1)), color)) {
			result.emplace_back(Move(square, Square_index(square.rank(), square.file() + 2)));
		}
		if (board.castle_WQ() &&
		board[square.rank()][square.file() - 1].same_piece(PIECE_NONE) &&
		board[square.rank()][square.file() - 2].same_piece(PIECE_NONE) &&
		board[square.rank()][square.file() - 3].same_piece(PIECE_NONE) &&
		!check_here &&
		!is_check(Board(board, Move(square.rank(), square.file() - 1)), color)) {
			result.emplace_back(Move(square, Square_index(square.rank(), square.file() - 2)));
		}
	} else {
		if (board.castle_BK() &&
		board[square.rank()][square.file() + 1].same_piece(PIECE_NONE) &&
		board[square.rank()][square.file() + 2].same_piece(PIECE_NONE) &&
		!check_here &&
		!is_check(Board(board, Move(square.rank(), square.file() + 1)), color)) {
			result.emplace_back(Move(square, Square_index(square.rank(), square.file() + 2)));
		}
		if (board.castle_BQ() &&
		board[square.rank()][square.file() - 1].same_piece(PIECE_NONE) &&
		board[square.rank()][square.file() - 2].same_piece(PIECE_NONE) &&
		board[square.rank()][square.file() - 3].same_piece(PIECE_NONE) &&
		!check_here &&
		!is_check(Board(board, Move(square.rank(), square.file() - 1)), color)) {
			result.emplace_back(Move(square, Square_index(square.rank(), square.file() - 2)));
		}
	}
	return validate_moves(board, result);
}
