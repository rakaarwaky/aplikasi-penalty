/**
 * @file capabilities_scoring_zone_validator.c
 * @brief Capability: validasi nilai zona tendangan (murni, tanpa I/O).
 */

#include "scoring/module.scoring.h"

/**
 * Pastikan zona dalam rentang sah [MIN_ZONE, MAX_ZONE].
 *
 * @param zone  ZoneVO yang akan divalidasi.
 * @return      ScoringError — SC_OK bila sah, SC_INVALID_ZONE bila di luar rentang.
 */
ScoringError capabilities_scoring_validate_zone(ZoneVO zone) {
    if (zone.value < MIN_ZONE || zone.value > MAX_ZONE) return SC_INVALID_ZONE;
    return SC_OK;
}
