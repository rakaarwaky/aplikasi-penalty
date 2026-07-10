#include "scoring/module.scoring.h"

ScoringError capabilities_scoring_record_kick(CompetitionState *state, int id, ZoneVO zone) {
    if (state == NULL || id < 0 || id >= state->participant_count)
        return SC_PARTICIPANT_NOT_FOUND;
    ParticipantEntity *p = &state->participants[id];
    if (p->kick_count >= TOTAL_KICKS) return SC_ALREADY_DONE;

    int k = p->kick_count;
    p->kicks[k] = zone.value;
    p->total_score += zone.value;
    p->zone_freq[zone.value]++;
    p->kick_count++;

    if (state->state == STATE_REGISTERED) {
        int all = 1;
        for (int i = 0; i < state->participant_count; i++)
            if (state->participants[i].kick_count < TOTAL_KICKS) { all = 0; break; }
        if (all) state->state = STATE_COMPLETED;
    }
    return SC_OK;
}
