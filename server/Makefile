# Compiler and flags
CC = clang
CFLAGS = -Wall -Wvla -Werror -Iinclude

# Directories
SRC_DIR = src
TEST_DIR = test
BIN_DIR = bin

# Files
MAIN_FILE = $(SRC_DIR)/server.c
SUPPORTING_FILES = $(SRC_DIR)/ThreadPool.c $(SRC_DIR)/TaskQueue.c $(SRC_DIR)/Task.c
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# Targets
MAIN_TARGET = $(BIN_DIR)/server
TEST_TARGETS = $(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/%, $(TEST_FILES))

########################################################################

.PHONY: all clean test

all: $(MAIN_TARGET) $(TEST_TARGETS)

$(MAIN_TARGET): $(MAIN_FILE) $(SUPPORTING_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/%: $(TEST_DIR)/%.c $(SUPPORTING_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(SUPPORTING_FILES) -o $@

clean:
	rm -rf $(BIN_DIR)

test: $(TEST_TARGETS)
	@echo "##### Running tests #####"
	@for test_bin in $(TEST_TARGETS); do \
	    echo "Running $$test_bin"; \
	    $$test_bin || exit 1; \
	done
	@echo "##### All tests passed #####"
