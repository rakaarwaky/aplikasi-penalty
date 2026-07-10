/**
 * @file agent_scoring_orchestrator.c
 * @brief Agent: orkestrasi pipeline scoring (validasi zona lalu catat), tanpa I/O.
 */

#include "scoring/module.scoring.h"

/**
 * Catat tendangan: validasi zona lalu rekam, via protocol aggregate.
 *
 * @param agg   Pointer ke ScoringAggregate (protocol terrakit).
 * @param state Pointer ke state kompetisi (dimodifikasi bila rekam berhasil).
 * @param id    ID peserta.
 * @param zone  ZoneVO zona tendangan.
 * @return      ScoringError — SC_NOT_REGISTERED (agg/state NULL), atau hasil validasi/rekam.
 */
ScoringError agent_scoring_record(ScoringAggregate *agg,
                                  CompetitionState *state,
                                  int id, ZoneVO zone) {
    /* Guard parameter. */
    if (agg == NULL || state == NULL) return SC_NOT_REGISTERED;

    /* Langkah 1: delegasikan validasi zona. */
    ScoringError ve = agg->protocol->validate_zone(zone);
    if (ve != SC_OK) return ve; /* zona tidak sah -> hentikan */

    /* Langkah 2: rekam tendangan. */
    return agg->protocol->record_kick(state, id, zone);
}
