/**
 * @file contract_display_port.h
 * @brief Interface display: function pointers untuk rendering UI.
 */

#ifndef SHARED_CONTRACT_DISPLAY_PORT_H
#define SHARED_CONTRACT_DISPLAY_PORT_H

#include "shared/taxonomy_display_constant.h"

/**
 * DisplayPort — interface untuk semua operasi rendering.
 * Surfaces memanggil melalui struct ini, bukan langsung ke ncurses.
 */
typedef struct {
    /* Drawing primitives */
    void (*cls)(void);
    void (*screen_refresh)(void);
    void (*draw_at)(int row, int col, const char *text);
    void (*draw_colored)(int row, int col, int color, int bold, const char *text);
    void (*draw_append)(const char *text);
    void (*draw_append_colored)(int color, int bold, const char *text);

    /* Higher-level drawing */
    void (*print_centered_colored)(int row, const char *text, int color, int bold);
    void (*print_colored)(int row, int col, const char *text, int color);
    void (*box)(int row, int col, int width, int height);
    void (*box_double)(int row, int col, int width, int height);
    void (*separator)(int row, int col, int width);
    void (*separator_thick)(int row, int col, int width);
    void (*highlight_row)(int row, int col, int width, const char *text);
    void (*normal_row)(int row, int col, int width, const char *text);
    void (*footer)(const char *help);
    void (*progress_bar)(int row, int col, int width, int percent, int color);
    const char *(*medal)(int rank);

    /* Input */
    int  (*readkey)(void);
    int  (*confirm)(const char *prompt);
    void (*input_string)(int row, int col, char *buf, int maxlen);

    /* Terminal info */
    int  (*get_lines)(void);
    int  (*get_cols)(void);

} DisplayPort;

#endif /* SHARED_CONTRACT_DISPLAY_PORT_H */
