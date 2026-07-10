/**
 * @file root_storage_container.c
 * @brief Siapkan struct penyimpanan dan sambungkan ke fungsi file.
 */

/* STORAGE — Root Container (wiring) */
#include "storage/module.storage.h"

/**
 * Bangun struct penyimpanan: isi dengan fungsi baca/tulis file
 * agar siap dipakai seluruh program.
 *
 * @return StorageAggregate yang sudah terisi.
 */
StorageAggregate root_storage_build(void) {
    StorageAggregate agg;
    static StorageProtocol proto;
    proto = storage_adapter_create();
    agg.protocol = &proto;
    return agg;
}
