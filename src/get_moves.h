#include <square_index.h>
#include <board.h>

#include <vector>

#define GAME_STATE_ALIVE        (1 << 1)
#define GAME_STATE_WHITE_WINS   (1 << 2)
#define GAME_STATE_BLACK_WINS   (1 << 3)
#define GAME_STATE_DRAW         (1 << 4)

uint8_t game_state(const Board& board);
uint8_t game_state(const Board& board, const std::vector <Move>& avail_moves);

bool is_check(const Board& board, uint8_t king_color);

std::vector <Move> get_moves(const Board& board, const Square_index& square);
std::vector <Move> get_moves_pawn(const Board& board, const Square_index& square);
std::vector <Move> get_moves_rook(const Board& board, const Square_index& square);
std::vector <Move> get_moves_knight(const Board& board, const Square_index& square);
std::vector <Move> get_moves_bishup(const Board& board, const Square_index& square);
std::vector <Move> get_moves_queen(const Board& board, const Square_index& square);
std::vector <Move> get_moves_king(const Board& board, const Square_index& square);
