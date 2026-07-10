#include "ranking/module.ranking.h"

/* Koordinasi: delegasi ke capabilities ranking. Tanpa I/O. */
RankingError agent_ranking_compute(RankingAggregate *agg,
                                   const CompetitionState *state,
                                   RankingEntryVO *out, int capacity) {
    if (agg == NULL || state == NULL || out == NULL) return RK_NO_PARTICIPANT;
    return agg->protocol->compute_ranking(state, out, capacity);
}
