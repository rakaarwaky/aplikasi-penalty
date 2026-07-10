/**
 * @file capabilities_ranking_calculator.c
 * @brief Capability: hitung ranking dengan tie-breaker frekuensi zona (murni, tanpa I/O).
 */

#include "ranking/module.ranking.h"

#include <stdlib.h>

/* ──────────────────────────────────────────────
 * Comparator qsort untuk menyusun RankingEntryVO.
 * Urutan prioritas (PRD 2.3):
 *   1. total_score DESC (skor tinggi di atas)
 *   2. tie-breaker: frekuensi zona 5,4,3,2,1 DESC
 *      (zona tinggi lebih diutamakan)
 * Mengembalikan <0 bila x harus di atas y.
 * ────────────────────────────────────────────── */
static int compare_entries(const void *a, const void *b) {
    const RankingEntryVO *x = (const RankingEntryVO *)a;
    const RankingEntryVO *y = (const RankingEntryVO *)b;
    if (x->total_score != y->total_score) return y->total_score - x->total_score;
    for (int z = MAX_ZONE; z >= 1; z--)
        if (x->zone_freq[z] != y->zone_freq[z])
            return y->zone_freq[z] - x->zone_freq[z];
    return 0;
}

/**
 * Hitung ranking seluruh peserta dari state.
 *
 * Langkah:
 *   1. Validasi guard (state/out NULL, state belum COMPLETED, kapasitas cukup).
 *   2. Salin data peserta ke array out (partisipan_id, total_score, zone_freq).
 *   3. qsort O(N log N) pakai compare_entries.
 *   4. Tetapkan rank dengan aturan seri: peserta sebanding (compare == 0)
 *      memakai rank yang sama, lalu lanjut (1,1,3,...).
 *
 * @param state    Pointer ke state kompetisi (read-only).
 * @param out      Array RankingEntryVO tujuan (minimal participant_count entri).
 * @param capacity Kapasitas array out.
 * @return         RankingError: RK_NO_PARTICIPANT (NULL/kuota), RK_NOT_READY (state),
 *                 RK_OK.
 */
RankingError capabilities_ranking_compute(const CompetitionState *state,
                                          RankingEntryVO *out, int capacity) {
    /* Guard: pointer wajib & kapasitas cukup. */
    if (state == NULL || out == NULL) return RK_NO_PARTICIPANT;
    if (state->state != STATE_COMPLETED) return RK_NOT_READY;
    if (capacity < state->participant_count) return RK_NO_PARTICIPANT;

    /* Salin data peserta -> array hasil (rank di-reset ke 0 dulu). */
    for (int i = 0; i < state->participant_count; i++) {
        out[i].participant_id = i;
        out[i].total_score = state->participants[i].total_score;
        for (int z = 0; z <= MAX_ZONE; z++)
            out[i].zone_freq[z] = state->participants[i].zone_freq[z];
        out[i].rank = 0;
    }

    /* Urutkan berdasar skor + tie-breaker zona. */
    qsort(out, state->participant_count, sizeof(RankingEntryVO), compare_entries);

    /* Tetapkan rank seri: 1,1,3,... (bandingkan dengan entri sebelumnya). */
    if (state->participant_count > 0) {
        out[0].rank = 1;
        for (int i = 1; i < state->participant_count; i++) {
            if (compare_entries(&out[i - 1], &out[i]) == 0)
                out[i].rank = out[i - 1].rank; /* seri */
            else
                out[i].rank = i + 1;
        }
    }
    return RK_OK;
}
