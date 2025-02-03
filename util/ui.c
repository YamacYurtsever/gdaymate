#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ui.h"
#include "gdmp.h"

#define MESSAGE_MAX_COUNT (UI_MESSAGE_WIN_HEIGHT - 2)

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

    ui->input_win = newwin(
        UI_INPUT_WIN_HEIGHT, 
        UI_INPUT_WIN_WIDTH, 
        UI_INPUT_WIN_START_Y, 
        UI_INPUT_WIN_START_X
    );

    ui->message_win = newwin(
        UI_MESSAGE_WIN_HEIGHT, 
        UI_MESSAGE_WIN_WIDTH, 
        UI_MESSAGE_WIN_START_Y, 
        UI_MESSAGE_WIN_START_X
    );

    ui->messages = malloc(MESSAGE_MAX_COUNT * sizeof(char *));
    ui->message_count = 0;

    for (int i = 0; i < MESSAGE_MAX_COUNT; i++) {
        ui->messages[i] = malloc(GDMP_MESSAGE_MAX_LEN * sizeof(char));
    }

    box(ui->input_win, 0, 0);
    wrefresh(ui->input_win);

    box(ui->message_win, 0, 0);
    wrefresh(ui->message_win);

    return ui;
}

void UIFree(UI ui) {
    for (int i = 0; i < MESSAGE_MAX_COUNT; i++) {
        free(ui->messages[i]);
    }

    free(ui->messages);
    free(ui);

    endwin();
}

void UIDisplayMessage(UI ui, char *message) {
    // Scroll messages (if overflows)
    if (ui->message_count >= MESSAGE_MAX_COUNT) {
        ui->message_count = MESSAGE_MAX_COUNT - 1;
        scroll_messages(ui);
    }

    // Add message to messages
    strncpy(ui->messages[ui->message_count], message, GDMP_MESSAGE_MAX_LEN - 1);
    ui->messages[ui->message_count][GDMP_MESSAGE_MAX_LEN - 1] = '\0';
    ui->message_count++;

    // Print messages
    print_messages(ui);
}

void UIDisplayInput(UI ui, char *prompt, char *input, size_t input_size) {
    // Print prompt
    werase(ui->input_win);
    box(ui->input_win, 0, 0);
    mvwprintw(ui->input_win, 1, 1, "%s", prompt);
    wrefresh(ui->input_win);

    // Capture input
    wgetnstr(ui->input_win, input, input_size - 1); 
}

void UIClearMessages(UI ui) {
    // Clear messages
    for (int i = 0; i < MESSAGE_MAX_COUNT; i++) {
        ui->messages[i][i] = '\0';
    }
    ui->message_count = 0;

    // Print messages
    print_messages(ui);
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Scrolls all messages, shifting them up in the messages array.
 */
void scroll_messages(UI ui) {
    for (int i = 0; i < MESSAGE_MAX_COUNT - 1; i++) {
        strncpy(ui->messages[i], ui->messages[i + 1], GDMP_MESSAGE_MAX_LEN - 1);
        ui->messages[i][GDMP_MESSAGE_MAX_LEN - 1] = '\0';
    }
}

/**
 * Prints all messages.
 */
void print_messages(UI ui) {
    werase(ui->message_win);
    box(ui->message_win, 0, 0);
    for (int i = 0; i < ui->message_count; i++) {
        mvwprintw(ui->message_win, 1 + i, 1, "%s", ui->messages[i]);
    }
    wrefresh(ui->message_win);
}
