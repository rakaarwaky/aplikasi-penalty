/**
 * @file surfaces_ranking_command.c
 * @brief Smart surface: hitung peringkat & delegasi render ke page.
 */

#include "cli/module.cli.h"

#include <stdio.h>

void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state,
                                  DisplayPort *dp) {
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Tendangan belum selesai.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    RankingEntryVO entries[MAX_PARTICIPANTS];
    RankingError e = agent_ranking_compute(agg, state, entries, MAX_PARTICIPANTS);
    if (e != RK_OK) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Ranking belum siap.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    int max_score = 1;
    int ms;
    for (ms = 0; ms < state->participant_count; ms++)
        if (entries[ms].total_score > max_score) max_score = entries[ms].total_score;

    ranking_page_draw(dp, entries, state, max_score);
}
