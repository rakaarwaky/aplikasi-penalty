/* STORAGE — Root Container (wiring) */
#include "storage/module.storage.h"

StorageAggregate root_storage_build(void) {
    StorageAggregate agg;
    static StorageProtocol proto;
    proto = storage_adapter_create();
    agg.protocol = &proto;
    return agg;
}
