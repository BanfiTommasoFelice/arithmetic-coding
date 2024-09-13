CXX                = gcc
CXXFLAGS_COMMON    = -std=c99 -Wall -Wextra
CXXFLAGS_FAST      = $(CXXFLAGS_COMMON) -O3 -ffast-math
CXXFLAGS_RELEASE   = $(CXXFLAGS_FAST)   --static -DNDEBUG 
CXXFLAGS_DEBUG     = $(CXXFLAGS_COMMON) -O0 -g3 -fsanitize=address,undefined
CXXFLAGS_LINK      =

MODE ?= debug
ifeq ($(MODE), fast)
	CXXFLAGS = $(CXXFLAGS_FAST)
else ifeq ($(MODE), release)
	CXXFLAGS = $(CXXFLAGS_RELEASE)
else
	CXXFLAGS = $(CXXFLAGS_DEBUG)
endif

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
UTL_DIR = $(SRC_DIR)/utils

UTILS   = $(wildcard $(UTL_DIR)/*.c)
HEADERS = $(wildcard $(UTL_DIR)/*.h)
OBJECTS = $(patsubst $(UTL_DIR)/%.c,$(OBJ_DIR)/%.o,$(UTILS))

$(OBJ_DIR)/%.o: $(UTL_DIR)/%.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(OBJ_DIR)/main.o $(CXXFLAGS_LINK) -o $(BIN_DIR)/main

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
