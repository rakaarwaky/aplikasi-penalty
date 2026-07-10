#ifndef SHARED_CONTRACT_STORAGE_PROTOCOL_H
#define SHARED_CONTRACT_STORAGE_PROTOCOL_H

/* Inbound protocol untuk save/load state kompetisi ke file. */

#include "shared/taxonomy_storage_error.h"
#include "shared/taxonomy_competition_state_vo.h"

/**
 * Simpan state kompetisi ke file.
 * @param filename  Nama file target (path lengkap).
 * @param state     Pointer ke state kompetisi (read-only).
 * @return          StorageError — ST_OK jika berhasil.
 */
typedef StorageError (*save_fn)(const char *filename, const CompetitionState *state);

/**
 * Muat state kompetisi dari file.
 * @param filename  Nama file sumber (path lengkap).
 * @param state     Pointer ke state kompetisi (akan diisi dari file).
 * @return          StorageError — ST_OK jika berhasil.
 */
typedef StorageError (*load_fn)(const char *filename, CompetitionState *state);

/** Protocol storage. */
typedef struct {
    save_fn save;  /**< Simpan state ke file. */
    load_fn load;  /**< Muat state dari file. */
} StorageProtocol;

#endif /* SHARED_CONTRACT_STORAGE_PROTOCOL_H */
