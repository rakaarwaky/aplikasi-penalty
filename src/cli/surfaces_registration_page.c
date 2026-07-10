/**
 * @file surfaces_registration_page.c
 * @brief Dumb surface: render layar pendaftaran peserta.
 */

#include "cli/module.cli.h"

#include <stdio.h>

#define BOX_HEIGHT_MIN 13

static void draw_double_line(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++)
        dp->draw_at(row, col + i, "\xe2\x95\x90");
}

void registration_page_draw(DisplayPort *dp, CompetitionState *state) {
    char buf[128];
    dp->cls();

    int cols = dp->get_cols();
    int count = state->participant_count;

    int gw = cols - 4;
    if (gw > 64) gw = 64;
    if (gw < 40) gw = 40;
    int box_col = (cols - gw) / 2;
    int box_row = 4;

    int box_height = count + BOX_HEIGHT_MIN;
    int max_h = dp->get_lines() - 6;
    if (box_height > max_h) box_height = max_h;

    dp->print_centered_colored(0, "Menu Utama > Pendaftaran Peserta", COLOR_DIM, 0);
    draw_double_line(dp, 1, 2, cols - 4);
    dp->print_centered_colored(2, "       PENDAFTARAN PESERTA        ", COLOR_TITLE, 1);
    draw_double_line(dp, 3, 2, cols - 4);

    dp->box(box_row, box_col, gw, box_height);

    int pct = (count * 100) / MAX_PARTICIPANTS;
    snprintf(buf, sizeof buf, "Kuota: %d/%d Terisi", count, MAX_PARTICIPANTS);
    dp->draw_colored(box_row + 2, box_col + 2, COLOR_INFO, 0, buf);
    dp->progress_bar(box_row + 3, box_col + 2, 30, pct, COLOR_SUCCESS);

    dp->separator(box_row + 4, box_col, gw);
    dp->draw_colored(box_row + 5, box_col + 2, COLOR_DIM, 0,
                     "Ketik nama, Enter. Kosongkan untuk selesai.");
    dp->separator(box_row + 6, box_col, gw);

    dp->draw_colored(box_row + 7, box_col + 2, COLOR_MENU, 1, "Peserta Terdaftar:");

    int i;
    for (i = 0; i < count; i++) {
        snprintf(buf, sizeof buf, "%2d. %s", i + 1, state->participants[i].name.value);
        dp->draw_colored(box_row + 8 + i, box_col + 4, COLOR_GOLD, 0, buf);
    }

    dp->separator(box_row + 9 + count, box_col, gw);
    dp->footer("[ENTER] Kosongkan & Enter untuk selesai  \xe2\x94\x82  [q] Kembali");
    dp->screen_refresh();
}

int registration_page_input_row(int participant_count) {
    return 4 + 10 + participant_count;
}

int registration_page_error_row(void) {
    return 4 + 5;
}
