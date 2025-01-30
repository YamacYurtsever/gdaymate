#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ui.h"
#include "gdmp.h"

struct ui {
    WINDOW *input_win;
    WINDOW *message_win;
    char **messages;
    int message_count;
};

void scroll_messages(UI ui);
void print_messages(UI ui);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

UI UINew(void) {
    initscr();
    UI ui = malloc(sizeof(*ui));

    ui->input_win = NULL;
    ui->message_win = newwin(
        UI_MESSAGE_WIN_HEIGHT, 
        UI_MESSAGE_WIN_WIDTH, 
        UI_MESSAGE_WIN_START_Y, 
        UI_MESSAGE_WIN_START_X
    );

    ui->messages = malloc(UI_MESSAGE_WIN_HEIGHT * sizeof(char *));
    ui->message_count = 0;

    for (int i = 0; i < UI_MESSAGE_WIN_HEIGHT; i++) {
        ui->messages[i] = malloc(GDMP_MESSAGE_MAX_LEN * sizeof(char));
    }

    return ui;
}

void UIFree(UI ui) {
    for (int i = 0; i < UI_MESSAGE_WIN_HEIGHT; i++) {
        free(ui->messages[i]);
    }

    free(ui->messages);
    free(ui);

    endwin();
}

void UIDisplayMessage(UI ui, char *message) {
    // Scroll messages (if overflows)
    if (ui->message_count >= UI_MESSAGE_WIN_HEIGHT) {
        ui->message_count = UI_MESSAGE_WIN_HEIGHT - 1;
        scroll_messages(ui);
    }

    // Copy message to messages
    strncpy(ui->messages[ui->message_count], message, GDMP_MESSAGE_MAX_LEN - 1);
    ui->messages[ui->message_count][GDMP_MESSAGE_MAX_LEN - 1] = '\0';
    ui->message_count++;

    // Print messages
    print_messages(ui);
}

void UIDisplayInput(UI ui, char *prompt, char *input, size_t input_size) {
    // Initialize a new window
    if (ui->input_win == NULL) {
        ui->input_win = newwin(
            UI_INPUT_WIN_HEIGHT, 
            UI_INPUT_WIN_WIDTH, 
            UI_INPUT_WIN_START_Y, 
            UI_INPUT_WIN_START_X
        );
    }

    // Draw input box
    werase(ui->input_win);
    box(ui->input_win, 0, 0);

    // Display prompt
    mvwprintw(ui->input_win, 1, 1, "%s", prompt);
    wrefresh(ui->input_win);

    // Capture input
    wgetnstr(ui->input_win, input, input_size - 1);
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Scrolls all messages, shifting them up in the messages array.
 */
void scroll_messages(UI ui) {
    for (int i = 0; i < UI_MESSAGE_WIN_HEIGHT - 1; i++) {
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
