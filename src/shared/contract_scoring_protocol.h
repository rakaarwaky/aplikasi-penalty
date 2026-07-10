/**
 * @file contract_scoring_protocol.h
 * @brief Daftar fungsi yang harus ada untuk input tendangan & skor.
 */

#ifndef SHARED_CONTRACT_SCORING_PROTOCOL_H
#define SHARED_CONTRACT_SCORING_PROTOCOL_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_zone_vo.h"
#include "shared/taxonomy_competition_error.h"

/* Tipe fungsi: periksa apakah zona dalam rentang 0..5. */
typedef ScoringError (*validate_zone_fn)(ZoneVO zone);

/* Tipe fungsi: catat satu tendangan ke data peserta. */
typedef ScoringError (*record_kick_fn)(CompetitionState *state, int id, ZoneVO zone);

/** Kumpulan fungsi scoring — diisi oleh root container. */
typedef struct {
    validate_zone_fn validate_zone;  /**< Cek zona. */
    record_kick_fn record_kick;      /**< Catat tendangan. */
} ScoringProtocol;

#endif /* SHARED_CONTRACT_SCORING_PROTOCOL_H */
