/**
 * @file agent_storage_orchestrator.c
 * @brief Simpan/muat data lomba ke/dari file melalui fungsi penyimpan.
 */

/* STORAGE — Agent Orchestrator */
#include "module.storage.h"

/**
 * Simpan data lomba ke file.
 */
StorageError agent_storage_save(const StorageAggregate *agg,
                                const char *filename,
                                const CompetitionState *state) {
    /* Jangan jalankan bila penghubung belum siap. */
    if (agg == NULL || agg->protocol == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->save == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    return agg->protocol->save(filename, state);
}

/**
 * Muat data lomba dari file.
 */
StorageError agent_storage_load(const StorageAggregate *agg,
                                const char *filename,
                                CompetitionState *state) {
    /* Jangan jalankan bila penghubung belum siap. */
    if (agg == NULL || agg->protocol == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->load == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    return agg->protocol->load(filename, state);
}
