/**
 * @file surfaces_storage_page.c
 * @brief Dumb surface: gambar layar pengelolaan penyimpanan (Storage).
 */

#include "cli/module.cli.h"
#include "storage/module.storage.h"

#include <stdio.h>

#ifndef STORAGE_DEFAULT_FILE
#define STORAGE_DEFAULT_FILE "data_lomba.bin"
#endif

static void draw_double_line(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++)
        dp->draw_at(row, col + i, "\xe2\x95\x90");
}

/**
 * Gambar layar Storage: judul, daftar menu, status file, dan pesan hasil.
 *
 * @param dp       Port tampilan yang dipakai.
 * @param selected Item yang sedang disorot (1..4).
 * @param file_exists 1 bila file data_lomba.bin ada di disk.
 * @param msg      Pesan hasil operasi terakhir (atau "" bila kosong).
 * @param msg_is_error 1 bila msg adalah pesan error (warna merah).
 */
void storage_page_draw(DisplayPort *dp, int selected, int file_exists,
                       const char *msg, int msg_is_error) {
    char buf[128];
    dp->cls();

    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > 60) gw = 60;
    if (gw < 40) gw = 40;
    int box_col = (cols - gw) / 2;
    int box_row = 3;

    /* Header full-width === */
    draw_double_line(dp, 0, 2, cols - 4);
    dp->print_centered_colored(1, "  PENYIMPANAN DATA LOMBA  ", COLOR_TITLE, 1);
    draw_double_line(dp, 2, 2, cols - 4);

    dp->box(box_row, box_col, gw, 13);
    dp->separator(box_row + 1, box_col, gw);

    dp->draw_colored(box_row + 2, box_col + 3, COLOR_INFO, 1, "MENU PENYIMPANAN");
    dp->separator(box_row + 3, box_col, gw);

    const char *items[4] = {
        "Simpan Lomba ke File",
        "Muat Lomba dari File",
        "Hapus File Tersimpan",
        "Reset Lomba (bersihkan memory)"
    };
    int i;
    for (i = 0; i < 4; i++) {
        int row = box_row + 4 + i;
        int item_sel = (i + 1 == selected);
        int item_color = item_sel ? COLOR_HIGHLIGHT : COLOR_MENU;
        if (item_sel) {
            int j;
            for (j = 1; j < gw - 1; j++)
                dp->draw_at(row, box_col + j, " ");
        }
        snprintf(buf, sizeof buf, "  [%d]  %s", i + 1, items[i]);
        dp->draw_colored(row, box_col + 3, item_color, item_sel, buf);
    }

    dp->separator(box_row + 9, box_col, gw);

    snprintf(buf, sizeof buf, "File: %s", STORAGE_DEFAULT_FILE);
    dp->draw_colored(box_row + 10, box_col + 2, COLOR_MENU, 0, buf);

    const char *fstate = file_exists ? "[Ada]" : "[Tidak ada]";
    int fcolor = file_exists ? COLOR_SUCCESS : COLOR_DIM;
    dp->draw_colored(box_row + 10, box_col + gw - 16, fcolor, 0, fstate);

    /* Pesan hasil operasi di baris bawah box. */
    if (msg != NULL && msg[0] != '\0') {
        int mcolor = msg_is_error ? COLOR_ERROR : COLOR_SUCCESS;
        dp->draw_colored(box_row + 11, box_col + 2, mcolor, 1, msg);
    }

    dp->footer("[↑/↓] Navigasi  │  [ENTER] Pilih  │  [ESC] Kembali ke menu");
    dp->screen_refresh();
}
