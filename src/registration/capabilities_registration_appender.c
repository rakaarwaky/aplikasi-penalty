/**
 * @file capabilities_registration_appender.c
 * @brief Capability: menambahkan peserta baru ke state kompetisi (murni, tanpa I/O).
 */

#include "registration/module.registration.h"

#include <string.h>

/**
 * Tambah peserta baru ke state.
 *
 * @param state  Pointer ke state kompetisi (dimodifikasi: slot baru + count + state).
 * @param name   Pointer ke VO nama peserta yang sudah tervalidasi.
 * @return       RegistrationError: REG_NAME_EMPTY (NULL), REG_FULL (kuota penuh), REG_OK.
 */
RegistrationError capabilities_registration_append(CompetitionState *state,
                                                   const ParticipantNameVO *name) {
    /* Guard: parameter wajib tidak boleh NULL. */
    if (state == NULL || name == NULL) return REG_NAME_EMPTY;

    /* Guard: kuota maksimum peserta. */
    if (state->participant_count >= MAX_PARTICIPANTS) return REG_FULL;

    /* Slot baru = indeks participant_count saat ini. */
    int slot = state->participant_count;

    /* Isi field entity dengan nilai awal yang konsisten. */
    state->participants[slot].id = slot;
    memcpy(state->participants[slot].name.value,
           name->value, MAX_NAME_LENGTH);
    state->participants[slot].name.value[MAX_NAME_LENGTH] = '\0'; /* jamin null-terminator */
    state->participants[slot].total_score = 0;
    state->participants[slot].kick_count = 0;
    for (int z = 0; z <= MAX_ZONE; z++) state->participants[slot].zone_freq[z] = 0; /* reset frekuensi zona */
    for (int k = 0; k < TOTAL_KICKS; k++) state->participants[slot].kicks[k] = -1; /* -1 = tendangan belum dilakukan */

    /* Komit jumlah peserta. */
    state->participant_count++;

    /* Transisi state machine: dari INIT ke REGISTERED begitu quota minimal tercapai. */
    if (state->state == STATE_INIT && state->participant_count >= MIN_PARTICIPANTS)
        state->state = STATE_REGISTERED;

    return REG_OK;
}
