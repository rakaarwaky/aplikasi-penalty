#include "recap/module.recap.h"

/* Recap hanya menyusun detail per peserta (indeks = participant_id).
   Urutan/ranking TIDAK dihitung di sini — dipinjam dari RankingProtocol. */
RecapError capabilities_recap_prepare_details(const CompetitionState *state,
                                              SearchResultVO *details, int capacity) {
    if (state == NULL || details == NULL) return RC_NOT_READY;
    if (state->state != STATE_COMPLETED) return RC_NOT_READY;
    if (capacity < state->participant_count) return RC_NOT_READY;

    for (int i = 0; i < state->participant_count; i++) {
        const ParticipantEntity *p = &state->participants[i];
        details[i].found = 1;
        details[i].participant_id = i;
        for (int c = 0; c < MAX_NAME_LENGTH + 1; c++) details[i].name[c] = p->name.value[c];
        details[i].total_score = p->total_score;
        for (int k = 0; k < TOTAL_KICKS; k++) details[i].kicks[k] = p->kicks[k];
        for (int z = 0; z <= MAX_ZONE; z++) details[i].zone_freq[z] = p->zone_freq[z];
    }
    return RC_OK;
}
