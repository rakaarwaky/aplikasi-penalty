/**
 * @file surfaces_ranking_command.c
 * @brief Smart surface: hitung peringkat & delegasi render ke page.
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

void cli_surfaces_ranking_execute(RankingAggregate *agg, ExportAggregate *ex,
                                  CompetitionState *state, DisplayPort *dp) {
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        show_error_screen(dp, "RANKING PESERTA",
                          "[GAGAL] Tendangan belum selesai.", COLOR_ERROR);
        return;
    }

    RankingEntryVO entries[MAX_PARTICIPANTS];
    RankingError e = agent_ranking_compute(agg, state, entries, MAX_PARTICIPANTS);
    if (e != RK_OK) {
        show_error_screen(dp, "RANKING PESERTA",
                          "[GAGAL] Ranking belum siap.", COLOR_ERROR);
        return;
    }

    int max_score = 1;
    int ms;
    for (ms = 0; ms < state->participant_count; ms++)
        if (entries[ms].total_score > max_score) max_score = entries[ms].total_score;

    ranking_page_draw(dp, entries, state, max_score, ex);
}
