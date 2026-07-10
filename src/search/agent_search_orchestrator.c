#include "search/module.search.h"

/* Koordinasi: delegasi ke capabilities search. Tanpa I/O. */
SearchError agent_search_find(SearchAggregate *agg,
                              const CompetitionState *state,
                              const ParticipantNameVO *name,
                              SearchResultVO *out) {
    if (agg == NULL || state == NULL || name == NULL || out == NULL) return SR_NOT_FOUND;
    return agg->protocol->find_participant(state, name, out);
}
