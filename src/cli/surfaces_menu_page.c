/**
 * @file surfaces_menu_page.c
 * @brief DUMB surface — implementasi layout (page) layar menu.
 *
 * Page hanya mengatur DI MANA komponen digambar (layout). Semua teks/warna
 * sudah final di view model — tidak ada branching domain di sini.
 */

#include "cli/surfaces_menu_page.h"

#include <stdio.h>
#include <string.h>

#define BOX_WIDTH     60
#define BOX_START_ROW 3
#define BOX_START_COL 2
#define GRID_HEIGHT   14
#define COL1_X        (BOX_START_COL + 3)
#define COL2_X        (BOX_START_COL + BOX_WIDTH - 16)
#define COL2_WIDTH    14
#define ROW_BASE      (BOX_START_ROW + 3)   /* baris pertama item menu */

void menu_page_render(DisplayPort *dp, const MenuPageView *vm) {
    char buf[128];
    dp->cls();
    (void)buf;

    menu_component_header(dp, "    APLIKASI PERHITUNGAN PENALTI    ");

    /* Bingkai + pemisah bawah header (layout page). */
    dp->box(BOX_START_ROW, BOX_START_COL, BOX_WIDTH, GRID_HEIGHT);
    dp->separator(BOX_START_ROW + 1, BOX_START_COL, BOX_WIDTH);

    int header_row = BOX_START_ROW + 2;
    menu_component_column_headers(dp, header_row, COL1_X, COL2_X);
    dp->separator(header_row + 1, BOX_START_COL, BOX_WIDTH);

    /* 6 baris menu (0..5). */
    int i;
    for (i = 0; i < MENU_PAGE_MAX_ROWS; i++) {
        int row = ROW_BASE + i;
        menu_component_row(dp, row, COL1_X, COL2_X, COL2_WIDTH,
                           BOX_START_COL, BOX_WIDTH, &vm->rows[i]);
    }

    int info_row = BOX_START_ROW + 9;
    dp->separator(info_row, BOX_START_COL, BOX_WIDTH);

    snprintf(buf, sizeof buf, "Peserta: %d/%d",
             vm->participant_count, vm->participant_max);
    menu_component_info(dp, info_row + 1, BOX_START_COL,
                        vm->state_text, vm->state_color, buf);

    menu_component_exit_button(dp, info_row + 1, BOX_START_COL,
                               BOX_WIDTH, vm->selected == 0);

    menu_component_footer(dp);
    dp->screen_refresh();
}

void menu_page_render_help(DisplayPort *dp, const char *rules[], int rule_count,
                           const char *navs[], int nav_count) {
    const int box_col = 2;
    const int box_width = BOX_WIDTH;

    dp->cls();
    menu_component_header(dp, "  PANDUAN PENGGUNAAN  ");

    dp->box(3, box_col, box_width, 18);
    dp->separator(4, box_col, box_width);

    menu_component_help_rules(dp, box_col, rules, rule_count);
    dp->separator(11, box_col, box_width);
    menu_component_help_nav(dp, box_col, navs, nav_count);
    dp->separator(18, box_col, box_width);

    menu_component_footer(dp);
    dp->screen_refresh();
}
