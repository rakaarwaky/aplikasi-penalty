/* STORAGE — Protocol (inbound interface) */
#ifndef SHARED_CONTRACT_STORAGE_PROTOCOL_H
#define SHARED_CONTRACT_STORAGE_PROTOCOL_H

#include "shared/taxonomy_storage_error.h"
#include "shared/taxonomy_competition_state_vo.h"

typedef struct {
    StorageError (*save)(const char *filename, const CompetitionState *state);
    StorageError (*load)(const char *filename, CompetitionState *state);
} StorageProtocol;

#endif /* SHARED_CONTRACT_STORAGE_PROTOCOL_H */
