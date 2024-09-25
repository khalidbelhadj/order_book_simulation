# Define variables
CC = clang++
CFLAGS = -Wall -Wextra -pedantic -std=c++17
DEBUG_FLAGS = $(CFLAGS) -g
RELEASE_FLAGS = $(CFLAGS) -O3 -ffast-math
VISUAL_FLAGS = $(CFLAGS) -I./lib/raylib/src/ -lraylib -L./lib/raylib/src/ -L/opt/homebrew/lib/ -lm -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -DVISUAL

SRC_DIR = src
BUILD_DIR = build
DEBUG_DIR = $(BUILD_DIR)/debug
RELEASE_DIR = $(BUILD_DIR)/release
VISUAL_DIR = $(BUILD_DIR)/visual

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
DEBUG_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(DEBUG_DIR)/%.o, $(SRC_FILES))
RELEASE_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(RELEASE_DIR)/%.o, $(SRC_FILES))
VISUAL_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(VISUAL_DIR)/%.o, $(SRC_FILES))

# Default target
# run all targets
all: debug release visual

# Debug build
debug: $(DEBUG_DIR)/main

$(DEBUG_DIR)/main: $(DEBUG_OBJS)
	$(CC) $(DEBUG_FLAGS) -o $@ $^

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(DEBUG_DIR)
	$(CC) $(DEBUG_FLAGS) -c -o $@ $<

# Release build
release: $(RELEASE_DIR)/main

$(RELEASE_DIR)/main: $(RELEASE_OBJS)
	$(CC) $(RELEASE_FLAGS) -o $@ $^

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(RELEASE_DIR)
	$(CC) $(RELEASE_FLAGS) -c -o $@ $<

# Visual build
visual: $(VISUAL_DIR)/main

$(VISUAL_DIR)/main: $(VISUAL_OBJS)
	$(CC) $(VISUAL_FLAGS) -o $@ $^

$(VISUAL_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(VISUAL_DIR)
	$(CC) $(VISUAL_FLAGS) -c -o $@ $<

# Clean build directories
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all debug release visual clean