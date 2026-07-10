#include "scoring/module.scoring.h"

ScoringAggregate root_scoring_build(void) {
    static ScoringProtocol protocol;
    protocol.validate_zone = capabilities_scoring_validate_zone;
    protocol.record_kick = capabilities_scoring_record_kick;

    ScoringAggregate a;
    a.protocol = &protocol;
    return a;
}
