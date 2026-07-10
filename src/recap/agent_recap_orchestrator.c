#include "recap/module.recap.h"

/* Koordinasi: delegasi ke capabilities recap. Tanpa I/O. */
RecapError agent_recap_prepare(RecapAggregate *agg,
                               const CompetitionState *state,
                               RankingEntryVO *ranking,
                               SearchResultVO *details,
                               int capacity) {
    if (agg == NULL || state == NULL || ranking == NULL || details == NULL) return RC_NOT_READY;
    return agg->protocol->prepare_recap(state, ranking, details, capacity);
}
