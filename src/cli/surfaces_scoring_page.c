/**
 * @file surfaces_scoring_page.c
 * @brief Dumb surface: render layar input tendangan.
 */

#include "cli/module.cli.h"

#include <stdio.h>

#define BOX_HEIGHT 14

static void draw_double_line(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++)
        dp->draw_at(row, col + i, "\xe2\x95\x90");
}

void scoring_page_draw(DisplayPort *dp, ParticipantEntity *part,
                       const char *msg, int msg_is_error) {
    char buf[128];
    if (!msg_is_error) dp->cls();

    int cols = dp->get_cols();

    int gw = cols - 4;
    if (gw > 64) gw = 64;
    if (gw < 40) gw = 40;
    int box_col = (cols - gw) / 2;
    int box_row = 4;

    dp->print_centered_colored(0, "Menu Utama > Input Tendangan", COLOR_DIM, 0);
    draw_double_line(dp, 1, 2, cols - 4);
    dp->print_centered_colored(2, "       INPUT TENDANGAN DAN SKOR       ", COLOR_TITLE, 1);
    draw_double_line(dp, 3, 2, cols - 4);

    dp->box(box_row, box_col, gw, BOX_HEIGHT);
    dp->separator(box_row + 1, box_col, gw);

    snprintf(buf, sizeof buf, "Peserta: %s", part->name.value);
    dp->draw_colored(box_row + 2, box_col + 2, COLOR_GOLD, 1, buf);

    int kick_pct = (part->kick_count.value * 100) / TOTAL_KICKS;
    snprintf(buf, sizeof buf, "Tendangan %d/%d: ", part->kick_count.value, TOTAL_KICKS);
    dp->draw_colored(box_row + 3, box_col + 2, COLOR_MENU, 0, buf);
    dp->progress_bar(box_row + 3, box_col + 18, 20, kick_pct, COLOR_SUCCESS);

    snprintf(buf, sizeof buf, "Skor sementara: %d poin", part->total_score.value);
    dp->draw_colored(box_row + 4, box_col + 2, COLOR_INFO, 1, buf);

    dp->separator(box_row + 5, box_col, gw);
    dp->draw_colored(box_row + 6, box_col + 2, COLOR_MENU, 0, "Riwayat tendangan:");

    int k;
    for (k = 0; k < TOTAL_KICKS; k++) {
        int cx = box_col + 4 + k * 4;
        int cy = box_row + 7;

        if (k < part->kick_count.value) {
            int zone = part->kicks[k].zone;
            int color = COLOR_DIM;
            if (zone >= 4) color = COLOR_SUCCESS;
            else if (zone >= 2) color = COLOR_WARNING;
            else if (zone == 0) color = COLOR_ERROR;
            snprintf(buf, sizeof buf, "Z%d ", zone);
            dp->draw_colored(cy, cx, color, 1, buf);
        } else if (k == part->kick_count.value) {
            dp->draw_colored(cy, cx, COLOR_INFO, 1, " -> ");
        } else {
            dp->draw_colored(cy, cx, COLOR_DIM, 0, " . ");
        }
    }

    dp->separator(box_row + 9, box_col, gw);
    dp->draw_colored(box_row + 10, box_col + 2, COLOR_DIM, 0,
                     "Zona: 0=Miss 1=Mudah 2=Sedang 3=Sulit 4=SgtSulit 5=Top");

    snprintf(buf, sizeof buf, "Masukkan zona (0-%d, contoh: 5): ", MAX_ZONE);
    dp->draw_colored(box_row + 11, box_col + 2, COLOR_INFO, 1, buf);

    if (msg != NULL && msg[0] != '\0') {
        int msg_color = msg_is_error ? COLOR_ERROR : COLOR_SUCCESS;
        dp->draw_colored(box_row + BOX_HEIGHT - 2, box_col + 2, msg_color, 1, msg);
    }

    dp->screen_refresh();
}
