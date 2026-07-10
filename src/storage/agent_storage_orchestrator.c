/**
 * @file agent_storage_orchestrator.c
 * @brief Simpan/muat data lomba ke/dari file melalui fungsi penyimpan.
 */

/* STORAGE — Agent Orkestrator */
#include "module.storage.h"
#include "shared/taxonomy_logger.h"

/**
 * Simpan data lomba ke file.
 */
StorageError agent_storage_save(const StorageAggregate *agg,
                                const char *filename,
                                const CompetitionState *state) {
    LOG_INFO("storage_save: memulai penyimpanan");
    /* Jangan jalankan bila penghubung belum siap. */
    if (agg == NULL || agg->protocol == NULL) {
        LOG_ERROR("storage_save: aggregate/protocol NULL");
        return ST_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->save == NULL) {
        LOG_ERROR("storage_save: fungsi save NULL");
        return ST_ERROR_FILE_NOT_FOUND;
    }
    StorageError result = agg->protocol->save(filename, state);
    if (result == ST_OK)
        LOG_INFO("storage_save: berhasil");
    else
        LOG_ERROR("storage_save: gagal");
    return result;
}

/**
 * Muat data lomba dari file.
 */
StorageError agent_storage_load(const StorageAggregate *agg,
                                const char *filename,
                                CompetitionState *state) {
    LOG_INFO("storage_load: memulai pemuatan");
    /* Jangan jalankan bila penghubung belum siap. */
    if (agg == NULL || agg->protocol == NULL) {
        LOG_ERROR("storage_load: aggregate/protocol NULL");
        return ST_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->load == NULL) {
        LOG_ERROR("storage_load: fungsi load NULL");
        return ST_ERROR_FILE_NOT_FOUND;
    }
    StorageError result = agg->protocol->load(filename, state);
    if (result == ST_OK)
        LOG_INFO("storage_load: berhasil");
    else
        LOG_ERROR("storage_load: gagal");
    return result;
}

StorageError agent_storage_delete(const StorageAggregate *agg,
                                  const char *filename) {
    LOG_INFO("storage_delete: memulai penghapusan");
    if (agg == NULL || agg->protocol == NULL) {
        LOG_ERROR("storage_delete: aggregate/protocol NULL");
        return ST_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->delete_file == NULL) {
        LOG_ERROR("storage_delete: fungsi delete_file NULL");
        return ST_ERROR_FILE_NOT_FOUND;
    }
    StorageError result = agg->protocol->delete_file(filename);
    if (result == ST_OK)
        LOG_INFO("storage_delete: berhasil");
    else
        LOG_ERROR("storage_delete: gagal");
    return result;
}
