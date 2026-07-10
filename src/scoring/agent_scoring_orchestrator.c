#include "scoring/module.scoring.h"

#include <stdio.h>

ScoringError agent_scoring_run(ScoringAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return SC_NOT_REGISTERED;
    if (state->state == STATE_INIT) {
        printf("  [GAGAL] Daftar peserta dulu (Menu 1).\n");
        return SC_NOT_REGISTERED;
    }

    agg->port->display_header();
    for (int p = 0; p < state->participant_count; p++) {
        ParticipantEntity *part = &state->participants[p];
        if (part->kick_count >= TOTAL_KICKS) continue;
        while (part->kick_count < TOTAL_KICKS) {
            agg->port->display_prompt(part, part->kick_count + 1);
            ZoneVO z;
            ScoringError re = agg->port->read_zone(&z);
            if (re != SC_OK) { agg->port->display_error(SC_INVALID_ZONE); continue; }
            ScoringError ve = agg->protocol->validate_zone(z);
            if (ve != SC_OK) { agg->port->display_error(ve); continue; }
            agg->protocol->record_kick(state, p, z);
            agg->port->display_result(z.value, z.value);
        }
        printf("  Selesai %s. Total: %d\n", part->name.value, part->total_score);
    }
    if (state->state == STATE_COMPLETED)
        printf("\nSemua tendangan selesai. Status: COMPLETED.\n");
    return SC_OK;
}
