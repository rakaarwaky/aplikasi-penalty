/**
 * @file root_storage_container.c
 * @brief Root container: rakit StorageAggregate (wiring protocol ke infrastructure).
 */

/* STORAGE — Root Container (wiring) */
#include "storage/module.storage.h"

/**
 * Bangun StorageAggregate.
 * Protocol di-buat via storage_adapter_create() (infrastructure),
 * disimpan di `static` agar alamatnya stabil untuk aggregate.
 *
 * @return StorageAggregate — berisi pointer protocol yang terisi.
 */
StorageAggregate root_storage_build(void) {
    StorageAggregate agg;
    static StorageProtocol proto;
    proto = storage_adapter_create();
    agg.protocol = &proto;
    return agg;
}
