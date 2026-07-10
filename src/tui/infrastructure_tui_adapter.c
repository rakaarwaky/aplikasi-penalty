#include "infrastructure_tui_adapter.h"

#ifdef _WIN32
#include <curses.h>   /* PDCurses di Windows */
#else
#include <ncurses.h>  /* ncurses di Linux */
#endif
#include <string.h>

void tui_init(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(COLOR_TITLE,    COLOR_CYAN,    COLOR_BLACK);
        init_pair(COLOR_MENU,     COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_HIGHLIGHT, COLOR_BLACK,  COLOR_CYAN);
        init_pair(COLOR_SUCCESS,  COLOR_GREEN,   COLOR_BLACK);
        init_pair(COLOR_ERROR,    COLOR_RED,     COLOR_BLACK);
        init_pair(COLOR_BORDER,   COLOR_CYAN,    COLOR_BLACK);
    }
}

void tui_end(void) {
    endwin();
}

void tui_clear(void) {
    clear();
}

void tui_print(int row, int col, const char *text) {
    if (text == NULL) return;
    mvprintw(row, col, "%s", text);
}

void tui_print_centered(int row, const char *text) {
    if (text == NULL) return;
    int len = (int)strlen(text);
    int col = (COLS - len) / 2;
    if (col < 0) col = 0;
    mvprintw(row, col, "%s", text);
}

void tui_box(int row, int col, int width, int height) {
    if (width < 2 || height < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_BORDER));

    /* Top border */
    mvaddch(row, col, ACS_ULCORNER);
    for (i = 1; i < width - 1; i++) mvaddch(row, col + i, ACS_HLINE);
    mvaddch(row, col + width - 1, ACS_URCORNER);

    /* Side borders */
    for (i = 1; i < height - 1; i++) {
        mvaddch(row + i, col, ACS_VLINE);
        mvaddch(row + i, col + width - 1, ACS_VLINE);
    }

    /* Bottom border */
    mvaddch(row + height - 1, col, ACS_LLCORNER);
    for (i = 1; i < width - 1; i++)
        mvaddch(row + height - 1, col + i, ACS_HLINE);
    mvaddch(row + height - 1, col + width - 1, ACS_LRCORNER);

    attroff(COLOR_PAIR(COLOR_BORDER));
}

void tui_highlight_row(int row, int col, int width, const char *text) {
    if (text == NULL || width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    for (i = 0; i < width - 2; i++) mvaddch(row, col + 1 + i, ' ');
    mvprintw(row, col + 2, "%s", text);
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
}

void tui_normal_row(int row, int col, int width, const char *text) {
    if (text == NULL || width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_MENU));
    for (i = 0; i < width - 2; i++) mvaddch(row, col + 1 + i, ' ');
    mvprintw(row, col + 2, "%s", text);
    attroff(COLOR_PAIR(COLOR_MENU));
}

int tui_getch(void) {
    return getch();
}
