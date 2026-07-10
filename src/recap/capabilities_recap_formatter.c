/**
 * @file capabilities_recap_formatter.c
 * @brief Capability: susun detail per peserta untuk rekapitulasi (murni, tanpa I/O).
 */

#include "recap/module.recap.h"

/**
 * Isikan array detail dengan data seluruh peserta (indeks = participant_id).
 *
 * File ini HANYA menyusun detail; urutan/ranking TIDAK dihitung di sini
 * — recap meminjam RankingProtocol dari fitur ranking (lihat agent).
 * Data yang disalin: nama, total_score, kicks, zone_freq.
 *
 * @param state    Pointer ke state kompetisi (read-only).
 * @param details  Array SearchResultVO tujuan (minimal participant_count).
 * @param capacity Kapasitas array details.
 * @return         RecapError: RC_NOT_READY (NULL/state belum COMPLETED/kuota),
 *                 RC_OK.
 */
RecapError capabilities_recap_prepare_details(const CompetitionState *state,
                                              SearchResultVO *details, int capacity) {
    /* Guard: pointer & kapasitas. */
    if (state == NULL || details == NULL) return RC_NOT_READY;
    if (state->state != STATE_COMPLETED) return RC_NOT_READY;
    if (capacity < state->participant_count) return RC_NOT_READY;

    /* Salin data tiap peserta ke entri detail. */
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
