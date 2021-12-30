#include <board.h>

#include <get_moves.h>

#include <iostream>

Board::Board() :
m_pieces(8, std::vector <Piece> (8)),
m_en_pessant_square(),
m_castle_BK(false), m_castle_BQ(false), m_castle_WK(false), m_castle_WQ(false),
m_halfmoves(0), m_fullmoves(0), m_turn(COLOR_NONE)
{ }

Board::Board(const std::string& _FEN, bool log_fenstring) :
m_pieces(8, std::vector <Piece> (8)),
m_en_pessant_square(),
m_castle_BK(false), m_castle_BQ(false), m_castle_WK(false), m_castle_WQ(false),
m_halfmoves(0), m_fullmoves(0), m_turn(COLOR_NONE)
{
	if (log_fenstring) {
		std::cerr << "loading FEN string '" << _FEN << "'\n\n" << std::endl;
	}
	uint8_t i = 0;
	for (uint8_t rank = 0, file = 0; _FEN[i] != ' '; i++) {
		if (_FEN[i] == '/') {
			rank++;
			file = 0;
			continue;
		}
		if (isdigit(_FEN[i])) {
			file += _FEN[i] - '0';
			continue;
		}
		char ch = tolower(_FEN[i]);
		switch (ch) {
			case 'p': (*this)[rank][file] = Piece(PIECE_PAWN,
			islower(_FEN[i]) ? COLOR_BLACK : COLOR_WHITE); break;
			case 'r': (*this)[rank][file] = Piece(PIECE_ROOK,
			islower(_FEN[i]) ? COLOR_BLACK : COLOR_WHITE); break;
			case 'n': (*this)[rank][file] = Piece(PIECE_KNIG,
			islower(_FEN[i]) ? COLOR_BLACK : COLOR_WHITE); break;
			case 'b': (*this)[rank][file] = Piece(PIECE_BISH,
			islower(_FEN[i]) ? COLOR_BLACK : COLOR_WHITE); break;
			case 'q': (*this)[rank][file] = Piece(PIECE_QUEE,
			islower(_FEN[i]) ? COLOR_BLACK : COLOR_WHITE); break;
			case 'k': (*this)[rank][file] = Piece(PIECE_KING,
			islower(_FEN[i]) ? COLOR_BLACK : COLOR_WHITE); break;
			default: break;
		};
		file++;
	}
	m_turn = _FEN[++i] == 'w' ? COLOR_WHITE : COLOR_BLACK;
	for (i += 2; _FEN[i] != ' '; i++) {
		if (_FEN[i] == '-') {
			continue;
		}
		m_castle_BK |= _FEN[i] == 'k';
		m_castle_BQ |= _FEN[i] == 'q';
		m_castle_WK |= _FEN[i] == 'K';
		m_castle_WQ |= _FEN[i] == 'Q';
	}
	if (_FEN[++i] != '-') {
		m_en_pessant_square = Square_index((std::string)"" + (char)toupper(_FEN[i]) + _FEN[i + 1]);
		i++;
	}
	std::string half, full;
	for (i += 2; _FEN[i] != ' '; i++) {
		half += _FEN[i];
	}
	for (i++; i < (uint8_t)_FEN.size(); i++) {
		full += _FEN[i];
	}
	m_halfmoves = std::stoi(half);
	m_fullmoves = std::stoi(full);
}

