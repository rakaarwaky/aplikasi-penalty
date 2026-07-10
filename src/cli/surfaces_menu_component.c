/**
 * @file surfaces_menu_component.c
 * @brief DUMB surface — implementasi komponen UI murni layar menu.
 *
 * Semua fungsi di sini hanya MENGGAMBAR potongan lokal. Tidak ada if/else
 * domain, tidak tahu CompetitionStateKind, tidak tahu nomor fitur. Command
 * (smart) yang menyiapkan teks + warna final, lalu page yang menyusun posisi.
 * Komponen TIDAK menggambar separator/box besar — itu urusan page (layout).
 */

#include "cli/surfaces_menu_component.h"

#include <stdio.h>
#include <string.h>

void menu_component_header(DisplayPort *dp, const char *title) {
    dp->print_centered_colored(0, UTF_DOUBLE_H_32, COLOR_DIM, 0);
    dp->print_centered_colored(1, title, COLOR_TITLE, 1);
    dp->print_centered_colored(2, UTF_DOUBLE_H_32, COLOR_DIM, 0);
}

void menu_component_column_headers(DisplayPort *dp, int row,
                                   int col1_x, int col2_x) {
    dp->draw_colored(row, col1_x, COLOR_INFO, 1, "MENU");
    dp->draw_colored(row, col2_x, COLOR_INFO, 1, "STATUS");
}

void menu_component_row(DisplayPort *dp, int row, int col1_x, int col2_x,
                        int col2_width, int box_col, int box_width,
                        const MenuRowView *view) {
    int item_color = view->selected ? COLOR_HIGHLIGHT : COLOR_MENU;

    if (view->selected) {
        int j;
        for (j = 1; j < box_width - 1; j++)
            dp->draw_at(row, box_col + j, " ");
    }

    dp->draw_colored(row, col1_x, item_color, view->selected, view->label);

    char buf[32];
    snprintf(buf, sizeof buf, "%-*s", col2_width, view->status);
    dp->draw_colored(row, col2_x, view->status_color, 0, buf);
}

void menu_component_info(DisplayPort *dp, int row, int box_col,
                         const char *state_text, int state_color,
                         const char *participant_text) {
    dp->draw_colored(row, box_col + 2, state_color, 0, state_text);
    dp->draw_colored(row, box_col + 24, COLOR_MENU, 0, participant_text);
}

void menu_component_exit_button(DisplayPort *dp, int row, int box_col,
                                int box_width, int selected) {
    int exit_color = selected ? COLOR_HIGHLIGHT : COLOR_ERROR;
    dp->draw_colored(row, box_col + box_width - 18, exit_color, 1, "  [0] Keluar  ");
}

void menu_component_footer(DisplayPort *dp) {
    dp->footer("[↑/↓] Navigasi  │  [ENTER] Pilih  │  [1-5] Shortcut  │  [h] Bantuan");
}

/* ── Bantuan ── */
void menu_component_help_rules(DisplayPort *dp, int box_col,
                               const char *rules[], int count) {
    dp->draw_colored(5, box_col + 2, COLOR_GOLD, 1, "ATURAN LOMBA:");
    int ri;
    for (ri = 0; ri < count; ri++)
        dp->draw_colored(6 + ri, box_col + 4, COLOR_MENU, 0, rules[ri]);
}

void menu_component_help_nav(DisplayPort *dp, int box_col,
                             const char *navs[], int count) {
    dp->draw_colored(12, box_col + 2, COLOR_GOLD, 1, "NAVIGASI:");
    int ni;
    for (ni = 0; ni < count; ni++)
        dp->draw_colored(13 + ni, box_col + 4, COLOR_MENU, 0, navs[ni]);
}
