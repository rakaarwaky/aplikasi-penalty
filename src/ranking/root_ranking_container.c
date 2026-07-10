/**
 * @file root_ranking_container.c
 * @brief Root container: rakit RankingAggregate (wiring protocol ke capabilities).
 */

#include "ranking/module.ranking.h"

/**
 * Bangun RankingAggregate: sambungkan protocol ke capability konkret.
 * Protocol `static` agar pointer-nya stabil untuk aggregate.
 *
 * @return RankingAggregate — berisi pointer protocol yang terisi.
 */
RankingAggregate root_ranking_build(void) {
    static RankingProtocol protocol;
    protocol.compute_ranking = capabilities_ranking_compute;

    RankingAggregate a;
    a.protocol = &protocol;
    return a;
}
