/**
 * @file agent_ranking_orchestrator.c
 * @brief Agent: orkestrasi perhitungan ranking (delegasi ke capability), tanpa I/O.
 */

#include "ranking/module.ranking.h"

/**
 * Hitung ranking via protocol aggregate.
 *
 * @param agg      Pointer ke RankingAggregate (protocol terrakit).
 * @param state    Pointer ke state kompetisi (read-only).
 * @param out      Array RankingEntryVO tujuan.
 * @param capacity Kapasitas array out.
 * @return         RankingError — RK_NO_PARTICIPANT (NULL) atau hasil compute_ranking.
 */
RankingError agent_ranking_compute(RankingAggregate *agg,
                                   const CompetitionState *state,
                                   RankingEntryVO *out, int capacity) {
    /* Guard parameter. */
    if (agg == NULL || state == NULL || out == NULL) return RK_NO_PARTICIPANT;
    /* Delegasikan seluruh perhitungan ke capability via protocol. */
    return agg->protocol->compute_ranking(state, out, capacity);
}
