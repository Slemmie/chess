

all: release

release:
	g++ -Wshadow -Wall -std=c++17 ./src/*.cpp -o driver -I ./src/ -O2

debug:
	g++ -Wshadow -Wall -std=c++17 ./src/*.cpp -o driver -I ./src/ \
	-g -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG
