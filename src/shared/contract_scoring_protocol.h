#ifndef SHARED_CONTRACT_SCORING_PROTOCOL_H
#define SHARED_CONTRACT_SCORING_PROTOCOL_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_zone_vo.h"
#include "shared/taxonomy_competition_error.h"

/* AES402: pakai ZoneVO, bukan int mentah. */
typedef ScoringError (*validate_zone_fn)(ZoneVO zone);
typedef ScoringError (*record_kick_fn)(CompetitionState *state, int id, ZoneVO zone);
typedef struct {
    validate_zone_fn validate_zone;
    record_kick_fn record_kick;
} ScoringProtocol;

#endif /* SHARED_CONTRACT_SCORING_PROTOCOL_H */
