CC = clang
CFLAGS = -Wall -Wvla -Werror -Iinclude

MAIN_FILE = src/server.c
SUPPORTING_FILES = src/ThreadPool.c src/TaskQueue.c src/Task.c
TEST_FILES = tests/testTask.c

MAIN_TARGET = server
TEST_TARGETS = $(TEST_FILES:.c=)

########################################################################

.PHONY: asan msan nosan

asan: CFLAGS += -fsanitize=address,undefined
asan: all

msan: CFLAGS += -fsanitize=memory,undefined -fsanitize-memory-track-origins
msan: all

nosan: all

########################################################################

.PHONY: all
all:
	$(CC) $(CFLAGS) $(MAIN_FILE) $(SUPPORTING_FILES) -o $(MAIN_TARGET)

.PHONY: test
test: $(TEST_TARGETS)

$(TEST_TARGETS): %: %.c $(SUPPORTING_FILES)
	$(CC) $(CFLAGS) $< $(SUPPORTING_FILES) -o $@

.PHONY: clean
clean:
	rm $(MAIN_TARGET) $(TEST_TARGETS)
