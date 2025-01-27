// Client User Interface

#ifndef UI_H
#define UI_H

/**
 * Creates a UI.
 */
void UINew(void);

/**
 * Frees a UI.
 */
void UIFree(void);

/**
 * Displays a message in the UI.
 */
void UIDisplayMessage(char *username, char *content);

/**
 * Displays an input box in the UI.
 */
void UIDisplayInputBox(char *prompt, char *buffer, size_t buffer_size);

#endif
