#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ui.h"
#include "gdmp.h"

#define INPUT_BOX_HEIGHT 3
#define INPUT_BOX_WIDTH COLS
#define INPUT_BOX_START_Y (LINES - INPUT_BOX_HEIGHT)
#define INPUT_BOX_START_X 0

#define MESSAGE_BOX_HEIGHT (LINES - INPUT_BOX_HEIGHT)

static char **messages = NULL;
static int message_count = 0;
static WINDOW *input_win = NULL;

void scroll_messages(void);
void print_messages(void);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

void UINew(void) {
    initscr();

    // Create messages array
    messages = malloc(MESSAGE_BOX_HEIGHT * sizeof(char *));
    for (int i = 0; i < MESSAGE_BOX_HEIGHT; i++) {
        messages[i] = malloc(GDMP_MESSAGE_MAX_LEN * sizeof(char));
    }
}

void UIFree(void) {
    // Free messages array
    for (int i = 0; i < MESSAGE_BOX_HEIGHT; i++) {
        free(messages[i]);
    }
    free(messages);

    endwin();
}

void UIDisplayMessage(char *username, char *content) {
    // Scroll messages (if overflows)
    if (message_count >= MESSAGE_BOX_HEIGHT) {
        message_count = MESSAGE_BOX_HEIGHT - 1;
        scroll_messages();
    }

    // Form message
    char message[GDMP_MESSAGE_MAX_LEN];
    snprintf(message, GDMP_MESSAGE_MAX_LEN, "%s: %s", username, content);

    // Copy message to messages
    strncpy(messages[message_count], message, GDMP_MESSAGE_MAX_LEN - 1);
    messages[message_count][GDMP_MESSAGE_MAX_LEN - 1] = '\0';
    message_count++;

    // Print messages
    print_messages();
}

void UIDisplayInputBox(char *prompt, char *buffer, size_t buffer_size) {
    // Initialize a new window
    if (!input_win) {
        input_win = newwin(
            INPUT_BOX_HEIGHT, 
            INPUT_BOX_WIDTH, 
            INPUT_BOX_START_Y, 
            INPUT_BOX_START_X
        );
    }

    // Draw input box
    werase(input_win);
    box(input_win, 0, 0);

    // Display prompt
    mvwprintw(input_win, 1, 1, "%s", prompt);
    wrefresh(input_win);

    // Capture input (into buffer)
    wgetnstr(input_win, buffer, buffer_size - 1);
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Scrolls all messages, shifting them up in the messages array.
 */
void scroll_messages(void) {
    for (int i = 0; i < MESSAGE_BOX_HEIGHT - 1; i++) {
        strncpy(messages[i], messages[i + 1], GDMP_MESSAGE_MAX_LEN - 1);
        messages[i][GDMP_MESSAGE_MAX_LEN - 1] = '\0';
    }
}

/**
 * Prints all messages.
 */
void print_messages(void) {
    clear();
    for (int i = 0; i < message_count; i++) {
        mvprintw(i, 0, "%s", messages[i]);
    }
    refresh();
}
