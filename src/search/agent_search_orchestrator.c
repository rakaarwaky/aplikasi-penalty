/**
 * @file agent_search_orchestrator.c
 * @brief Agent: orkestrasi pencarian (delegasi ke capability), tanpa I/O.
 */

#include "search/module.search.h"

/**
 * Cari peserta via protocol aggregate.
 *
 * @param agg   Pointer ke SearchAggregate (protocol terrakit).
 * @param state Pointer ke state kompetisi (read-only).
 * @param name  Pointer ke VO nama dicari.
 * @param out   Pointer ke SearchResultVO tujuan.
 * @return      SearchError — SR_NOT_FOUND (NULL) atau hasil find_participant.
 */
SearchError agent_search_find(SearchAggregate *agg,
                              const CompetitionState *state,
                              const ParticipantNameVO *name,
                              SearchResultVO *out) {
    /* Guard parameter. */
    if (agg == NULL || state == NULL || name == NULL || out == NULL) return SR_NOT_FOUND;
    /* Delegasikan pencarian ke capability via protocol. */
    return agg->protocol->find_participant(state, name, out);
}
