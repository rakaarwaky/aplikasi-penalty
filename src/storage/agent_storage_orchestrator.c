/* STORAGE — Agent Orchestrator */
#include "module.storage.h"

StorageError agent_storage_save(const StorageAggregate *agg,
                                const char *filename,
                                const CompetitionState *state) {
    if (agg == NULL || agg->protocol == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->save == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    return agg->protocol->save(filename, state);
}

StorageError agent_storage_load(const StorageAggregate *agg,
                                const char *filename,
                                CompetitionState *state) {
    if (agg == NULL || agg->protocol == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->load == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    return agg->protocol->load(filename, state);
}
