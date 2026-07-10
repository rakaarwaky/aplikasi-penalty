#ifndef SHARED_CONTRACT_REGISTRATION_AGGREGATE_H
#define SHARED_CONTRACT_REGISTRATION_AGGREGATE_H

/* Aggregate untuk fitur pendaftaran peserta. */

#include "shared/contract_registration_protocol.h"

/** Aggregate pendaftaran — menggabungkan protocol pendaftaran. */
typedef struct {
    RegistrationProtocol *protocol; /**< Pointer ke protocol pendaftaran. */
} RegistrationAggregate;

#endif /* SHARED_CONTRACT_REGISTRATION_AGGREGATE_H */
