#ifndef SHARED_CONTRACT_SEARCH_AGGREGATE_H
#define SHARED_CONTRACT_SEARCH_AGGREGATE_H

/* Aggregate untuk fitur pencarian peserta. */

#include "shared/contract_search_protocol.h"

/** Aggregate search — menggabungkan protocol pencarian peserta. */
typedef struct {
    SearchProtocol *protocol; /**< Pointer ke protocol search. */
} SearchAggregate;

#endif /* SHARED_CONTRACT_SEARCH_AGGREGATE_H */
