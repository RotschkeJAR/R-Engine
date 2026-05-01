SRC          = src
LIB          = lib
LIB_SPEC     = $(LIB)/Linux
BIN          = bin
LIB_BIN      = $(BIN)/lib
SH           = shaders
TEST         = test

CC           = g++
CFLAG        = -m64 -march=x86-64 -pedantic-errors -Wall -ffast-math

SC           = glslc
SFLAG        = --target-env=vulkan1.3 --target-spv=spv1.6 -O

RE           = $(BIN)/libRE.a
OUT          = Game

.PHONY: all, compile_shaders, update_git, fetch_git

all:
	@make --no-print-directory $(SH)/*.spv
	@make --no-print-directory $(OUT)

$(OUT): $(RE) $(TEST)/*.cpp
	@errorCaused=false; \
	for file in $(TEST)/*.cpp; do \
		echo $${file}; \
		$(CC) $(CFLAG) -std=c++20 -x c++ -c "$${file}" -I "$(SRC)" -I "/usr/" || errorCaused=true; \
	done; \
	if $$errorCaused; then \
		echo "GAME - ERROR: Failed compiling"; \
		rm -f *.o && rm -f $(TEST)/*.gch; \
		exit 1; \
	fi
	-@rm -f $(TEST)/*.gch
	@if ! $(CC) $(CFLAG) *.o -o "$(OUT)" -L $(BIN) -l RE -l X11 -l Xrandr -l Xinerama -l wayland-client; then \
		echo "GAME - ERROR: Failed linking"; \
		rm -f *.o; \
		exit 1; \
	fi
	-@rm -f *.o
	-@echo "GAME - SUCCESS"

$(RE): $(SRC)/* $(LIB_BIN)/*
	-@rm -f *.o $(BIN)/*.o
	@if [ "$(wildcard $(BIN)/*.gch)" != "" ]; then \
		mv $(BIN)/*.gch $(SRC); \
	fi
	@errorCaused=false; \
	for file in $(SRC)/*.cpp; do \
		echo $${file}; \
		if ! $(CC) $(CFLAG) -std=c++20 -x c++ -c "$${file}" -I $(LIB) -I $(LIB_SPEC) -I "$(HOME)/Vulkan SDK/x86_64/include"; then \
			errorCaused=true; \
		fi; \
	done; \
	if $$errorCaused; then \
		echo "ENGINE - ERROR: Failed compiling"; \
		rm -f *.o && rm -f $(SRC)/*.gch; \
		exit 1; \
	fi
	@mv *.o $(BIN)
	@if [ "$(wildcard $(SRC)/*.gch)" != "" ]; then \
		mv $(SRC)/*.gch $(BIN); \
	fi
	-@rm -f $(RE)
	if ! ar rs "$(RE)" $(BIN)/*.o $(LIB_BIN)/*.o; then \
		echo "ENGINE - ERROR: Failed creating static library"
		rm -f $(RE); \
		exit 1; \
	fi
	-@echo "ENGINE - SUCCESS"

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
	if $$errorCaused; then \
		echo "LIBRARIES - ERROR: Failed compiling libraries"; \
		rm -f *.o; \
		exit 1; \
	fi
	-@rm -f *.o $(LIB_BIN)/*.o
	@mv *.o $(LIB_BIN)
	-@echo "LIBRARIES - SUCCESS"

$(SH)/*.spv: $(SH)/*.glsl
	@make --no-print-directory compile_shaders

compile_shaders:
	-@rm -f $(SH)/*.spv
	@for vertexShader in $(SH)/*_vertex.glsl; do \
		echo $${vertexShader}; \
		$(SC) $(SFLAG) -x glsl -fshader-stage=vertex -o "$${vertexShader}.spv" "$${vertexShader}"; \
	done
	@for fragmentShader in $(SH)/*_fragment.glsl; do \
		echo $${fragmentShader}; \
		$(SC) $(SFLAG) -x glsl -fshader-stage=fragment -o "$${fragmentShader}.spv" "$${fragmentShader}"; \
	done
	@for computeShader in $(SH)/*_compute.glsl; do \
		echo $${computeShader}; \
		$(SC) $(SFLAG) -x glsl -fshader-stage=compute -o "$${computeShader}.spv" "$${computeShader}"; \
	done

update_git:
	@git add .
	@git commit -m "$(commit_name)"
	@git push -f

fetch_git:
	-@rm -f $(SRC)/* && rm -f $(SH)/*
	@git fetch --all
	@git reset --hard origin/main
