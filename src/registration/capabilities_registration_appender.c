#include "registration/module.registration.h"

#include <stdio.h>

RegistrationError capabilities_registration_append(CompetitionState *state,
                                                   const ParticipantNameVO *name) {
    if (state == NULL || name == NULL) return REG_NAME_EMPTY;
    if (state->participant_count >= MAX_PARTICIPANTS) return REG_FULL;

    int slot = state->participant_count;
    state->participants[slot].id = slot;
    snprintf(state->participants[slot].name.value,
             sizeof state->participants[slot].name.value, "%s", name->value);
    state->participants[slot].total_score = 0;
    state->participants[slot].kick_count = 0;
    for (int z = 0; z <= MAX_ZONE; z++) state->participants[slot].zone_freq[z] = 0;
    for (int k = 0; k < TOTAL_KICKS; k++) state->participants[slot].kicks[k] = -1;

    state->participant_count++;
    if (state->state == STATE_INIT && state->participant_count >= MIN_PARTICIPANTS)
        state->state = STATE_REGISTERED;

    return REG_OK;
}
