#include "scoring/module.scoring.h"

/* Koordinasi 2 subsistem capabilities: validasi zona lalu pencatatan. Tanpa I/O. */
ScoringError agent_scoring_record(ScoringAggregate *agg,
                                  CompetitionState *state,
                                  int id, ZoneVO zone) {
    if (agg == NULL || state == NULL) return SC_NOT_REGISTERED;

    ScoringError ve = agg->protocol->validate_zone(zone);
    if (ve != SC_OK) return ve;

    return agg->protocol->record_kick(state, id, zone);
}
