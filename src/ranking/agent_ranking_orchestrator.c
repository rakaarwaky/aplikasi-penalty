/**
 * @file agent_ranking_orchestrator.c
 * @brief Hitung peringkat melalui fungsi yang sudah disiapkan.
 */

#include "ranking/module.ranking.h"

/**
 * Hitung peringkat peserta.
 *
 * @param agg      Penghubung ke fungsi hitung peringkat.
 * @param state    Data lomba (hanya dibaca).
 * @param out      Tempat menulis hasil peringkat.
 * @param capacity Cukupnya ruang pada array out.
 * @return         Hasil perhitungan, atau RK_NO_PARTICIPANT bila kosong.
 */
RankingError agent_ranking_compute(RankingAggregate *agg,
                                   const CompetitionState *state,
                                   RankingEntryVO *out, int capacity) {
    /* Parameter wajib tidak boleh kosong. */
    if (agg == NULL || state == NULL || out == NULL) return RK_NO_PARTICIPANT;
    /* Hitung peringkat. */
    return agg->protocol->compute_ranking(state, out, capacity);
}
