/**
 * @file surfaces_recap_command.c
 * @brief Smart surface: siapkan data rekap & delegasi render ke page.
 */

#include "cli/module.cli.h"

#include <stdio.h>

void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state,
                                DisplayPort *dp) {
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Tendangan belum selesai.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agent_recap_prepare(agg, state, ranking, details, MAX_PARTICIPANTS);
    if (e != RC_OK) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Rekap belum siap.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
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
                    total_score_all, avg_score, highest_score);
}
