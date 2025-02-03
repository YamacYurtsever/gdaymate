// Client User Interface

#ifndef UI_H
#define UI_H

#define UI_INPUT_WIN_HEIGHT 3
#define UI_INPUT_WIN_WIDTH COLS
#define UI_INPUT_WIN_START_Y (LINES - UI_INPUT_WIN_HEIGHT)
#define UI_INPUT_WIN_START_X 0

#define UI_MESSAGE_WIN_HEIGHT (LINES - UI_INPUT_WIN_HEIGHT)
#define UI_MESSAGE_WIN_WIDTH COLS
#define UI_MESSAGE_WIN_START_Y 0
#define UI_MESSAGE_WIN_START_X 0

typedef struct ui *UI;

/**
 * Creates a UI.
 */
UI UINew(void);

/**
 * Frees a UI.
 */
void UIFree(UI ui);

/**
 * Displays a message in the message window.
 */
void UIDisplayMessage(UI ui, char *message);

/**
 * Displays an input window with the prompt, 
 * stores the inputed value in the buffer.
 */
void UIDisplayInput(UI ui, char *prompt, char *buffer, size_t buffer_size);

/**
 * Clears all messages.
 */
void UIClearMessages(UI ui);

#endif
