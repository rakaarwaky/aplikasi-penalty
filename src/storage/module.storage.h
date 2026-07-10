/* Deklarasi modul penyimpanan — save/load state kompetisi. */
#ifndef MODULE_STORAGE_H
#define MODULE_STORAGE_H

#include "shared/contract_storage_protocol.h"
#include "shared/taxonomy_storage_error.h"
#include "shared/taxonomy_competition_state_vo.h"

/* INFRASTRUCTURE — implementasi file I/O (mengimplementasikan StorageProtocol) */
StorageProtocol storage_adapter_create(void);

/* AGENT — orkestrasi melalui protocol */
typedef struct {
    const StorageProtocol *protocol;
} StorageAggregate;

StorageError agent_storage_save(const StorageAggregate *agg,
                                const char *filename,
                                const CompetitionState *state);

StorageError agent_storage_load(const StorageAggregate *agg,
                                const char *filename,
                                CompetitionState *state);

/* ROOT — container builder (wiring only) */
StorageAggregate root_storage_build(void);

#endif /* MODULE_STORAGE_H */
