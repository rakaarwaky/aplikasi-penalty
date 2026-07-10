#ifndef SHARED_CONTRACT_RANKING_AGGREGATE_H
#define SHARED_CONTRACT_RANKING_AGGREGATE_H

/* Aggregate untuk fitur perhitungan ranking. */

#include "shared/contract_ranking_protocol.h"

/** Aggregate ranking — menggabungkan protocol perhitungan ranking. */
typedef struct {
    RankingProtocol *protocol; /**< Pointer ke protocol ranking. */
} RankingAggregate;

#endif /* SHARED_CONTRACT_RANKING_AGGREGATE_H */
