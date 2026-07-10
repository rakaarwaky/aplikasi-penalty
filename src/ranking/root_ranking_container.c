/**
 * @file root_ranking_container.c
 * @brief Siapkan struct ranking dan sambungkan ke fungsi hitung peringkat.
 */

#include "ranking/module.ranking.h"

/**
 * Bangun struct ranking: isi dengan alamat fungsi hitung peringkat
 * agar siap dipakai seluruh program.
 *
 * @return RankingAggregate yang sudah terisi.
 */
RankingAggregate root_ranking_build(void) {
    static RankingProtocol protocol;
    protocol.compute_ranking = capabilities_ranking_compute;

    RankingAggregate a;
    a.protocol = &protocol;
    return a;
}
