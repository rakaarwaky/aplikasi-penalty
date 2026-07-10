/**
 * @file infrastructure_tui_adapter.h
 * @brief Antarmuka ncurses: gambar layar & baca tombol (dipakai layar menu).
 */

#ifndef INFRASTRUCTURE_TUI_ADAPTER_H
#define INFRASTRUCTURE_TUI_ADAPTER_H

#ifdef _WIN32
#include <curses.h>   /* PDCurses di Windows */
#else
#include <ncurses.h>  /* ncurses di Linux */
#endif

/* Hidupkan & matikan moda ncurses. */
void tui_init(void);
void tui_end(void);

/* Bersihkan layar. */
void tui_clear(void);

/* Tulis teks. */
void tui_print(int row, int col, const char *text);
void tui_print_centered(int row, const char *text);
void tui_print_colored(int row, int col, const char *text, int color_pair);
void tui_print_centered_colored(int row, const char *text, int color_pair, int bold);
void tui_box(int row, int col, int width, int height);
void tui_box_double(int row, int col, int width, int height);
void tui_highlight_row(int row, int col, int width, const char *text);
void tui_normal_row(int row, int col, int width, const char *text);
void tui_separator(int row, int col, int width);
void tui_separator_thick(int row, int col, int width);

/* Warna (nomor pasangan ncurses; 0 = default). */
#define COLOR_DEFAULT   0
#define COLOR_TITLE     1
#define COLOR_MENU      2
#define COLOR_HIGHLIGHT 3
#define COLOR_SUCCESS   4
#define COLOR_ERROR     5
#define COLOR_BORDER    6
#define COLOR_DIM       7
#define COLOR_GOLD      8
#define COLOR_SILVER    9
#define COLOR_BRONZE    10
#define COLOR_WARNING   11
#define COLOR_INFO      12
#define COLOR_HEADER    13

/* Lebar default progress bar (total karakter termasuk bingkai []). */
#define TUI_BAR_WIDTH 26

/* Baca satu tombol. */
int tui_getch(void);

/* Gambar progress bar [#####-----] 50% di (row,col). persen 0..100. */
void tui_progress_bar(int row, int col, int width, int percent, int color);

/* Footer bantuan konsisten di baris paling bawah layar. */
void tui_footer(const char *help);

/* Kembalikan label peringkat untuk 1,2,3 (string statis). rank lain: spasi. */
const char *tui_medal(int rank);

/* Konfirmasi ya/tidak. Tampilkan prompt, balik 1 bila user tekan y/Y. */
int tui_confirm(const char *prompt);

/* Splash screen dengan animasi sederhana. */
void tui_splash(int delay_ms);

/* Kode tombol (pakai definisi asli ncurses). */
#define TUI_KEY_UP     KEY_UP
#define TUI_KEY_DOWN   KEY_DOWN
#define TUI_KEY_ENTER  10
#define TUI_KEY_ESC    27

#endif /* INFRASTRUCTURE_TUI_ADAPTER_H */
