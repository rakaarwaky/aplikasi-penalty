/**
 * @file contract_display_port.h
 * @brief Port antarmuka layar — struct function pointer untuk semua operasi
 *        menggambar ke terminal.
 *
 * Infrastructure (infrastructure_tui_adapter) mengimplementasikan port ini.
 * Surfaces layer hanya memegang pointer ke struct ini dan memanggil
 * fungsi melaluinya — tidak pernah menyentuh ncurses secara langsung.
 */

#ifndef SHARED_CONTRACT_DISPLAY_PORT_H
#define SHARED_CONTRACT_DISPLAY_PORT_H

#include "shared/taxonomy_display_constant.h"

/**
 * Kumpulan operasi menggambar ke terminal.
 * Diisi oleh root container dari implementasi TUI konkret.
 */
typedef struct {

    /* ── Lifecycle & Refresh ── */
    void (*cls)(void);              /**< Bersihkan seluruh layar. */
    void (*screen_refresh)(void);     /**< Kirim buffer ke terminal (refresh). */

    /* ── Primitif Draw (pengganti attron+mvprintw+attroff) ── */

    /** Tulis teks polos di posisi (row, col). */
    void (*draw_at)(int row, int col, const char *text);

    /**
     * Tulis teks berwarna di posisi (row, col).
     * @param color  Salah satu COLOR_* dari taxonomy_display_constant.h
     * @param bold   1 = tebal, 0 = normal
     */
    void (*draw_colored)(int row, int col, int color, int bold,
                         const char *text);

    /** Lanjutkan tulis teks dari posisi kursor saat ini (printw). */
    void (*draw_append)(const char *text);

    /**
     * Lanjutkan tulis teks berwarna dari posisi kursor saat ini
     * (attron + printw + attroff).
     */
    void (*draw_append_colored)(int color, int bold, const char *text);

    /* ── Komponen TUI Tingkat Tinggi ── */

    void (*print_centered_colored)(int row, const char *text,
                                   int color, int bold);
    void (*print_colored)(int row, int col, const char *text, int color);
    void (*box)(int row, int col, int width, int height);
    void (*box_double)(int row, int col, int width, int height);
    void (*separator)(int row, int col, int width);
    void (*separator_thick)(int row, int col, int width);
    void (*highlight_row)(int row, int col, int width, const char *text);
    void (*normal_row)(int row, int col, int width, const char *text);
    void (*footer)(const char *help);
    void (*progress_bar)(int row, int col, int width, int percent, int color);
    const char *(*medal)(int rank);   /**< Label medali "🥇/🥈/🥉" atau spasi. */

    /* ── Input ── */
    int  (*readkey)(void);            /**< Baca satu tombol; kembalikan kode. */
    int  (*confirm)(const char *prompt); /**< Dialog ya/tidak; 1 = ya. */

} DisplayPort;

#endif /* SHARED_CONTRACT_DISPLAY_PORT_H */
