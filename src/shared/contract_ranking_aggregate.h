/**
 * @file contract_ranking_aggregate.h
 * @brief Wadah protocol ranking yang dipakai seluruh program.
 */

#ifndef SHARED_CONTRACT_RANKING_AGGREGATE_H
#define SHARED_CONTRACT_RANKING_AGGREGATE_H

#include "shared/contract_ranking_protocol.h"
#include "shared/taxonomy_rank_vo.h"

/** Penghubung ke fungsi ranking (disimpan alamatnya saja). */
typedef struct {
    RankingProtocol *protocol; /**< Pointer ke fungsi-fungsi ranking. */
} RankingAggregate;

#endif /* SHARED_CONTRACT_RANKING_AGGREGATE_H */
