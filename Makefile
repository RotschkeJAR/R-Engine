# GNU Make 4.4.1

# Possible values:
#	debug
#	release
BUILD    ?= release

# Possible values:
#	GNU
#	MSVC
COMPILER ?= GNU

WINDOWS      := Windows_NT
LINUX        := Linux

ifeq ($(COMPILER),GNU)
	CC            := gcc
	CFLAG         := -std=c2x -m64 -march=x86-64 -pedantic-errors -Wall -ffast-math -MMD -MP -fPIC -O2
	CXX           := g++
	CXXFLAG       := -std=c++20 -m64 -march=x86-64 -pedantic-errors -Wall -ffast-math -MMD -MP -O2
	LD            := g++
ifeq ($(BUILD),release)
	CFLAG         += -D NDEBUG
	CXXFLAG       += -D NDEBUG
endif
ifeq ($(OS),Windows_NT)
	CFLAG         += -D _WIN32_WINNT=0x0A00
	CXXFLAG       += -D _WIN32_WINNT=0x0A00
endif
else ifeq($(COMPILER),MSVC)
	CC            := cl
	CFLAG         := /nologo /EHsc /TP /std:c17 /W1 /favor:blend /D _WIN32_WINNT=0x0A00
	CXX           := cl
	CXXFLAG       := /nologo /EHsc /TP /std:c++20 /W1 /favor:blend /D _WIN32_WINNT=0x0A00
	LD            := link
ifeq ($(BUILD),release)
	CFLAG         += /D NDEBUG
	CXXFLAG       += /D NDEBUG
endif
else
	$(error Unknown compiler selected '$(COMPILER)')
endif

SC           := glslc
SFLAG        := --target-env=vulkan1.3 --target-spv=spv1.6 -O -Werror -MD

SRC          := src
LIB_SRC          := lib
SH           := shaders
SH_SRC       := $(SH)/src
TEST         := test

BIN          := bin
LIB_BIN      := $(BIN)/lib
TEST_BIN     := $(BIN)/test

OUT          := Game