Board::Board(const Board& _board, const Move& _move) :
m_pieces(_board.pieces()),
m_en_pessant_square(),
m_castle_BK(_board.castle_BK()), m_castle_BQ(_board.castle_BQ()),
m_castle_WK(_board.castle_WK()), m_castle_WQ(_board.castle_WQ()),
m_halfmoves(_board.halfmoves()), m_fullmoves(_board.fullmoves()),
m_turn(_board.turn() == COLOR_WHITE ? COLOR_BLACK : COLOR_WHITE)
{
	// change move counter
	m_fullmoves += m_turn == COLOR_BLACK;
	if (!_board[_move.from].same_piece(PIECE_PAWN) && _board[_move.to].same_piece(PIECE_NONE)) {
		m_halfmoves++;
	} else {
		m_halfmoves = 0;
	}
	// check for castling
	if (_board[_move.from].same_piece(PIECE_KING) &&
	abs((signed int)_move.from.file() - _move.to.file()) > 1) {
		if (_move.to == Square_index(0, 6)) {
			(*this)[0][5] = _board[0][7];
			(*this)[0][7] = Piece();
		}
		if (_move.to == Square_index(0, 2)) {
			(*this)[0][3] = _board[0][0];
			(*this)[0][0] = Piece();
		}
		if (_move.to == Square_index(7, 6)) {
			(*this)[7][5] = _board[7][7];
			(*this)[7][7] = Piece();
		}
		if (_move.to == Square_index(7, 2)) {
			(*this)[7][3] = _board[7][0];
			(*this)[7][0] = Piece();
		}
	}
	// check for en pessant
	if (_board[_move.from].same_piece(PIECE_PAWN) &&
	_move.from.file() != _move.to.file() &&
	_board[_move.to].same_piece(PIECE_NONE)) {
		(*this)[_move.from.rank()][_move.to.file()] = Piece();
	}
	// move piece
	(*this)[_move.to] = _board[_move.from];
	(*this)[_move.from] = Piece();
	// check for promotion
	if (_board[_move.from].same_piece(PIECE_PAWN) &&
	(_move.to.rank() == 0 || _move.to.rank() == 7)) {
		if (_move.promote_result.same_piece(PIECE_NONE)) {
			(*this)[_move.to] = Piece(PIECE_QUEE, _board[_move.from].same_color(COLOR_WHITE) ?
			COLOR_WHITE : COLOR_BLACK);
		} else {
			(*this)[_move.to] = _move.promote_result;
		}
	}
	// update m_en_pessant_square
	if (_board[_move.from].same_piece(PIECE_PAWN) &&
	(_move.from.rank() == 1 || _move.from.rank() == 6) &&
	(_move.to.rank() == 3 || _move.to.rank() == 4)) {
		if (_move.from.rank() == 1) {
			m_en_pessant_square = Square_index(2, _move.from.file());
		} else {
			m_en_pessant_square = Square_index(5, _move.from.file());
		}
	}
	// update castle rights
	if (_board[_move.from].same_piece(PIECE_KING)) {
		if (_board[_move.from].same_color(COLOR_WHITE)) {
			m_castle_WK = m_castle_WQ = false;
		} else if (_board[_move.from].same_color(COLOR_BLACK)) {
			m_castle_BK = m_castle_BQ = false;
		}
	}
	if (_board[_move.from].same_piece(PIECE_ROOK)) {
		if (_board[_move.from].same_color(COLOR_WHITE) && _move.from.rank() == 7) {
			if (_move.from.file() == 0) {
				m_castle_WQ = false;
			} else if (_move.from.file() == 7) {
				m_castle_WK = false;
			}
		} else if (_board[_move.from].same_color(COLOR_BLACK) && _move.from.rank() == 0) {
			if (_move.from.file() == 0) {
				m_castle_BQ = false;
			} else if (_move.from.file() == 7) {
				m_castle_BK = false;
			}
		}
	}
	if (_board[_move.to].same_piece(PIECE_ROOK)) {
		if (_board[_move.to].same_color(COLOR_WHITE) && _move.to.rank() == 7) {
			if (_move.to.file() == 0) {
				m_castle_WQ = false;
			} else if (_move.to.file() == 7) {
				m_castle_WK = false;
			}
		} else if (_board[_move.to].same_color(COLOR_BLACK) && _move.to.rank() == 0) {
			if (_move.from.file() == 0) {
				m_castle_BQ = false;
			} else if (_move.to.file() == 7) {
				m_castle_BK = false;
			}
		}
	}
}

std::vector <Move> Board::moves_of_color(uint8_t color) const {
	std::vector <Move> result;
	for (uint8_t rank = 0; rank < 8; rank++) {
		for (uint8_t file = 0; file < 8; file++) {
			if ((*this)[rank][file].same_color(color)) {
				std::vector <Move> here = get_moves(*this, Square_index(rank, file));
				result.insert(result.end(), here.begin(), here.end());
			}
		}
	}
	return result;
}

std::vector <Move> Board::moves_of_square(const Square_index& square) const {
	return get_moves(*this, square);
}
