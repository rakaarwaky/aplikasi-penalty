/**
 * @file root_scoring_container.c
 * @brief Root container: rakit ScoringAggregate (wiring protocol ke capabilities).
 */

#include "scoring/module.scoring.h"

/**
 * Bangun ScoringAggregate: sambungkan protocol ke capabilities konkret.
 * Protocol `static` agar pointer-nya stabil untuk aggregate.
 *
 * @return ScoringAggregate — berisi pointer protocol yang terisi.
 */
ScoringAggregate root_scoring_build(void) {
    static ScoringProtocol protocol;
    protocol.validate_zone = capabilities_scoring_validate_zone;
    protocol.record_kick = capabilities_scoring_record_kick;

    ScoringAggregate a;
    a.protocol = &protocol;
    return a;
}
