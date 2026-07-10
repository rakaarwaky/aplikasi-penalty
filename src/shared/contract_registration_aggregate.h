/**
 * @file contract_registration_aggregate.h
 * @brief Wadah protocol pendaftaran yang dipakai seluruh program.
 */

#ifndef SHARED_CONTRACT_REGISTRATION_AGGREGATE_H
#define SHARED_CONTRACT_REGISTRATION_AGGREGATE_H

#include "shared/contract_registration_protocol.h"
#include "shared/taxonomy_participant_vo.h"

/** Penghubung ke fungsi pendaftaran (disimpan alamatnya saja). */
typedef struct {
    RegistrationProtocol *protocol; /**< Pointer ke fungsi-fungsi pendaftaran. */
} RegistrationAggregate;

#endif /* SHARED_CONTRACT_REGISTRATION_AGGREGATE_H */
