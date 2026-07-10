/*
 * infrastructure_storage_adapter.c
 * Baca/tulis data lomba dari/ke file biner.
 */

#include "module.storage.h"
#include <stdio.h>
#include <string.h>

static StorageError storage_save_impl(const char *filename, const CompetitionState *state) {
    if (filename == NULL || state == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        return ST_ERROR_PERMISSION;
    }

    size_t written = fwrite(state, sizeof(CompetitionState), 1, file);
    if (written != 1 || fclose(file) != 0) {
        return ST_ERROR_CORRUPT;
    }

    return ST_OK;
}

static StorageError storage_load_impl(const char *filename, CompetitionState *state) {
    if (filename == NULL || state == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }

    size_t read_count = fread(state, sizeof(CompetitionState), 1, file);
    int io_err = ferror(file);
    fclose(file);

    if (read_count != 1 || io_err != 0) {
        return ST_ERROR_CORRUPT;
    }

    return ST_OK;
}

static StorageError storage_delete_impl(const char *filename) {
    if (filename == NULL) return ST_ERROR_FILE_NOT_FOUND;

    FILE *check = fopen(filename, "rb");
    if (check == NULL) return ST_ERROR_FILE_NOT_FOUND;
    fclose(check);

    if (remove(filename) != 0) return ST_ERROR_PERMISSION;
    return ST_OK;
}

StorageProtocol storage_adapter_create(void) {
    StorageProtocol protocol;
    protocol.save = storage_save_impl;
    protocol.load = storage_load_impl;
    protocol.delete_file = storage_delete_impl;
    return protocol;
}
