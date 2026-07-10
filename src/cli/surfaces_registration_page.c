/**
 * @file surfaces_registration_page.c
 * @brief Dumb surface: render layar pendaftaran peserta.
 */

#include "cli/module.cli.h"

#include <stdio.h>

#define BOX_WIDTH 56
#define BOX_ROW 3
#define BOX_COL 2

void registration_page_draw(DisplayPort *dp, CompetitionState *state) {
    char buf[128];
    dp->cls();

    int count = state->participant_count;
    int box_height = count + 13;
    int max_h = dp->get_lines() - 4;
    if (box_height > max_h) box_height = max_h;

    dp->print_centered_colored(0, "Menu Utama > Pendaftaran Peserta", COLOR_DIM, 0);
    dp->print_centered_colored(1, UTF_DOUBLE_H_32, COLOR_DIM, 0);
    dp->print_centered_colored(2, "       PENDAFTARAN PESERTA        ", COLOR_TITLE, 1);
    dp->print_centered_colored(3, UTF_DOUBLE_H_32, COLOR_DIM, 0);

    dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, box_height);

    int pct = (count * 100) / MAX_PARTICIPANTS;
    snprintf(buf, sizeof buf, "Kuota: %d/%d Terisi", count, MAX_PARTICIPANTS);
    dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_INFO, 0, buf);
    dp->progress_bar(BOX_ROW + 3, BOX_COL + 2, 30, pct, COLOR_SUCCESS);

    dp->separator(BOX_ROW + 4, BOX_COL, BOX_WIDTH);
    dp->draw_colored(BOX_ROW + 5, BOX_COL + 2, COLOR_DIM, 0,
                     "Ketik nama, Enter. Kosongkan untuk selesai.");
    dp->separator(BOX_ROW + 6, BOX_COL, BOX_WIDTH);

    dp->draw_colored(BOX_ROW + 7, BOX_COL + 2, COLOR_MENU, 1, "Peserta Terdaftar:");

    int i;
    for (i = 0; i < count; i++) {
        snprintf(buf, sizeof buf, "%2d. %s", i + 1, state->participants[i].name.value);
        dp->draw_colored(BOX_ROW + 8 + i, BOX_COL + 4, COLOR_GOLD, 0, buf);
    }

    dp->separator(BOX_ROW + 9 + count, BOX_COL, BOX_WIDTH);
    dp->screen_refresh();
}

int registration_page_input_row(int participant_count) {
    return BOX_ROW + 10 + participant_count;
}

int registration_page_error_row(int participant_count) {
    return BOX_ROW + participant_count + 13 - 2;
}
