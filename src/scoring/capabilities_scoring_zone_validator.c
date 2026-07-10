/**
 * @file capabilities_scoring_zone_validator.c
 * @brief Cek apakah angka zona tendangan berada dalam rentang sah.
 */

#include "scoring/module.scoring.h"

/**
 * Periksa angka zona tendangan.
 *
 * @param zone  Zona yang dimasukkan pengguna.
 * @return      SC_OK bila 0-5, SC_INVALID_ZONE bila di luar rentang.
 */
ScoringError capabilities_scoring_validate_zone(ZoneVO zone) {
    /* Zona harus antara 0 (miss) dan 5 (terendah poin tertinggi). */
    if (zone.value < MIN_ZONE || zone.value > MAX_ZONE) return SC_INVALID_ZONE;
    return SC_OK;
}
