/**
 * @file root_search_container.c
 * @brief Siapkan struct pencarian dan sambungkan ke fungsi cari.
 */

#include "search/module.search.h"

/**
 * Bangun struct pencarian: isi dengan alamat fungsi cari peserta
 * agar siap dipakai seluruh program.
 *
 * @return SearchAggregate yang sudah terisi.
 */
SearchAggregate root_search_build(void) {
    static SearchProtocol protocol;
    protocol.find_participant = capabilities_search_find;

    SearchAggregate a;
    a.protocol = &protocol;
    return a;
}
