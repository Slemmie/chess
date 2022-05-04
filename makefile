# makefile responsible for building the main program
# has targets for setting up any other builds elsewhere

# main target (release) - does not build dependencies
all: release
	make clean

# take care of building and exporting dependencies
deps:
	cd ./src/gui/vendor/ && make

# C++ flags
CXX_FLAGS = -Wshadow -Wall -std=c++17
CXX_FLAGS_RELEASE = -O2
CXX_FLAGS_DEBUG = -g -fsanitize=undefined,address -D_GLIBCXX_DEBUG

# source files
SRC_FILES =                 \
$(wildcard ./src/*.cpp)     \
$(wildcard ./src/gui/*.cpp)

# header files
HDR_FILES =               \
$(wildcard ./src/*.h)     \
$(wildcard ./src/gui/*.h)

# object files
OBJ_FILES_RELEASE = ${SRC_FILES:.cpp=_release.o}
OBJ_FILES_DEBUG = ${SRC_FILES:.cpp=_debug.o}

# links
LINKS_GUI =                          \
-L ./src/gui/vendor/bin/glfw/ -lglfw3 \
-ldl                                 \
-lGL                                 \
-lpthread                            \
-L ./src/gui/vendor/bin/glew/ -lGLEW
LINKS = $(LINKS_GUI)

# release target
release: ${OBJ_FILES_RELEASE}
	g++ $(CXX_FLAGS) $^ -o chess $(CXX_FLAGS_RELEASE) $(LINKS)

# C++ release compilation
%_release.o: %.cpp ${HDR_FILES}
	g++ $(CXX_FLAGS) $< -c -o $@ $(CXX_FLAGS_RELEASE) $(LINKS) -I ./src/gui/vendor/bin/

# debug target
debug: ${OBJ_FILES_DEBUG}
	g++ $(CXX_FLAGS) $^ -o chess $(CXX_FLAGS_DEBUG) $(LINKS)

# C++ debug compilation
%_debug.o: %.cpp ${HDR_FILES}
	g++ $(CXX_FLAGS) $< -c -o $@ $(CXX_FLAGS_DEBUG) $(LINKS) -I ./src/gui/vendor/bin/

# clean up temporary files
.PHONY: clean
clean:
	find . -name *.o -delete
