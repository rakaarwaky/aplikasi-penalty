/**
 * @file infrastructure_tui_adapter.h
 * @brief Pembungkus ncurses: inisialisasi layar, splash, fungsi gambar,
 *        dan factory DisplayPort.
 *
 * Berkas ini mengimplementasikan DisplayPort (lihat contract_display_port.h)
 * sekaligus menyediakan fungsi tui_* tingkat rendah yang masih dipakai
 * langsung oleh beberapa lapisan.
 */

#ifndef INFRASTRUCTURE_TUI_ADAPTER_H
#define INFRASTRUCTURE_TUI_ADAPTER_H

#ifdef _WIN32
#include <curses.h>   /* PDCurses di Windows */
#else
#include <ncurses.h>  /* ncurses di Linux */
#endif

#include "shared/contract_display_port.h"

/* Hidupkan & matikan moda ncurses. Dipanggil dari root entry. */
void tui_init(void);
void tui_end(void);

/* Splash screen dengan animasi sederhana. Dipanggil dari root entry. */
void tui_splash(int delay_ms);

/* Factory: buat DisplayPort yang sudah terisi semua function pointer. */
DisplayPort tui_display_port_create(void);

/* ── Fungsi gambar tingkat rendah (juga dipakai langsung) ── */
void tui_clear(void);
void tui_print(int row, int col, const char *text);
void tui_print_centered(int row, const char *text);
void tui_print_colored(int row, int col, const char *text, int color_pair);
void tui_print_centered_colored(int row, const char *text, int color_pair, int bold);
void tui_box(int row, int col, int width, int height);
void tui_box_double(int row, int col, int width, int height);
void tui_separator(int row, int col, int width);
void tui_separator_thick(int row, int col, int width);
void tui_highlight_row(int row, int col, int width, const char *text);
void tui_normal_row(int row, int col, int width, const char *text);
void tui_footer(const char *help);
void tui_progress_bar(int row, int col, int width, int percent, int color);
const char *tui_medal(int rank);

/* ── Input ── */
int tui_getch(void);
int tui_confirm(const char *prompt);

#endif /* INFRASTRUCTURE_TUI_ADAPTER_H */
