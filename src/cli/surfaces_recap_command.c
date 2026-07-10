/**
 * @file surfaces_recap_command.c
 * @brief Smart surface: siapkan data rekap & delegasi render ke page.
 */

#include "cli/module.cli.h"

#include <stdio.h>

static void draw_double_line(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++)
        dp->draw_at(row, col + i, "\xe2\x95\x90");
}

static void show_error_screen(DisplayPort *dp, const char *title,
                              const char *msg, int color) {
    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > 60) gw = 60;
    if (gw < 40) gw = 40;
    int box_col = (cols - gw) / 2;
    char hdr[64];
    snprintf(hdr, sizeof hdr, "  %s  ", title);

    dp->cls();
    draw_double_line(dp, 0, 2, cols - 4);
    dp->print_centered_colored(1, hdr, COLOR_TITLE, 1);
    draw_double_line(dp, 2, 2, cols - 4);
    dp->box(4, box_col, gw, 6);
    dp->print_centered_colored(6, msg, color, 1);
    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}

void cli_surfaces_recap_execute(RecapAggregate *agg, ExportAggregate *ex,
                                CompetitionState *state, DisplayPort *dp) {
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        show_error_screen(dp, "REKAPITULASI LENGKAP",
                          "[GAGAL] Tendangan belum selesai.", COLOR_ERROR);
        return;
    }

    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agent_recap_prepare(agg, state, ranking, details, MAX_PARTICIPANTS);
    if (e != RC_OK) {
        show_error_screen(dp, "REKAPITULASI LENGKAP",
                          "[GAGAL] Rekap belum siap.", COLOR_ERROR);
        return;
    }

    int total_score_all = 0;
    int highest_score   = 0;
    int i;
    for (i = 0; i < state->participant_count; i++) {
        total_score_all += ranking[i].total_score;
        if (ranking[i].total_score > highest_score)
            highest_score = ranking[i].total_score;
    }
    int avg_score = (state->participant_count > 0)
                    ? total_score_all / state->participant_count : 0;

    recap_page_draw(dp, ranking, details, state,
                    total_score_all, avg_score, highest_score, ex);
}
