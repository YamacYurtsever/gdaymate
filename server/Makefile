CC = clang
CFLAGS = -Wall -Wvla -Werror -Iinclude

MAIN_FILE = src/server.c
SUPPORTING_FILES = src/ThreadPool.c src/TaskQueue.c src/Task.c
TEST_FILES = tests/testThreadPool.c tests/testTaskQueue.c tests/testTask.c

BIN_DIR = bin
MAIN_TARGET = $(BIN_DIR)/server
TEST_TARGETS = $(patsubst tests/%.c, $(BIN_DIR)/%, $(TEST_FILES))

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
tests: $(TEST_TARGETS)

$(BIN_DIR)/%: tests/%.c $(SUPPORTING_FILES)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(SUPPORTING_FILES) -o $@

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)
