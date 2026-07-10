/**
 * @file root_search_container.c
 * @brief Root container: rakit SearchAggregate (wiring protocol ke capabilities).
 */

#include "search/module.search.h"

/**
 * Bangun SearchAggregate: sambungkan protocol ke capability konkret.
 * Protocol `static` agar pointer-nya stabil untuk aggregate.
 *
 * @return SearchAggregate — berisi pointer protocol yang terisi.
 */
SearchAggregate root_search_build(void) {
    static SearchProtocol protocol;
    protocol.find_participant = capabilities_search_find;

    SearchAggregate a;
    a.protocol = &protocol;
    return a;
}
