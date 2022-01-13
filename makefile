
src_files = \
./src/driver.cpp \
./src/game-0-1/board.cpp \
./src/game-0-1/get_moves.cpp

all: release

release:
	g++ -Wshadow -Wall -std=c++17 $(src_files) -o driver -I ./src/ -O2 -lpthread

debug:
	g++ -Wshadow -Wall -std=c++17 ./src/*.cpp -o driver -I ./src/ \
	-g -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG -lpthread

debug_noflags:
	g++ -Wshadow -Wall -std=c++17 $(src_files) -o driver -I ./src/ -g -lpthread

util_binfile:
	g++ -Wshadow -Wall -std=c++17 ./src/util/binfile.cpp -o ./src/util/binfile -I ./src/ -O2 -lpthread
