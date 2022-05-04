# makefile responsible for building and extracting GUI submodules
## these include
## - glfw
## - glew
## - glm
## - stb_image
### everything is, if linked, statically linked

# target for adding any missing submodules
.PHONY: init_submodules
init_submodules:
	git submodule deinit -f ./glfw/
	git submodule update --init ./glfw/
	
	git submodule deinit -f ./glew/
	git submodule update --init ./glew/
	
	git submodule deinit -f ./glm/
	git submodule update --init ./glm/
