CC = clang
CFLAGS = -Wall -Wvla -Werror -Iinclude

BIN_DIR = bin
TEST_DIR = test

MAIN_FILE = src/server.c
SUPPORTING_FILES = src/ThreadPool.c src/TaskQueue.c src/Task.c
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

MAIN_TARGET = $(BIN_DIR)/server
TEST_TARGETS = $(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/%, $(TEST_FILES))

########################################################################

.PHONY: asan msan nosan

asan: CFLAGS += -fsanitize=address,undefined
asan: all

msan: CFLAGS += -fsanitize=memory,undefined -fsanitize-memory-track-origins
msan: all

nosan: all

########################################################################

.PHONY: all
all: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_FILE) $(SUPPORTING_FILES)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: test
test: $(TEST_TARGETS)

$(BIN_DIR)/%: test/%.c $(SUPPORTING_FILES)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(SUPPORTING_FILES) -o $@

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)
