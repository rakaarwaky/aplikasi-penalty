/**
 * @file taxonomy_display_constant.h
 * @brief Konstanta abstrak untuk warna layar dan kode tombol navigasi.
 *
 * Surfaces layer hanya boleh include file ini untuk mendapat nilai
 * warna dan tombol — tanpa menyentuh ncurses secara langsung.
 */

#ifndef SHARED_TAXONOMY_DISPLAY_CONSTANT_H
#define SHARED_TAXONOMY_DISPLAY_CONSTANT_H

/* ── Pasangan Warna (color-pair IDs, cocok dengan urutan start_color) ── */
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

/* ── Lebar default progress bar ── */
#define TUI_BAR_WIDTH   26

/* ── Kode Tombol (nilai integer, tidak bergantung ncurses) ── */
#define TUI_KEY_ENTER   10
#define TUI_KEY_ESC     27
#define TUI_KEY_UP      259   /* KEY_UP di ncurses */
#define TUI_KEY_DOWN    258   /* KEY_DOWN di ncurses */

#endif /* SHARED_TAXONOMY_DISPLAY_CONSTANT_H */
