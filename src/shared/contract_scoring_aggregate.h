/**
 * @file contract_scoring_aggregate.h
 * @brief Wadah protocol scoring yang dipakai seluruh program.
 */

#ifndef SHARED_CONTRACT_SCORING_AGGREGATE_H
#define SHARED_CONTRACT_SCORING_AGGREGATE_H

#include "shared/contract_scoring_protocol.h"
#include "shared/taxonomy_total_score_vo.h"

/** Penghubung ke fungsi scoring (disimpan alamatnya saja). */
typedef struct {
    ScoringProtocol *protocol; /**< Pointer ke fungsi-fungsi scoring. */
} ScoringAggregate;

#endif /* SHARED_CONTRACT_SCORING_AGGREGATE_H */
