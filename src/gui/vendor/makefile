# makefile responsible for building and extracting GUI submodules
## these include
## - glfw
## - glew
## - glm
## - stb_image
### everything is, if linked, statically linked

# main target - builds and exports everything
all: glfw glew glm
	make init_submodules

# target for adding any missing submodules
.PHONY: init_submodules
init_submodules:
	git submodule deinit -f ./glfw/
	git submodule update --init ./glfw/
	
	git submodule deinit -f ./glew/
	git submodule update --init ./glew/
	
	git submodule deinit -f ./glm/
	git submodule update --init ./glm/

# build and export glfw
.PHONY: glfw
glfw:
	git submodule deinit -f ./glfw/
	git submodule update --init ./glfw/
	
	if [ -d "./glfw/build/" ]; then rm -rf ./glfw/build/; fi;
	mkdir ./glfw/build/ && cmake -S ./glfw/ -B ./glfw/build/
	cd ./glfw/build/ && make glfw
	
	if [ ! -d "./bin/glfw/" ]; then mkdir -p ./bin/glfw/; fi;
	
	cp ./glfw/build/src/libglfw3.a ./bin/glfw/libglfw3.a
	cp ./glfw/include/GLFW/glfw3.h ./bin/glfw/glfw3.h

# build and export glew
.PHONY: glew
glew:
	git submodule deinit -f ./glew/
	git submodule update --init ./glew/
	
	cd ./glew/auto/ && make
	cd ./glew/ && make
	
	if [ ! -d "./bin/glew/" ]; then mkdir -p ./bin/glew/; fi;
	
	cp ./glew/lib/libGLEW.a ./bin/glew/libGLEW.a
	cp ./glew/include/GL/glew.h ./bin/glew/glew.h

# export glm
.PHONY: glm
glm:
	git submodule deinit -f ./glm/
	git submodule update --init ./glm/
	
	if [ ! -d "./bin/glm/" ]; then mkdir -p ./bin/glm/; fi;
	
	cp -r ./glm/glm/* ./bin/glm/
