/**
 * @file module.storage.h
 * @brief Kumpulan fungsi simpan/muat data lomba ke file (cari lewat satu file ini).
 */

#ifndef MODULE_STORAGE_H
#define MODULE_STORAGE_H

#include "shared/contract_storage_protocol.h"
#include "shared/taxonomy_storage_error.h"
#include "shared/taxonomy_competition_state_vo.h"

/* Nama file penyimpanan bawaan (lomba tersimpan di direktori kerja). */
#define STORAGE_DEFAULT_FILE "data_lomba.bin"

/* Fungsi baca/tulis file (diisi alamatnya saat rakit). */
StorageProtocol storage_adapter_create(void);

/* Penghubung ke fungsi penyimpanan. */
typedef struct {
    const StorageProtocol *protocol;
} StorageAggregate;

StorageError agent_storage_save(const StorageAggregate *agg,
                                const char *filename,
                                const CompetitionState *state);

StorageError agent_storage_load(const StorageAggregate *agg,
                                const char *filename,
                                CompetitionState *state);

/* Hapus file data tersimpan di disk. */
StorageError agent_storage_delete(const StorageAggregate *agg,
                                  const char *filename);

/* Siapkan struct penyimpanan. */
StorageAggregate root_storage_build(void);

#endif /* MODULE_STORAGE_H */
