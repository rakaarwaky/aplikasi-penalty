/**
 * @file capabilities_ranking_calculator.c
 * @brief Hitung urutan peringkat peserta beserta aturan seri.
 */

#include "ranking/module.ranking.h"

#include <stdlib.h>

/* Urutkan dua peserta: skor tinggi di atas; bila skor sama,
   zona tinggi (5..1) lebih diutamakan sebagai pemecah seri. */
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
 * Susun peringkat seluruh peserta dari data lomba.
 *
 * Hasil: urutkan berdasar skor lalu frekuensi zona, lalu tetapkan
 * nomor peringkat (peserta seri mendapat nomor yang sama).
 *
 * @param state    Data lomba (hanya dibaca).
 * @param out      Tempat menulis hasil peringkat.
 * @param capacity Cukupnya ruang pada array out.
 * @return         RK_OK bila berhasil, atau error bila belum siap/ruang kurang.
 */
RankingError capabilities_ranking_compute(const CompetitionState *state,
                                          RankingEntryVO *out, int capacity) {
    /* Data atau ruang tidak sah. */
    if (state == NULL || out == NULL) return RK_NO_PARTICIPANT;
    if (state->state != STATE_COMPLETED) return RK_NOT_READY;
    if (capacity < state->participant_count) return RK_NO_PARTICIPANT;

    /* Salin data peserta ke hasil. */
    for (int i = 0; i < state->participant_count; i++) {
        out[i].participant_id = i;
        out[i].total_score = state->participants[i].total_score;
        for (int z = 0; z <= MAX_ZONE; z++)
            out[i].zone_freq[z] = state->participants[i].zone_freq[z];
        out[i].rank = 0;
    }

    /* Urutkan berdasar skor dan frekuensi zona. */
    qsort(out, state->participant_count, sizeof(RankingEntryVO), compare_entries);

    /* Tetapkan nomor peringkat, jaga aturan seri (1,1,3,...). */
    if (state->participant_count > 0) {
        out[0].rank = 1;
        for (int i = 1; i < state->participant_count; i++) {
            if (compare_entries(&out[i - 1], &out[i]) == 0)
                out[i].rank = out[i - 1].rank;
            else
                out[i].rank = i + 1;
        }
    }
    return RK_OK;
}
