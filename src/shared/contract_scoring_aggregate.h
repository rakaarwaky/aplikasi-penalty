#ifndef SHARED_CONTRACT_SCORING_AGGREGATE_H
#define SHARED_CONTRACT_SCORING_AGGREGATE_H

/* Aggregate untuk fitur pencatatan tendangan. */

#include "shared/contract_scoring_protocol.h"

/** Aggregate scoring — menggabungkan protocol pencatatan tendangan. */
typedef struct {
    ScoringProtocol *protocol; /**< Pointer ke protocol scoring. */
} ScoringAggregate;

#endif /* SHARED_CONTRACT_SCORING_AGGREGATE_H */
