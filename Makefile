# Compiler and flags
CC = clang
CFLAGS = -Wall -Wvla -Werror -Iinclude -Imulti/include -fsanitize=address -lncurses

# Directories
SRC_DIR = src
MULTI_DIR = multi
UTIL_DIR = util
TEST_DIR = test
BIN_DIR = bin

# Files
SERVER_FILES = $(SRC_DIR)/run_server.c $(SRC_DIR)/server.c $(SRC_DIR)/server_process.c
CLIENT_FILES = $(SRC_DIR)/run_client.c $(SRC_DIR)/client.c
UTIL_FILES = $(wildcard $(UTIL_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# Targets
SERVER_TARGET = $(BIN_DIR)/run_server
CLIENT_TARGET = $(BIN_DIR)/run_client
TEST_TARGETS = $(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/%, $(TEST_FILES))

########################################################################

.PHONY: all clean test

all: $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGETS) $(MULTI_SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_FILES) $(UTIL_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_TARGET): $(CLIENT_FILES) $(UTIL_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/%: $(TEST_DIR)/%.c $(UTIL_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(UTIL_FILES) -o $@

clean:
	rm -rf $(BIN_DIR)

test: $(TEST_TARGETS)
	@echo "Running tests..."
	@for test_bin in $(TEST_TARGETS); do \
	    echo "Running $$test_bin"; \
	    $$test_bin || exit 1; \
	done
	@echo "All tests passed"
