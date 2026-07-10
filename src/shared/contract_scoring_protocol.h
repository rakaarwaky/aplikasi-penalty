#ifndef SHARED_CONTRACT_SCORING_PROTOCOL_H
#define SHARED_CONTRACT_SCORING_PROTOCOL_H

/* Inbound protocol untuk pencatatan tendangan (scoring). */

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_zone_vo.h"
#include "shared/taxonomy_competition_error.h"

/**
 * Validasi zona tendangan.
 * @param zone  ZoneVO yang akan divalidasi.
 * @return      ScoringError — SC_OK jika zona valid.
 */
typedef ScoringError (*validate_zone_fn)(ZoneVO zone);

/**
 * Catat satu tendangan ke state peserta.
 * @param state  Pointer ke state kompetisi (akan dimodifikasi).
 * @param id     ID peserta yang melakukan tendangan.
 * @param zone   ZoneVO zona tendangan.
 * @return       ScoringError — SC_OK jika berhasil.
 */
typedef ScoringError (*record_kick_fn)(CompetitionState *state, int id, ZoneVO zone);

/** Protocol scoring. */
typedef struct {
    validate_zone_fn validate_zone;  /**< Validasi zona sebelum pencatatan. */
    record_kick_fn record_kick;      /**< Catat tendangan ke state peserta. */
} ScoringProtocol;

#endif /* SHARED_CONTRACT_SCORING_PROTOCOL_H */
