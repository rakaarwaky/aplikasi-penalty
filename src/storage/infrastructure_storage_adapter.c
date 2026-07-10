/**
 * @file infrastructure_storage_adapter.c
 * @brief Infrastructure: implementasi StorageProtocol — I/O file biner state (AES404: port impl).
 */

/* STORAGE — Infrastructure Adapter (file I/O) */
#include "module.storage.h"
#include <stdio.h>
#include <string.h>

/* ──────────────────────────────────────────────
 * Implementasi konkret save: tulis seluruh struct
 * CompetitionState ke file biner (wb). Penulisan
 * 1 elemen berukuran sizeof(state) — cepat tapi
 * raw (tidak portabel antar arsitektur).
 * ────────────────────────────────────────────── */
static StorageError storage_save_impl(const char *filename, const CompetitionState *state) {
    if (filename == NULL || state == NULL) {
        return ST_ERROR_FILE_NOT_FOUND;
    }

    FILE *file = fopen(filename, "wb");
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

/* ──────────────────────────────────────────────
 * Implementasi konkret load: baca file biner ke
 * struct state. Cek read_count == 1 untuk memastikan
 * file tidak rusak/terpotong.
 * ────────────────────────────────────────────── */
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
        return ST_ERROR_CORRUPT;
    }

    return ST_OK;
}

/**
 * Bangun StorageProtocol dengan menyambungkan function-pointer
 * ke implementasi file I/O di atas. Dipanggil oleh root container.
 */
StorageProtocol storage_adapter_create(void) {
    StorageProtocol protocol;
    protocol.save = storage_save_impl;
    protocol.load = storage_load_impl;
    return protocol;
}
