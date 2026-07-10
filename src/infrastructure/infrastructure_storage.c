/* INFRASTRUCTURE — Data Persistence Implementation */
#include "infrastructure/infrastructure_storage.h"
#include <stdio.h>
#include <string.h>

StorageError storage_init(Storage *storage, const char *filename) {
    if (storage == NULL || filename == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    
    strncpy(storage->filename, filename, sizeof(storage->filename) - 1);
    storage->filename[sizeof(storage->filename) - 1] = '\0';
    
    return ST_OK;
}

StorageError storage_save(const Storage *storage, const CompetitionState *state) {
    if (storage == NULL || state == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    
    FILE *file = fopen(storage->filename, "wb");
    if (file == NULL) {
        return ST_ERROR_PERMISSION;
    }
    
    size_t written = fwrite(state, sizeof(CompetitionState), 1, file);
    fclose(file);
    
    if (written != 1) {
        return ST_ERROR_CORRUPT;
    }
    
    return ST_OK;
}

StorageError storage_load(Storage *storage, CompetitionState *state) {
    if (storage == NULL || state == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    
    FILE *file = fopen(storage->filename, "rb");
    if (file == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }
    
    size_t read_count = fread(state, sizeof(CompetitionState), 1, file);
    fclose(file);
    
    if (read_count != 1) {
        return ST_ERROR_CORRUPT;
    }
    
    return ST_OK;
}
