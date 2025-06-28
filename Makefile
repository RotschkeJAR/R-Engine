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

.PHONY: all, compile_shaders, update_git, fetch_git

all:
	@make --no-print-directory $(LIB_BIN)/*
	@make --no-print-directory $(RE)
	@make --no-print-directory $(SH)/*.spv
	@make --no-print-directory $(OUT)

$(OUT): $(RE) *.cpp
	@$(CC) $(CFLAG) -o "$(OUT)" *.cpp $(LDFLAG)

$(RE): $(SRC)/*
	-@rm -f *.o $(BIN)/*.o
	@if [ "$(wildcard $(BIN)/*.gch)" != "" ]; then \
		mv $(BIN)/*.gch $(SRC); \
	fi
	@$(CC) $(CFLAG) -std=c++20 -x c++ -c $(SRC)/*.cpp -I$(LIB) -I$(LIB_SPEC) -I"$(HOME)/Vulkan SDK/x86_64/include" || (rm -f *.o; exit 1)
	@mv *.o $(BIN)
	@if [ "$(wildcard $(SRC)/*.gch)" != "" ]; then \
		mv $(SRC)/*.gch $(BIN); \
	fi
	-@rm -f $(RE)
	@ar rs "$(RE)" $(BIN)/*.o $(LIB_BIN)/*.o || (rm -f $(RE); exit 1)

$(LIB_BIN)/*: $(LIB)/* $(LIB_SPEC)/*
	@rm -f *.o $(LIB_BIN)/*.o
	@if [ "$(wildcard $(LIB)/*.c)" != "" ]; then \
		$(CC) $(CFLAG) -std=c2x -x c -c $(LIB)/*.c || (rm -f *.o; exit 1); \
	fi
	@if [ "$(wildcard $(LIB)/*.cpp)" != "" ]; then \
		$(CC) $(CFLAG) -std=c++20 -x c++ -c $(LIB)/*.cpp || (rm -f *.o; exit 1); \
	fi
	@if [ "$(wildcard $(LIB_SPEC)/*.c)" != "" ]; then \
		$(CC) $(CFLAG) -std=c2x -x c -c $(LIB_SPEC)/*.c || (rm -f *.o; exit 1); \
	fi
	@if [ "$(wildcard $(LIB_SPEC)/*.cpp)" != "" ]; then \
		$(CC) $(CFLAG) -std=c++20 -x c++ -c $(LIB_SPEC)/*.cpp || (rm -f *.o; exit 1); \
	fi
	@mv *.o $(LIB_BIN)

$(SH)/*.spv: $(SH)/*.glsl
	-@rm -f $(SH)/*.spv
	-@if [ -f "$(VERT_GL)" ]; then \
		$(SC) $(SFLAG) -x glsl -fshader-stage=vertex -o $(SH)/vertex.spv $(VERT_GL); \
	elif [ -f "$(VERT_HL)" ]; then \
		$(SC) $(SFLAG) -x hlsl -fshader-stage=vertex -o $(SH)/vertex.spv $(VERT_HL); \
	else \
		echo "No vertex shader code found!"; \
	fi
	-@if [ -f "$(FRAG_GL)" ]; then \
		$(SC) $(SFLAG) -x glsl -fshader-stage=fragment -o $(SH)/fragment.spv $(FRAG_GL); \
	elif [ -f "$(FRAG_HL)" ]; then \
		$(SC) $(SFLAG) -x hlsl -fshader-stage=fragment -o $(SH)/fragment.spv $(FRAG_HL); \
	else \
		echo "No fragment shader code found!"; \
	fi

update_git:
	@git add .
	@git commit -m "$(commit_name)"
	@git push -f

fetch_git:
	@git fetch --all
	@git reset --hard origin/main
