/**
 * @file infrastructure_tui_adapter.c
 * @brief Infrastructure: pembungkus ncurses untuk semua I/O layar (AES404-style wrapper).
 *
 * Semua fungsi di sini murni pembungkus (wrapper) panggilan ncurses;
 * ia tidak memegang logika bisnis — hanya gambar/beri input. Dipakai
 * oleh layer surfaces (cli) agar surfaces tetap bebas dari detil ncurses.
 */

#include "infrastructure_tui_adapter.h"

#ifdef _WIN32
#include <curses.h>   /* PDCurses di Windows */
#else
#include <ncurses.h>  /* ncurses di Linux */
#endif
#include <string.h>

/* ──────────────────────────────────────────────
 * Inisialisasi ncurses: raw mode (cbreak), no echo,
 * keypad aktif, cursor disembunyikan, lalu daftarkan
 * 6 pasang warna (pair) bila terminal mendukung.
 * ────────────────────────────────────────────── */
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

/* Bebaskan layar ncurses (wajib sebelum keluar). */
void tui_end(void) {
    endwin();
}

/* Bersihkan seluruh layar. */
void tui_clear(void) {
    clear();
}

/* Cetak teks di (row, col) — aman bila text NULL. */
void tui_print(int row, int col, const char *text) {
    if (text == NULL) return;
    mvprintw(row, col, "%s", text);
}

/* Cetak teks di tengah baris row (berdasar lebar layar COLS). */
void tui_print_centered(int row, const char *text) {
    if (text == NULL) return;
    int len = (int)strlen(text);
    int col = (COLS - len) / 2;
    if (col < 0) col = 0;
    mvprintw(row, col, "%s", text);
}

/* Gambar kotak berbingkai ACS di (row,col) ukuran width x height. */
void tui_box(int row, int col, int width, int height) {
    if (width < 2 || height < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_BORDER));

    /* Bingkai atas. */
    mvaddch(row, col, ACS_ULCORNER);
    for (i = 1; i < width - 1; i++) mvaddch(row, col + i, ACS_HLINE);
    mvaddch(row, col + width - 1, ACS_URCORNER);

    /* Bingkai sisi kiri/kanan. */
    for (i = 1; i < height - 1; i++) {
        mvaddch(row + i, col, ACS_VLINE);
        mvaddch(row + i, col + width - 1, ACS_VLINE);
    }

    /* Bingkai bawah. */
    mvaddch(row + height - 1, col, ACS_LLCORNER);
    for (i = 1; i < width - 1; i++)
        mvaddch(row + height - 1, col + i, ACS_HLINE);
    mvaddch(row + height - 1, col + width - 1, ACS_LRCORNER);

    attroff(COLOR_PAIR(COLOR_BORDER));
}

/* Baris terpilih: latar cyan + teks (highlight). */
void tui_highlight_row(int row, int col, int width, const char *text) {
    if (text == NULL || width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    for (i = 0; i < width - 2; i++) mvaddch(row, col + 1 + i, ' ');
    mvprintw(row, col + 2, "%s", text);
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
}

/* Baris biasa: latar default + teks (menu). */
void tui_normal_row(int row, int col, int width, const char *text) {
    if (text == NULL || width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_MENU));
    for (i = 0; i < width - 2; i++) mvaddch(row, col + 1 + i, ' ');
    mvprintw(row, col + 2, "%s", text);
    attroff(COLOR_PAIR(COLOR_MENU));
}

/* Baca satu tombol dari pengguna (ncurses getch). */
int tui_getch(void) {
    return getch();
}
