#include "ranking/module.ranking.h"

RankingError agent_ranking_run(RankingAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return RK_NOT_READY;
    if (state->state != STATE_COMPLETED) {
        agg->port->display_not_ready();
        return RK_NOT_READY;
    }
    RankingEntryVO entries[MAX_PARTICIPANTS];
    RankingError e = agg->protocol->compute_ranking(state, entries, MAX_PARTICIPANTS);
    if (e != RK_OK) return e;

    agg->port->display_header();
    for (int i = 0; i < state->participant_count; i++)
        agg->port->display_entry(state, &entries[i]);
    return RK_OK;
}
