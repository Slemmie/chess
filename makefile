

all: release

release:
	g++ -Wshadow -Wall -std=c++17 ./src/*.cpp -o driver -I ./src/ -O2

debug:
	g++ -Wshadow -Wall -std=c++17 ./src/*.cpp -o driver -I ./src/ \
	-g -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG

debug_noflags:
	g++ -Wshadow -Wall -std=c++17 ./src/*.cpp -o driver -I ./src/ -g

util_binfile:
	g++ -Wshadow -Wall -std=c++17 ./src/util/binfile.cpp -o ./src/util/binfile -I ./src/ -O2
