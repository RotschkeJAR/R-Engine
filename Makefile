SRC          = src
LIB          = lib
LIB_SPEC     = $(LIB)/Linux
BIN          = bin
LIB_BIN      = $(BIN)/lib
SH           = shaders

CC           = g++
CFLAG        = -m64 -march=x86-64 -pedantic-errors -Wall -ffast-math
LDFLAG       = -lRE -I$(SRC) -I/usr/ -L$(BIN) -lX11 -lwayland-client

SC           = glslc
SFLAG        = --target-env=vulkan1.0 --target-spv=spv1.0 -O

# shader files
VERT_GL      = $(SH)/vertex.glsl
FRAG_GL      = $(SH)/fragment.glsl
VERT_HL      = $(SH)/vertex.hlsl
FRAG_HL      = $(SH)/fragment.hlsl

RE           = $(BIN)/libRE.a
OUT          = Game

.PHONY: all, update_git, fetch_git

all:
	@make --no-print-directory $(OUT)

$(OUT): $(RE) *.cpp
	@for file in *.cpp; do \
		echo $${file}; \
		$(CC) $(CFLAG) -std=c++20 -x c++ -o "$(OUT)" "$${file}" $(LDFLAG); \
	done

$(RE): $(SRC)/* $(SH)/*.spv $(LIB_BIN)/*
	-@rm -f *.o $(BIN)/*.o
	@if [ "$(wildcard $(BIN)/*.gch)" != "" ]; then \
		mv $(BIN)/*.gch $(SRC); \
	fi
	@errorCaused=false; \
	for file in $(SRC)/*.cpp; do \
		echo $${file}; \
		if ! $(CC) $(CFLAG) -std=c++20 -x c++ -c "$${file}" -I$(LIB) -I$(LIB_SPEC) -I"$(HOME)/Vulkan SDK/x86_64/include"; then \
			errorCaused=true; \
		fi; \
	done; \
	if ! $$errorCaused; then \
		mv *.o $(BIN); \
		if [ "$(wildcard $(SRC)/*.gch)" != "" ]; then \
			mv $(SRC)/*.gch $(BIN); \
		fi; \
		rm -f $(RE); \
		ar rs "$(RE)" $(BIN)/*.o $(LIB_BIN)/*.o || (rm -f $(RE); exit 1); \
		echo "ENGINE - SUCCESS"; \
	else \
		rm -f *.o; \
		rm $(SRC)/*.gch; \
		echo "ENGINE - ERROR: Failed compiling"; \
	fi

$(LIB_BIN)/*: $(LIB)/* $(LIB_SPEC)/*
	@errorCaused=false; \
	if [ "$(wildcard $(LIB)/*.c)" != "" ]; then \
		for file in $(LIB)/*.c; do \
			if ! $(CC) $(CFLAG) -std=c2x -x c -c "$${file}" || (rm -f *.o; exit 1); then \
				errorCaused=true; \
			fi; \
		done; \
	fi; \
	if [ "$(wildcard $(LIB)/*.cpp)" != "" ]; then \
		for file in $(LIB)/*.cpp; do \
			if ! $(CC) $(CFLAG) -std=c++20 -x c++ -c "$${file}" || (rm -f *.o; exit 1); then \
				errorCaused=true; \
			fi; \
		done; \
	fi; \
	if [ "$(wildcard $(LIB_SPEC)/*.c)" != "" ]; then \
		for file in $(LIB_SPEC)/*.c; do \
			if ! $(CC) $(CFLAG) -std=c2x -x c -c "$${file}" || (rm -f *.o; exit 1); then \
				errorCaused=true; \
			fi; \
		done; \
	fi; \
	if [ "$(wildcard $(LIB_SPEC)/*.cpp)" != "" ]; then \
		for file in $(LIB_SPEC)/*.cpp; do \
			if ! $(CC) $(CFLAG) -std=c++20 -x c++ -c "$${file}" || (rm -f *.o; exit 1); then \
				errorCaused=true; \
			fi; \
		done; \
	fi; \
	if ! $$errorCaused; then \
		rm -f *.o $(LIB_BIN)/*.o; \
		mv *.o $(LIB_BIN); \
		echo "LIBRARIES - SUCCESS"
	else \
		rm -f *.o; \
		echo "LIBRARIES - ERROR: Failed compiling libraries"
	fi

$(SH)/*.spv: $(SH)/*.glsl
	-@rm -f $(SH)/*.spv
	@for vertexShader in $(SH)/*_vertex.glsl; do \
		echo $${vertexShader}; \
		$(SC) $(SFLAG) -x glsl -fshader-stage=vertex -o $${vertexShader}.spv $${vertexShader}; \
	done
	@for fragmentShader in $(SH)/*_fragment.glsl; do \
		echo $${fragmentShader}; \
		$(SC) $(SFLAG) -x glsl -fshader-stage=fragment -o $${fragmentShader}.spv $${fragmentShader}; \
	done

update_git:
	@git add .
	@git commit -m "$(commit_name)"
	@git push -f

fetch_git:
	@git fetch --all
	@git reset --hard origin/main
