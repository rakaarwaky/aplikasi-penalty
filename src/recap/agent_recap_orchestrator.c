#include "recap/module.recap.h"

RecapError agent_recap_run(RecapAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return RC_NOT_READY;
    if (state->state != STATE_COMPLETED) {
        agg->port->display_not_ready();
        return RC_NOT_READY;
    }
    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agg->protocol->prepare_recap(state, ranking, details, MAX_PARTICIPANTS);
    if (e != RC_OK) return e;

    agg->port->display_header();
    for (int i = 0; i < state->participant_count; i++)
        agg->port->display_row(state, &ranking[i], &details[ranking[i].participant_id]);
    agg->port->display_wait();
    return RC_OK;
}
