#include "recap/module.recap.h"

/* Koordinasi 2 subsistem: urutan dari ranking (kontrak pinjaman) +
   detail dari recap sendiri. Tanpa I/O. */
RecapError agent_recap_prepare(RecapAggregate *agg,
                               const CompetitionState *state,
                               RankingEntryVO *ranking,
                               SearchResultVO *details,
                               int capacity) {
    if (agg == NULL || state == NULL || ranking == NULL || details == NULL) return RC_NOT_READY;

    if (agg->ranking->compute_ranking(state, ranking, capacity) != RK_OK)
        return RC_NOT_READY;

    return agg->protocol->prepare_details(state, details, capacity);
}
