/**
 * @file capabilities_recap_formatter.c
 * @brief Kumpulkan data tiap peserta untuk ditampilkan di rekapitulasi.
 */

#include "recap/module.recap.h"

/**
 * Salin data seluruh peserta ke array hasil rekap.
 *
 * @param state    Data lomba (hanya dibaca).
 * @param details Tempat menulis data tiap peserta.
 * @param capacity Cukupnya ruang pada array details.
 * @return         RC_OK bila berhasil, atau error bila belum siap/ruang kurang.
 */
RecapError capabilities_recap_prepare_details(const CompetitionState *state,
                                              SearchResultVO *details, int capacity) {
    /* Data atau ruang tidak sah. */
    if (state == NULL || details == NULL) return RC_NOT_READY;
    if (state->state != STATE_COMPLETED) return RC_NOT_READY;
    if (capacity < state->participant_count) return RC_NOT_READY;

    /* Salin data tiap peserta ke hasil. */
    for (int i = 0; i < state->participant_count; i++) {
        const ParticipantEntity *p = &state->participants[i];
        details[i].found = 1;
        details[i].participant_id = i;
        for (int c = 0; c < MAX_NAME_LENGTH + 1; c++) details[i].name[c] = p->name.value[c];
        details[i].total_score = p->total_score.value;
        for (int k = 0; k < TOTAL_KICKS; k++) details[i].kicks[k] = p->kicks[k].zone;
        for (int z = 0; z <= MAX_ZONE; z++) details[i].zone_freq[z] = p->zone_freq.freq[z];
    }
    return RC_OK;
}
