#ifndef INFRASTRUCTURE_TUI_ADAPTER_H
#define INFRASTRUCTURE_TUI_ADAPTER_H

/* ncurses initialization and teardown */
void tui_init(void);
void tui_end(void);

/* Screen clearing */
void tui_clear(void);

/* Output functions */
void tui_print(int row, int col, const char *text);
void tui_print_centered(int row, const char *text);
void tui_box(int row, int col, int width, int height);
void tui_highlight_row(int row, int col, int width, const char *text);
void tui_normal_row(int row, int col, int width, const char *text);

/* Color definitions */
#define COLOR_DEFAULT  0
#define COLOR_TITLE    1
#define COLOR_MENU     2
#define COLOR_HIGHLIGHT 3
#define COLOR_SUCCESS  4
#define COLOR_ERROR    5
#define COLOR_BORDER   6

/* Input */
int tui_getch(void);

/* Key constants */
#define TUI_KEY_UP     259
#define TUI_KEY_DOWN   258
#define TUI_KEY_ENTER  10
#define TUI_KEY_ESC    27

#endif /* INFRASTRUCTURE_TUI_ADAPTER_H */
