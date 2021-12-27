#include <game.h>

#include <iostream>
#include <map>
#include <string>

Move read_move() {
	std::string line;
	std::getline(std::cin, line);
	uint8_t color = COLOR_NONE;
	if (line.empty() || (line[0] != 'w' && line[0] != 'b')) {
		std::cout << "bad input" << std::endl;
		return read_move();
	}
	color = line[0] == 'w' ? COLOR_WHITE : COLOR_BLACK;
	line.erase(line.begin());
	if ((int)line.size() <= 3) {
		std::cout << "bad input" << std::endl;
		return read_move();
	}
	line.erase(line.begin());
	if (!(line[0] >= 'a' && line[0] <= 'h')) {
		std::cout << "bad input" << std::endl;
		return read_move();
	}
	if (!isdigit(line[1])) {
		std::cout << "bad input" << std::endl;
		return read_move();
	}
	Square_index from(line.substr(0, 2));
	line.erase(line.begin(), line.begin() + 2);
	if (line.empty()) {
		std::cout << "bad input" << std::endl;
		return read_move();
	}
	line.erase(line.begin());
	if (!(line[0] >= 'a' && line[0] <= 'h')) {
		std::cout << "bad input" << std::endl;
		return read_move();
	}
	if (!isdigit(line[1])) {
		std::cout << "bad input" << std::endl;
		return read_move();
	}
	Square_index to(line.substr(0, 2));
	line.erase(line.begin(), line.begin() + 2);
	if (!line.empty()) {
		line.erase(line.begin());
	}
	if (line == "queen") {
		return Move(from, to, Piece(PIECE_QUEE, color));
	} else if (line == "rook") {
		return Move(from, to, Piece(PIECE_ROOK, color));
	} else if (line == "knight") {
		return Move(from, to, Piece(PIECE_KNIG, color));
	} else if (line == "bishup") {
		return Move(from, to, Piece(PIECE_BISH, color));
	}
	return Move(from, to);
}

int main() {
	
	Board board(DEFAULT_START_POS);
	
	while (true) {
		std::cout << "\n\n" << board << std::endl;
		uint8_t gamestate = game_state(board);
		if (gamestate != GAME_STATE_ALIVE) {
			if (gamestate == GAME_STATE_DRAW) {
				std::cout << "\n     DRAW\n" << std::endl;
			}
			if (gamestate == GAME_STATE_WHITE_WINS) {
				std::cout << "\n     WHITE WINS\n" << std::endl;
			}
			if (gamestate == GAME_STATE_BLACK_WINS) {
				std::cout << "\n     BLACK WINS\n" << std::endl;
			}
			return 0;
		}
		std::vector <Move> moves = board.moves_of_color(board.turn());
		bool exists = false;
		Move desired_move = moves[0];
		do {
			std::cout << "move > ";
			std::cout.flush();
			desired_move = read_move();
			for (Move move : moves) {
				if (desired_move == move) {
					exists = true;
					break;
				}
			}
		} while (!exists);
		board = Board(board, desired_move);
	}
	
}
