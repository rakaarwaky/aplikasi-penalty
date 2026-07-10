/**
 * @file infrastructure_storage_adapter.c
 * @brief Baca/tulis data lomba dari/ke file biner.
 */

/* STORAGE — Adapter Infrastruktur (file I/O) */
#include "module.storage.h"
#include <stdio.h>
#include <string.h>

/* Tulis seluruh struct data lomba ke file biner. */
static StorageError storage_save_impl(const char *filename, const CompetitionState *state) {
    if (filename == NULL || state == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        return ST_ERROR_PERMISSION; /* tidak bisa buka file */
    }

    size_t written = fwrite(state, sizeof(CompetitionState), 1, file);
    fclose(file);

    if (written != 1) {
        return ST_ERROR_CORRUPT; /* penulisan tidak penuh */
    }

    return ST_OK;
}

/* Baca file biner ke struct data lomba. */
static StorageError storage_load_impl(const char *filename, CompetitionState *state) {
    if (filename == NULL || state == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }

    size_t read_count = fread(state, sizeof(CompetitionState), 1, file);
    fclose(file);

    if (read_count != 1) {
        return ST_ERROR_CORRUPT; /* file rusak/terpotong */
    }

    return ST_OK;
}

/**
 * Siapkan struct penyimpan: sambungkan fungsi tulis & baca file.
 */
StorageProtocol storage_adapter_create(void) {
    StorageProtocol protocol;
    protocol.save = storage_save_impl;
    protocol.load = storage_load_impl;
    return protocol;
}
