/**
 * @file root_recap_container.c
 * @brief Root container: rakit RecapAggregate (wiring protocol recap + ranking pinjaman).
 */

#include "recap/module.recap.h"

/**
 * Bangun RecapAggregate.
 *
 * Berbeda dengan container lain, recap menerima `ranking_protocol` sebagai
 * argumen — karena RecapAggregate menyimpan pointer ke RankingProtocol
 * milik fitur ranking (dipinjam, bukan dirakit di sini).
 *
 * @param ranking_protocol Pointer ke RankingProtocol yg sudah di-build
 *                        (dari root_ranking_build()).
 * @return RecapAggregate — berisi protocol recap + pointer ranking.
 */
RecapAggregate root_recap_build(RankingProtocol *ranking_protocol) {
    static RecapProtocol protocol;
    protocol.prepare_details = capabilities_recap_prepare_details;

    RecapAggregate a;
    a.protocol = &protocol;
    a.ranking = ranking_protocol;
    return a;
}
