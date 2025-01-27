# Compiler and flags
CC = clang
CFLAGS = -Wall -Wvla -Werror -Iinclude -fsanitize=address -lncurses

# Directories
SRC_DIR = src
UTIL_DIR = util
TEST_DIR = test
BIN_DIR = bin

# Files
SERVER_FILE = $(SRC_DIR)/server.c
CLIENT_FILE = $(SRC_DIR)/client.c
UTIL_FILES = $(wildcard $(UTIL_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# Targets
SERVER_TARGET = $(BIN_DIR)/server
CLIENT_TARGET = $(BIN_DIR)/client
TEST_TARGETS = $(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/%, $(TEST_FILES))

########################################################################

.PHONY: all clean test

all: $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGETS)

$(SERVER_TARGET): $(SERVER_FILE) $(UTIL_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_TARGET): $(CLIENT_FILE) $(UTIL_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/%: $(TEST_DIR)/%.c $(UTIL_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(UTIL_FILES) -o $@

clean:
	rm -rf $(BIN_DIR)

test: $(TEST_TARGETS)
	@echo "##### Running tests #####"
	@for test_bin in $(TEST_TARGETS); do \
	    echo "Running $$test_bin"; \
	    $$test_bin || exit 1; \
	done
	@echo "##### All tests passed #####"