SOURCES               := $(wildcard $(SRC)/*.cpp)
DEPENDENCIES          := $(patsubst $(SRC)/%.cpp,$(BIN)/%.d,$(SOURCES))

LIB_SOURCES           := $(wildcard $(LIB_SRC)/*.c)
LIB_DEPENDENCIES      := $(patsubst $(LIB_SRC)/%.c,$(LIB_BIN)/%.d,$(LIB_SOURCES))

TEST_SOURCES          := $(wildcard $(TEST)/*.cpp)
TEST_DEPENDENCIES     := $(patsubst $(TEST)/%.cpp,$(TEST_BIN)/%.d,$(TEST_SOURCES))

SHADER_SOURCES        := $(wildcard $(SH_SRC)/*.glsl)
SHADER_BINARIES       := $(patsubst $(SH_SRC)/%.glsl,$(SH)/%.glsl.spv,$(SHADER_SOURCES))
SHADER_DEPENDENCIES   := $(patsubst $(SH_SRC)/%.glsl,$(SH)/%.glsl.spv.d,$(SHADER_SOURCES))

ifeq ($(OS),Windows_NT)
	LIB_SPEC          := $(LIB_SRC)/Windows
	LIB_SPEC_BIN      := $(LIB_BIN)/Windows
	VULKAN_LIB        := "C:\VulkanSDK\Include"
else ifeq ($(OS),Linux)
	LIB_SPEC          := $(LIB_SRC)/Linux
	LIB_SPEC_BIN      := $(LIB_BIN)/Linux
	VULKAN_LIB        := "$(HOME)/Vulkan SDK/x86_64/include"
else
	$(error Unknown operating system '$(OS)')
endif

LIB_SPEC_SOURCES      := $(wildcard $(LIB_SPEC)/*.c)
LIB_SPEC_DEPENDENCIES := $(patsubst $(LIB_SPEC)/%.c,$(LIB_SPEC_BIN)/%.d,$(LIB_SPEC_SOURCES))

ifeq ($(COMPILER),GNU)
	OBJECTS           := $(patsubst $(SRC)/%.cpp,$(BIN)/%.o,$(SOURCES))
	LIB_OBJECTS       := $(patsubst $(LIB_SRC)/%.c,$(LIB_BIN)/%.o,$(LIB_SOURCES))
	LIB_SPEC_OBJECTS  := $(patsubst $(LIB_SPEC)/%.c,$(LIB_SPEC_BIN)/%.o,$(LIB_SPEC_SOURCES))
	TEST_OBJECTS      := $(patsubst $(TEST)/%.cpp,$(TEST_BIN)/%.o,$(TEST_SOURCES))
	RE                := $(BIN)/libRE.a
else ifeq ($(COMPILER),MSVC)
	OBJECTS           := $(patsubst $(SRC)/%.cpp,$(BIN)/%.obj,$(SOURCES))
	LIB_OBJECTS       := $(patsubst $(LIB_SRC)/%.c,$(LIB_BIN)/%.obj,$(LIB_SOURCES))
	LIB_SPEC_OBJECTS  := $(patsubst $(LIB_SPEC)/%.c,$(LIB_SPEC_BIN)/%.obj,$(LIB_SPEC_SOURCES))
	TEST_OBJECTS      := $(patsubst $(TEST)/%.cpp,$(TEST_BIN)/%.obj,$(TEST_SOURCES))
	RE                := $(BIN)/RE.lib
else
	$(error Unknown compiler selected '$(COMPILER)')
endif

.PHONY: release \
	debug \
	all \
	clear \
	update_git \
	fetch_git

release:
	@$(MAKE) all BUILD=release --no-print-directory

debug:
	@$(MAKE) all BUILD=debug --no-print-directory

all: $(OUT) $(SHADER_BINARIES)

$(OUT): $(TEST_OBJECTS) $(RE)
ifeq ($(OS),Windows_NT)
ifeq ($(BUILD),debug)
ifeq ($(COMPILER),GNU)
	@$(LD) $< -o $@ -L $(BIN) -l RE -l gdi32 -l user32 -static-libgcc -static-libstdc++
else ifeq ($(COMPILER),MSVC)
	@$(LD) /NOLOGO $(subst /,\,$^) gdi32.lib user32.lib /OUT:"$@" /SUBSYSTEM:CONSOLE
else
	$(error Unknown compiler selected '$(COMPILER)')
endif
else ifeq ($(BUILD),release)
ifeq ($(COMPILER),GNU)
	@$(LD) $< -o $@ -L $(BIN) -l RE -l gdi32 -l user32 -static-libgcc -static-libstdc++ -mwindows
else ifeq ($(COMPILER),MSVC)
	@$(LD) /NOLOGO $(subst /,\,$^) gdi32.lib user32.lib /OUT:"$@" /SUBSYSTEM:WINDOWS
else
	$(error Unknown compiler selected '$(COMPILER)')
endif
endif
else ifeq ($(OS),Linux)
	@$(LD) $< -o $@ -L $(BIN) -l RE -l dl -l X11 -l Xrandr -l Xinerama -l wayland-client -l xkbcommon
else
	$(error Unknown operating system '$(OS)')
endif

$(RE): $(OBJECTS) $(LIB_OBJECTS) $(LIB_SPEC_OBJECTS)
ifeq ($(COMPILER),GNU)
	@ar rs "$@" $^
else ifeq ($(COMPILER),MSVC)
	@lib /NOLOGO /OUT:"$@" $(subst /,\,$^)
else
	$(error Unknown compiler selected '$(COMPILER)')
endif

ifeq ($(COMPILER),GNU)

$(BIN)/%.o: $(SRC)/%.cpp | $(BIN)
	-@echo $<
	@$(CXX) $(CXXFLAG) -c $< -o $@ -I $(LIB_SRC) -I $(LIB_SPEC) -I $(VULKAN_LIB)

$(TEST_BIN)/%.o: $(TEST)/%.cpp | $(TEST_BIN)
	-@echo $<
	@$(CXX) $(CXXFLAG) -c $< -o $@ -I $(SRC)

$(LIB_BIN)/%.o: $(LIB_SRC)/%.c | $(LIB_BIN)
	-@echo $<
	@$(CC) $(CFLAG) -c $< -o $@

$(LIB_SPEC_BIN)/%.o: $(LIB_SPEC)/%.c | $(LIB_SPEC_BIN)
	-@echo $<
	@$(CC) $(CFLAG) -c $< -o $@

else ifeq ($(COMPILER),MSVC)

$(BIN)/%.obj: $(SRC)/%.cpp | $(BIN)
	@$(CXX) $(CXXFLAG) /c $(subst /,\,$<) /Fo:"$(subst /,\,$@)" /I $(LIB_SRC) /I $(LIB_SPEC) /I $(VULKAN_LIB)

$(TEST_BIN)/%.obj: $(TEST)/%.cpp | $(TEST_BIN)
	@$(CXX) $(CXXFLAG) /c $(subst /,\,$<) /Fo:"$(subst /,\,$@)" /I $(SRC)

$(LIB_BIN)/%.obj: $(LIB_SRC)/%.c | $(LIB_BIN)
	@$(CC) $(CFLAG) /c $(subst /,\,$<) /Fo:"$(subst /,\,$@)"

$(LIB_SPEC_BIN)/%.obj: $(LIB_SPEC)/%.c | $(LIB_SPEC_BIN)
	@$(CC) $(CFLAG) /c $(subst /,\,$<) /Fo:"$(subst /,\,$@)"

else
	$(error Unknown compiler selected '$(COMPILER)')
endif

$(SH)/Vertex_%.glsl.spv: $(SH_SRC)/Vertex_%.glsl
	-@echo $<
	@$(SC) $(SFLAG) -x glsl -fshader-stage=vertex $< -o $@

$(SH)/Fragment_%.glsl.spv: $(SH_SRC)/Fragment_%.glsl
	-@echo $<
	@$(SC) $(SFLAG) -x glsl -fshader-stage=fragment $< -o $@

$(SH)/Compute_%.glsl.spv: $(SH_SRC)/Compute_%.glsl
	-@echo $<
	@$(SC) $(SFLAG) -x glsl -fshader-stage=compute $< -o $@

-include $(DEPENDENCIES)

-include $(TEST_DEPENDENCIES)

-include $(LIB_DEPENDENCIES)

-include $(LIB_SPEC_DEPENDENCIES)

-include $(SHADER_DEPENDENCIES)

ifeq ($(OS),Windows_NT)

clear:
	-@del /f $(subst /,\,$(SHADER_DEPENDENCIES) \
		$(OBJECTS) \
		$(DEPENDENCIES) \
		$(LIB_OBJECTS) \
		$(LIB_DEPENDENCIES) \
		$(LIB_SPEC_OBJECTS) \
		$(LIB_SPEC_DEPENDENCIES) \
		$(TEST_OBJECTS) \
		$(TEST_DEPENDENCIES))

$(BIN):
	-@mkdir $(subst /,\,$@)

$(LIB_BIN): $(BIN)
	-@mkdir $(subst /,\,$@)

$(LIB_SPEC_BIN): $(LIB_BIN)
	-@mkdir $(subst /,\,$@)

$(TEST_BIN): $(BIN)
	-@mkdir $(subst /,\,$@)

else ifeq ($(OS),Linux)

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

$(BIN):
	-@mkdir -p $@

$(LIB_BIN): $(BIN)
	-@mkdir -p $@

$(LIB_SPEC_BIN): $(LIB_BIN)
	-@mkdir -p $@

$(TEST_BIN): $(BIN)
	-@mkdir -p $@

else
	$(error Unknown operating system '$(OS)')
endif

update_git:
	@git add .
	@git commit -m "$(commit_name)"
	@git push -f

fetch_git:
ifeq ($(OS),Windows_NT)
	-@del /f $(subst /,\,$(SOURCES) $(SHADER_BINARIES))
else ifeq ($(OS),Linux)
	-@rm -f $(SOURCES) $(SHADER_BINARIES)
else
	$(error Unknown operating system '$(OS)')
endif
	@git fetch --all
	@git reset --hard origin/main
