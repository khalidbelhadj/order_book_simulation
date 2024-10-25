# Base
CXX = clang++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
LD_FLAGS =
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
BUILD_DIR = build

# Debug
DEBUG_DIR = $(BUILD_DIR)/debug
DEBUG_FLAGS = $(CXXFLAGS) -g
DEBUG_LD_FLAGS = $(LD_FLAGS)
DEBUG_SRC_FILES = $(SRC_FILES)
DEBUG_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(DEBUG_DIR)/%.o, $(DEBUG_SRC_FILES))

# Release
RELEASE_DIR = $(BUILD_DIR)/release
RELEASE_FLAGS = $(CXXFLAGS) -O3 -ffast-math
RELEASE_LD_FLAGS = $(LD_FLAGS)
RELEASE_SRC_FILES = $(SRC_FILES)
RELEASE_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(RELEASE_DIR)/%.o, $(RELEASE_SRC_FILES))

# Visual
VISUAL_DIR = $(BUILD_DIR)/visual
VISUAL_FLAGS = $(CXXFLAGS) -DVISUAL -g
VISUAL_LD_FLAGS = $(LD_FLAGS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -L/usr/local/lib -L/opt/homebrew/lib -lglfw
VISUAL_SRC_FILES = $(SRC_FILES) $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
VISUAL_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(VISUAL_DIR)/%.o, $(VISUAL_SRC_FILES))
IMGUI_DIR = lib/imgui

# Default target
# run all targets
all: debug release visual

# Debug build
debug: $(DEBUG_DIR)/main

$(DEBUG_DIR)/main: $(DEBUG_OBJS)
	$(CXX) $(DEBUG_FLAGS) $(DEBUG_LD_FLAGS) -o $@ $^

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(DEBUG_DIR)
	$(CXX) $(DEBUG_FLAGS) -c -o $@ $<

# Release build
release: $(RELEASE_DIR)/main

$(RELEASE_DIR)/main: $(RELEASE_OBJS)
	$(CXX) $(RELEASE_FLAGS) $(RELEASE_LD_FLAGS) -o $@ $^

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(RELEASE_FLAGS) -c -o $@ $<

# Visual build
visual: $(VISUAL_DIR)/main

$(VISUAL_DIR)/main: $(VISUAL_OBJS)
	$(CXX) $(VISUAL_FLAGS) $(VISUAL_LD_FLAGS) -o $@ $^

$(VISUAL_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(VISUAL_DIR)
	$(CXX) $(VISUAL_FLAGS) -c -o $@ $<

fmt:
	clang-format -i --sort-includes ./src/*.cpp ./src/*.h

# Clean build directories
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all debug release visual clean fmt