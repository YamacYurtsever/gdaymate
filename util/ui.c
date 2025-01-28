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

struct ui {
    char **messages;
    int message_count;
    WINDOW *input_win;
};

void scroll_messages(UI ui);
void print_messages(UI ui);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

UI UINew(void) {
    initscr();
    UI ui = malloc(sizeof(*ui));

    ui->messages = malloc(MESSAGE_BOX_HEIGHT * sizeof(char *));
    ui->message_count = 0;
    ui->input_win = NULL;

    for (int i = 0; i < MESSAGE_BOX_HEIGHT; i++) {
        ui->messages[i] = malloc(GDMP_MESSAGE_MAX_LEN * sizeof(char));
    }

    return ui;
}

void UIFree(UI ui) {
    for (int i = 0; i < MESSAGE_BOX_HEIGHT; i++) {
        free(ui->messages[i]);
    }

    free(ui->messages);
    free(ui);

    endwin();
}

void UIDisplayMessage(UI ui, char *username, char *content) {
    // Scroll messages (if overflows)
    if (ui->message_count >= MESSAGE_BOX_HEIGHT) {
        ui->message_count = MESSAGE_BOX_HEIGHT - 1;
        scroll_messages(ui);
    }

    // Form message
    char message[GDMP_MESSAGE_MAX_LEN];
    snprintf(message, GDMP_MESSAGE_MAX_LEN, "%s: %s", username, content);

    // Copy message to messages
    strncpy(ui->messages[ui->message_count], message, GDMP_MESSAGE_MAX_LEN - 1);
    ui->messages[ui->message_count][GDMP_MESSAGE_MAX_LEN - 1] = '\0';
    ui->message_count++;

    // Print messages
    print_messages(ui);
}

void UIDisplayInputBox(UI ui, char *prompt, char *buffer, size_t buffer_size) {
    // Initialize a new window
    if (ui->input_win == NULL) {
        ui->input_win = newwin(
            INPUT_BOX_HEIGHT, 
            INPUT_BOX_WIDTH, 
            INPUT_BOX_START_Y, 
            INPUT_BOX_START_X
        );
    }

    // Draw input box
    werase(ui->input_win);
    box(ui->input_win, 0, 0);

    // Display prompt
    mvwprintw(ui->input_win, 1, 1, "%s", prompt);
    wrefresh(ui->input_win);

    // Capture input (into buffer)
    wgetnstr(ui->input_win, buffer, buffer_size - 1);
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Scrolls all messages, shifting them up in the messages array.
 */
void scroll_messages(UI ui) {
    for (int i = 0; i < MESSAGE_BOX_HEIGHT - 1; i++) {
        strncpy(ui->messages[i], ui->messages[i + 1], GDMP_MESSAGE_MAX_LEN - 1);
        ui->messages[i][GDMP_MESSAGE_MAX_LEN - 1] = '\0';
    }
}

/**
 * Prints all messages.
 */
void print_messages(UI ui) {
    clear();
    for (int i = 0; i < ui->message_count; i++) {
        mvprintw(i, 0, "%s", ui->messages[i]);
    }
    refresh();
}
