#include "scoring/module.scoring.h"

ScoringError capabilities_scoring_validate_zone(ZoneVO zone) {
    if (zone.value < MIN_ZONE || zone.value > MAX_ZONE) return SC_INVALID_ZONE;
    return SC_OK;
}
