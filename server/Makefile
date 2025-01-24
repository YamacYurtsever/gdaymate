CC = clang
CFLAGS = -Wall -Wvla -Werror -Iinclude

MAIN_FILE = src/server.c
SUPPORTING_FILES = src/ThreadPool.c src/TaskQueue.c src/Task.c

TARGET = server

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
	$(CC) $(CFLAGS) $(MAIN_FILE) $(SUPPORTING_FILES) -o $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET)
