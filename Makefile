SRC          = src
BIN          = bin

CC           = g++
CFLAG        = -m64 -march=x86-64 -Wall -ffast-math -std=c++17
LDFLAG       = -lRE -I$(SRC) -L$(BIN) -lGL

RE           = $(BIN)/libRE.a
OUT          = Game

.PHONY: all

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

update_git:
	@git add .
	@git commit -m "$(commit_name)"
	@git push

fetch_git:
	@git pull
