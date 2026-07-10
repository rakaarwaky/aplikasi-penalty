/**
 * @file surfaces_storage_page.c
 * @brief Dumb surface: menggambar layar menu Simpan/Muat data lomba.
 */

/* CLI — Dumb Surface (Page) */
#include "cli/module.cli.h"
#include "shared/taxonomy_display_constant.h"
#include "shared/taxonomy_game_constant.h"

#include <stdio.h>

/* Baris tempat daftar opsi mulai (setelah header box). */
static const int OPT_ROW_START = 5;

static void draw_double_line(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++)
        dp->draw_at(row, col + i, "\xe2\x95\x90");
}

/* Gambar layar penyimpanan.
 * selected: indeks opsi terpilih (1..4). show_file: ada/tidak file tersimpan.
 * msg/msg_color: baris status di bawah opsi (pesan hasil aksi terakhir). */
void storage_page_draw(DisplayPort *dp, int selected, int show_file,
                       const char *msg, int msg_color) {
    char buf[128];
    dp->cls();

    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > BOX_WIDTH_MAX) gw = BOX_WIDTH_MAX;
    if (gw < BOX_WIDTH_MIN) gw = BOX_WIDTH_MIN;
    int box_col = (cols - gw) / 2;
    int box_row = 3;

    /* Header full-width seperti menu utama. */
    draw_double_line(dp, 0, 2, cols - 4);
    dp->print_centered_colored(1, "  SIMPAN / MUAT DATA LOMBA  ", COLOR_TITLE, 1);
    draw_double_line(dp, 2, 2, cols - 4);

    dp->box(box_row, box_col, gw, 14);
    dp->separator(box_row + 1, box_col, gw);

    int col1_x = box_col + 3;
    dp->draw_colored(box_row + 2, col1_x, COLOR_INFO, 1, "PILIHAN");
    dp->separator(box_row + 3, box_col, gw);

    const char *labels[5] = {
        "",
        "[1] Simpan Data Lomba",
        "[2] Muat Data Lomba",
        "[3] Hapus File Tersimpan",
        "[4] Reset Lomba (bersihkan memori)"
    };

    int i;
    for (i = 1; i <= 4; i++) {
        int row = box_row + OPT_ROW_START - 1 + i;
        int item_sel = (i == selected);
        int item_color = item_sel ? COLOR_HIGHLIGHT : COLOR_MENU;
        if (item_sel) {
            int j;
            for (j = 1; j < gw - 1; j++)
                dp->draw_at(row, box_col + j, " ");
        }
        dp->draw_colored(row, col1_x, item_color, item_sel, labels[i]);
    }

    dp->separator(box_row + 10, box_col, gw);

    /* Info status file tersimpan. */
    snprintf(buf, sizeof buf, "File: %s", DEFAULT_STORAGE_FILENAME);
    dp->draw_colored(box_row + 11, box_col + 2, COLOR_DIM, 0, buf);
    dp->draw_colored(box_row + 11, box_col + gw - 16,
                     show_file ? COLOR_SUCCESS : COLOR_DIM, 0,
                     show_file ? "[Ada]" : "[Kosong]");

    /* Baris pesan hasil aksi. */
    if (msg != NULL && msg[0] != '\0') {
        dp->draw_colored(box_row + 12, box_col + 2, msg_color, 0, msg);
    }

    dp->footer("[\xe2\x86\x91/\xe2\x86\x93] Navigasi  \xe2\x94\x82  [ENTER] Pilih  \xe2\x94\x82  [0/q] Kembali ke menu");
    dp->screen_refresh();
}
