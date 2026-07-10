/**
 * @file agent_recap_orchestrator.c
 * @brief Agent: orkestrasi recap — pinjam ranking (kontrak) lalu susun detail, tanpa I/O.
 */

#include "recap/module.recap.h"

/**
 * Siapkan data recap: hitung ranking (via RankingProtocol yg dipinjam)
 * lalu susun detail per peserta (via RecapProtocol sendiri).
 *
 * Dua subsistem dikoordinasikan di sini:
 *   1. agg->ranking->compute_ranking(...) — MILIK fitur ranking (dipinjam).
 *   2. agg->protocol->prepare_details(...)   — MILIK fitur recap.
 *
 * @param agg      Pointer ke RecapAggregate (berisi protocol recap + ranking).
 * @param state    Pointer ke state kompetisi (read-only).
 * @param ranking  Array RankingEntryVO tujuan (hasil ranking).
 * @param details  Array SearchResultVO tujuan (hasil detail).
 * @param capacity Kapasitas kedua array.
 * @return         RecapError — RC_NOT_READY bila NULL/gagal ranking, RC_OK.
 */
RecapError agent_recap_prepare(RecapAggregate *agg,
                               const CompetitionState *state,
                               RankingEntryVO *ranking,
                               SearchResultVO *details,
                               int capacity) {
    /* Guard parameter. */
    if (agg == NULL || state == NULL || ranking == NULL || details == NULL) return RC_NOT_READY;

    /* Langkah 1: pinjam ranking dari fitur ranking (kontrak). */
    if (agg->ranking->compute_ranking(state, ranking, capacity) != RK_OK)
        return RC_NOT_READY;

    /* Langkah 2: susun detail peserta dari protocol recap sendiri. */
    return agg->protocol->prepare_details(state, details, capacity);
}
