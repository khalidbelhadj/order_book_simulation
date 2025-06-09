# Base
CXX = g++
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

# Default target
# run all targets
all: debug release

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

fmt:
	clang-format -i --sort-includes ./src/*.cpp ./src/*.h

# Clean build directories
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all debug release clean fmt