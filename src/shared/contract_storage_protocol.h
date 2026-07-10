/**
 * @file contract_storage_protocol.h
 * @brief Daftar fungsi yang harus ada untuk simpan/muat data ke file.
 */

#ifndef SHARED_CONTRACT_STORAGE_PROTOCOL_H
#define SHARED_CONTRACT_STORAGE_PROTOCOL_H

#include "shared/taxonomy_storage_error.h"
#include "shared/taxonomy_competition_state_vo.h"

/* Tipe fungsi: tulis data lomba ke file. */
typedef StorageError (*save_fn)(const char *filename, const CompetitionState *state);

/* Tipe fungsi: baca data lomba dari file. */
typedef StorageError (*load_fn)(const char *filename, CompetitionState *state);

/* Tipe fungsi: hapus file data tersimpan di disk. */
typedef StorageError (*delete_fn)(const char *filename);

/** Kumpulan fungsi penyimpanan — diisi oleh root container. */
typedef struct {
    save_fn save;     /**< Simpan ke file. */
    load_fn load;     /**< Muat dari file. */
    delete_fn delete_file; /**< Hapus file tersimpan. */
} StorageProtocol;

#endif /* SHARED_CONTRACT_STORAGE_PROTOCOL_H */
