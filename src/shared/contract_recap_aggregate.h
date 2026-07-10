#ifndef SHARED_CONTRACT_RECAP_AGGREGATE_H
#define SHARED_CONTRACT_RECAP_AGGREGATE_H

/* Aggregate untuk fitur rekapitulasi hasil — menggabungkan recap + ranking. */

#include "shared/contract_recap_protocol.h"
#include "shared/contract_ranking_protocol.h"

/** Aggregate rekapitulasi — menggabungkan protocol recap + ranking. */
typedef struct {
    RecapProtocol *protocol;    /**< Protocol recap (milik fitur recap). */
    RankingProtocol *ranking;   /**< Protocol ranking (dipinjam dari fitur ranking). */
} RecapAggregate;

#endif /* SHARED_CONTRACT_RECAP_AGGREGATE_H */
