/**
 * @file agent_search_orchestrator.c
 * @brief Cari peserta melalui fungsi yang sudah disiapkan.
 */

#include "search/module.search.h"

/**
 * Cari peserta berdasar nama.
 *
 * @param agg   Penghubung ke fungsi pencarian.
 * @param state Data lomba (hanya dibaca).
 * @param name  Nama yang dicari.
 * @param out   Tempat menulis hasil.
 * @return      Hasil pencarian, atau SR_NOT_FOUND bila kosong.
 */
SearchError agent_search_find(SearchAggregate *agg,
                              const CompetitionState *state,
                              const ParticipantNameVO *name,
                              SearchResultVO *out) {
    /* Parameter wajib tidak boleh kosong. */
    if (agg == NULL || state == NULL || name == NULL || out == NULL) return SR_NOT_FOUND;
    /* Lakukan pencarian. */
    return agg->protocol->find_participant(state, name, out);
}
