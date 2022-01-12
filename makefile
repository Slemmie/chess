
#src_files = \
#./src/*.cpp \
#./src/ai-0-1/*.cpp

src_files = \
./src/driver.cpp \
./src/board.cpp \
./src/get_moves.cpp

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
