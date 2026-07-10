/**
 * @file contract_sanitize_aggregate.h
 * @brief Wadah SanitizeProtocol yang dipakai seluruh program.
 */

#ifndef SHARED_CONTRACT_SANITIZE_AGGREGATE_H
#define SHARED_CONTRACT_SANITIZE_AGGREGATE_H

#include "shared/contract_sanitize_protocol.h"

/** Penghubung ke fungsi sanitasi (disimpan alamatnya saja). */
typedef struct {
    SanitizeProtocol *protocol; /**< Pointer ke fungsi-fungsi sanitasi. */
} SanitizeAggregate;

#endif /* SHARED_CONTRACT_SANITIZE_AGGREGATE_H */
