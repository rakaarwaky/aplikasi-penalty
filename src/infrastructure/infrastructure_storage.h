akjelassiapa pemilik/* INFRASTRUCTURE — Data Persistence Layer */
#ifndef INFRASTRUCTURE_STORAGE_H
#define INFRASTRUCTURE_STORAGE_H

#include "shared/taxonomy_competition_error.h"
#include "shared/taxonomy_competition_state_vo.h"

typedef struct {
    char filename[256];
} Storage;

StorageError storage_init(Storage *storage, const char *filename);
StorageError storage_save(const Storage *storage, const CompetitionState *state);
StorageError storage_load(Storage *storage, CompetitionState *state);

#endif /* INFRASTRUCTURE_STORAGE_H */
