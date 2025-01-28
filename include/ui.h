// Client User Interface

#ifndef UI_H
#define UI_H

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
 * Displays a message in the UI.
 */
void UIDisplayMessage(UI ui, char *message);

/**
 * Displays an input box in the UI.
 */
void UIDisplayInputBox(UI ui, char *prompt, char *buffer, size_t buffer_size);

#endif
