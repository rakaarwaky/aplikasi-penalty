/**
 * @file surfaces_scoring_page.c
 * @brief Dumb surface: render layar input tendangan.
 */

#include "cli/module.cli.h"

#include <stdio.h>

#define BOX_WIDTH 58
#define BOX_HEIGHT 14
#define BOX_ROW 3
#define BOX_COL 2

void scoring_page_draw(DisplayPort *dp, ParticipantEntity *part,
                       const char *msg, int msg_is_error) {
    char buf[128];
    if (!msg_is_error) dp->cls();

    dp->print_centered_colored(0, "Menu Utama > Input Tendangan", COLOR_DIM, 0);
    dp->print_centered_colored(1, UTF_DOUBLE_H_32, COLOR_DIM, 0);
    dp->print_centered_colored(2, "       INPUT TENDANGAN DAN SKOR       ", COLOR_TITLE, 1);
    dp->print_centered_colored(3, UTF_DOUBLE_H_32, COLOR_DIM, 0);

    dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
    dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    snprintf(buf, sizeof buf, "Peserta: %s", part->name.value);
    dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_GOLD, 1, buf);

    int kick_pct = (part->kick_count.value * 100) / TOTAL_KICKS;
    snprintf(buf, sizeof buf, "Tendangan %d/%d: ", part->kick_count.value, TOTAL_KICKS);
    dp->draw_colored(BOX_ROW + 3, BOX_COL + 2, COLOR_MENU, 0, buf);
    dp->progress_bar(BOX_ROW + 3, BOX_COL + 18, 20, kick_pct, COLOR_SUCCESS);

    snprintf(buf, sizeof buf, "Skor sementara: %d poin", part->total_score.value);
    dp->draw_colored(BOX_ROW + 4, BOX_COL + 2, COLOR_INFO, 1, buf);

    dp->separator(BOX_ROW + 5, BOX_COL, BOX_WIDTH);
    dp->draw_colored(BOX_ROW + 6, BOX_COL + 2, COLOR_MENU, 0, "Riwayat tendangan:");

    int k;
    for (k = 0; k < TOTAL_KICKS; k++) {
        int cx = BOX_COL + 4 + k * 4;
        int cy = BOX_ROW + 7;

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

    dp->separator(BOX_ROW + 9, BOX_COL, BOX_WIDTH);
    dp->draw_colored(BOX_ROW + 10, BOX_COL + 2, COLOR_DIM, 0,
                     "Zona: 0=Miss 1=Mudah 2=Sedang 3=Sulit 4=SgtSulit 5=Top");

    snprintf(buf, sizeof buf, "Masukkan zona (0-%d, contoh: 5): ", MAX_ZONE);
    dp->draw_colored(BOX_ROW + 11, BOX_COL + 2, COLOR_INFO, 1, buf);

    if (msg != NULL && msg[0] != '\0') {
        int msg_color = msg_is_error ? COLOR_ERROR : COLOR_SUCCESS;
        dp->draw_colored(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, msg_color, 1, msg);
    }

    dp->screen_refresh();
}
