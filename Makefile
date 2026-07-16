SRC          := src
LIB          := lib
LIB_SPEC     := $(LIB)/Linux
SH           := shaders
SH_SRC       := $(SH)/src
TEST         := test

BIN           := bin
LIB_BIN       := $(BIN)/lib
LIB_SPEC_BIN  := $(LIB_BIN)/Linux
TEST_BIN      := $(BIN)/test

CXX               := g++
CXXFLAG           := -std=c++20 -m64 -march=x86-64 -pedantic-errors -Wall -ffast-math -MMD -MP -D NDEBUG

CC                := gcc
CFLAG             := -std=c2x -m64 -march=x86-64 -pedantic-errors -Wall -ffast-math -MMD -MP -fPIC -D NDEBUG

SC                := glslc
SFLAG             := --target-env=vulkan1.3 --target-spv=spv1.6 -O -Werror -MD

RE                := $(BIN)/libRE.a
OUT               := Game

SOURCES               := $(wildcard $(SRC)/*.cpp)
OBJECTS               := $(patsubst $(SRC)/%.cpp,$(BIN)/%.o,$(SOURCES))
DEPENDENCIES          := $(patsubst $(SRC)/%.cpp,$(BIN)/%.d,$(SOURCES))

LIB_SOURCES           := $(wildcard $(LIB)/*.c)
LIB_OBJECTS           := $(patsubst $(LIB)/%.c,$(LIB_BIN)/%.o,$(LIB_SOURCES))
LIB_DEPENDENCIES      := $(patsubst $(LIB)/%.c,$(LIB_BIN)/%.d,$(LIB_SOURCES))

LIB_SPEC_SOURCES      := $(wildcard $(LIB_SPEC)/*.c)
LIB_SPEC_OBJECTS      := $(patsubst $(LIB_SPEC)/%.c,$(LIB_SPEC_BIN)/%.o,$(LIB_SPEC_SOURCES))
LIB_SPEC_DEPENDENCIES := $(patsubst $(LIB_SPEC)/%.c,$(LIB_SPEC_BIN)/%.d,$(LIB_SPEC_SOURCES))

TEST_SOURCES          := $(wildcard $(TEST)/*.cpp)
TEST_OBJECTS          := $(patsubst $(TEST)/%.cpp,$(TEST_BIN)/%.o,$(TEST_SOURCES))
TEST_DEPENDENCIES     := $(patsubst $(TEST)/%.cpp,$(TEST_BIN)/%.d,$(TEST_SOURCES))

SHADER_SOURCES        := $(wildcard $(SH_SRC)/*.glsl)
SHADER_BINARIES       := $(patsubst $(SH_SRC)/%.glsl,$(SH)/%.glsl.spv,$(SHADER_SOURCES))
SHADER_DEPENDENCIES   := $(patsubst $(SH_SRC)/%.glsl,$(SH)/%.glsl.spv.d,$(SHADER_SOURCES))

.PHONY: all \
	clear \
	update_git \
	fetch_git

all: $(OUT) $(SHADER_BINARIES)

$(OUT): $(RE) $(TEST_OBJECTS)
	@$(CXX) $(CXXFLAG) $(TEST_OBJECTS) -o $@ -L $(BIN) -l RE -l dl -l X11 -l Xrandr -l Xinerama -l wayland-client -l xkbcommon

$(RE): $(OBJECTS) $(LIB_OBJECTS) $(LIB_SPEC_OBJECTS)
	@ar rs "$@" $^

$(BIN)/%.o: $(SRC)/%.cpp | $(BIN)
	@$(CXX) $(CXXFLAG) -c $< -o $@ -I $(LIB) -I $(LIB_SPEC) -I "$(HOME)/Vulkan SDK/x86_64/include"

$(TEST_BIN)/%.o: $(TEST)/%.cpp | $(TEST_BIN)
	@$(CXX) $(CXXFLAG) -c $< -o $@ -I $(SRC)

$(LIB_BIN)/%.o: $(LIB)/%.c | $(LIB_BIN)
	@$(CC) $(CFLAG) -c $< -o $@

$(LIB_SPEC_BIN)/%.o: $(LIB_SPEC)/%.c | $(LIB_SPEC_BIN)
	@$(CC) $(CFLAG) -c $< -o $@

$(SH)/Vertex_%.glsl.spv: $(SH_SRC)/Vertex_%.glsl
	@$(SC) $(SFLAG) -x glsl -fshader-stage=vertex $< -o $@

$(SH)/Fragment_%.glsl.spv: $(SH_SRC)/Fragment_%.glsl
	@$(SC) $(SFLAG) -x glsl -fshader-stage=fragment $< -o $@

$(SH)/Compute_%.glsl.spv: $(SH_SRC)/Compute_%.glsl
	@$(SC) $(SFLAG) -x glsl -fshader-stage=compute $< -o $@

-include $(DEPENDENCIES)

-include $(TEST_DEPENDENCIES)

-include $(LIB_DEPENDENCIES)

-include $(LIB_SPEC_DEPENDENCIES)

-include $(SHADER_DEPENDENCIES)

clear:
	-@rm -f $(SHADER_DEPENDENCIES) \
		$(OBJECTS) \
		$(DEPENDENCIES) \
		$(LIB_OBJECTS) \
		$(LIB_DEPENDENCIES) \
		$(LIB_SPEC_OBJECTS) \
		$(LIB_SPEC_DEPENDENCIES) \
		$(TEST_OBJECTS) \
		$(TEST_DEPENDENCIES)

update_git:
	@git add .
	@git commit -m "$(commit_name)"
	@git push -f

fetch_git:
	-@rm -f $(SRC)/* && rm -f $(SH)/*
	@git fetch --all
	@git reset --hard origin/main

$(BIN):
	-@mkdir -p $@

$(LIB_BIN): $(BIN)
	-@mkdir -p $@

$(LIB_SPEC_BIN): $(LIB_BIN)
	-@mkdir -p $@

$(TEST_BIN): $(BIN)
	-@mkdir -p $@
