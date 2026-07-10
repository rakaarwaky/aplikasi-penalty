/**
 * @file agent_recap_orchestrator.c
 * @brief Susun data rekap: hitung peringkat lalu kumpulkan detail.
 */

#include "recap/module.recap.h"

/**
 * Siapkan data rekapitulasi: dapatkan urutan peringkat, lalu
 * kumpulkan detail tiap peserta.
 *
 * @param agg      Struct recap (berisi fungsi detail & peringkat).
 * @param state    Data lomba (hanya dibaca).
 * @param ranking  Tempat menulis hasil peringkat.
 * @param details  Tempat menulis detail peserta.
 * @param capacity Cukupnya ruang pada kedua array.
 * @return         RC_OK bila berhasil, atau RC_NOT_READY bila gagal.
 */
RecapError agent_recap_prepare(RecapAggregate *agg,
                               const CompetitionState *state,
                               RankingEntryVO *ranking,
                               SearchResultVO *details,
                               int capacity) {
    /* Parameter wajib tidak boleh kosong. */
    if (agg == NULL || state == NULL || ranking == NULL || details == NULL) return RC_NOT_READY;

    /* Langkah 1: hitung peringkat peserta. */
    if (agg->ranking->compute_ranking(state, ranking, capacity) != RK_OK)
        return RC_NOT_READY;

    /* Langkah 2: kumpulkan detail tiap peserta. */
    return agg->protocol->prepare_details(state, details, capacity);
}
