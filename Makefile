SRC          = src
BIN          = bin
SH           = shaders

CC           = g++
CFLAG        = -m64 -march=x86-64 -pedantic-errors -Wall -ffast-math -std=c++17
LDFLAG       = -l RE -I $(SRC) -I /usr/ -L $(BIN) -l X11 -l wayland-client

SC           = glslc
SFLAG        = --target-env=vulkan1.3 --target-spv=spv1.6 -O

# shader files
VERT_GL      = $(SH)/vertex.glsl
FRAG_GL      = $(SH)/fragment.glsl
VERT_HL      = $(SH)/vertex.hlsl
FRAG_HL      = $(SH)/fragment.hlsl

RE           = $(BIN)/libRE.a
OUT          = Game

.PHONY: all, compile_shaders, update_git, fetch_git

all:
	@make --no-print-directory $(RE)
	@make --no-print-directory $(OUT)

$(OUT): $(RE) *.cpp
	@$(CC) $(CFLAG) -o "$(OUT)" *.cpp $(LDFLAG)

$(RE): $(SRC)/*
	-@rm -f *.o $(BIN)/*.o
	@if [ "$(wildcard $(BIN)/*.gch)" != "" ]; then \
		mv $(BIN)/*.gch $(SRC); \
	fi
	@$(CC) $(CFLAG) -c $(SRC)/*.cpp -I$(SRC) || (rm -f *.o; exit 1)
	@mv *.o $(BIN)
	@if [ "$(wildcard $(SRC)/*.gch)" != "" ]; then \
		mv $(SRC)/*.gch $(BIN); \
	fi
	-@rm -f $(RE)
	@ar rs "$(RE)" $(BIN)/*.o || (rm -f $(RE); exit 1)

compile_shaders: $(SH)/*
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
