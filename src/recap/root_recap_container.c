/**
 * @file root_recap_container.c
 * @brief Siapkan struct recap dan sambungkan ke fungsi detail & peringkat.
 */

#include "recap/module.recap.h"

/**
 * Bangun struct recap: isi fungsi detail peserta, lalu simpan
 * penghubung peringkat yang sudah dibuat di tempat lain.
 *
 * @param ranking_protocol Penghubung peringkat yang sudah siap.
 * @return RecapAggregate yang sudah terisi.
 */
RecapAggregate root_recap_build(RankingProtocol *ranking_protocol) {
    static RecapProtocol protocol;
    protocol.prepare_details = capabilities_recap_prepare_details;

    RecapAggregate a;
    a.protocol = &protocol;
    a.ranking = ranking_protocol;
    return a;
}
