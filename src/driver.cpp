#include <game.h>

#include <ai-0-1/ai.h>

#include <ai-0-1/learn/train.h>

#include <iostream>
#include <map>
#include <string>
#include <cstring>

Move read_move() {
	std::cout << "move > ";
	std::cout.flush();
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

void training() {
	std::string srcdir;
	int32_t id;
	std::cout << "source dir > ";
	std::cin >> srcdir;
	std::cout << "id > ";
	std::cin >> id;
	std::cout << "is source dir '" << srcdir << "' with id " << id << " correct? (y/n) > ";
	std::string yorn;
	std::cin >> yorn;
	if (yorn != "y" && yorn != "Y") {
		training();
		return;
	}
	ai01::Train(srcdir, id, std::make_shared <ai01::Book_moves> ("./src/ai-0-1/book/games_coord"));
}

int main(int argc, char** argv) {
	
	bool black_ai = false;
	bool white_ai = false;
	
	uint64_t millis = 5000;
	
	bool training_flag = false;
	
	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "train")) {
			training_flag = true;
		}
		if (!strcmp(argv[i], "w")) {
			white_ai = true;
		}
		if (!strcmp(argv[i], "b")) {
			black_ai = true;
		}
		std::string s = argv[i];
		bool num = true;
		for (char c : s) {
			num &= isdigit(c);
		}
		if (num) {
			millis = std::stoll(s);
		}
	}
	
	if (training_flag) {
		training();
		return 0;
	}
	
	ai01::AI ai("./src/ai-0-1/book/games_coord");
	//ai01::AI <ai01::Learn_eval> ai(
	//ai01::Learn_eval("./src/ai-0-1/learn/batches/0/subject_0"), "./src/ai-0-1/book/games_coord");
	
	Board board(DEFAULT_START_POS);
	ai01::Move_table movetable;
	
	while (true) {
		ai.push_hash(Board_hash::hash(board));
		movetable.push(Board_hash::hash(board));
		std::cout << "\n\n" << board << std::endl;
		uint8_t gamestate = game_state(board);
		if (gamestate == GAME_STATE_ALIVE && movetable.count(Board_hash::hash(board)) >= 3) {
			gamestate = GAME_STATE_DRAW;
		}
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
		if (white_ai && board.turn() == COLOR_WHITE) {
			Move desired_move = ai.find_move(board, millis, true);
			board = Board(board, desired_move);
			continue;
		}
		if (black_ai && board.turn() == COLOR_BLACK) {
			Move desired_move = ai.find_move(board, millis, true);
			board = Board(board, desired_move);
			continue;
		}
		std::vector <Move> moves = board.moves_of_color(board.turn());
		bool exists = false;
		Move desired_move = moves[0];
		do {
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
