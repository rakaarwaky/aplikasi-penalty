/**
 * @file contract_search_aggregate.h
 * @brief Wadah protocol search yang dipakai seluruh program.
 */

#ifndef SHARED_CONTRACT_SEARCH_AGGREGATE_H
#define SHARED_CONTRACT_SEARCH_AGGREGATE_H

#include "shared/contract_search_protocol.h"
#include "shared/taxonomy_search_result_vo.h"

/** Penghubung ke fungsi pencarian (disimpan alamatnya saja). */
typedef struct {
    SearchProtocol *protocol; /**< Pointer ke fungsi-fungsi cari. */
} SearchAggregate;

#endif /* SHARED_CONTRACT_SEARCH_AGGREGATE_H */
