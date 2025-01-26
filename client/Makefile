# Compiler and flags
CC = clang
CFLAGS = -Wall -Wvla -Werror -Iinclude

# Directories
SRC_DIR = src
BIN_DIR = bin

# Files
MAIN_FILE = $(SRC_DIR)/client.c

# Targets
MAIN_TARGET = $(BIN_DIR)/client

########################################################################

.PHONY: all clean test

all: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_FILE) $(SUPPORTING_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BIN_DIR)
