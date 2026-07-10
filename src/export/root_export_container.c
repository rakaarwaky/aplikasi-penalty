/**
 * @file root_export_container.c
 * @brief Root container: rakit ExportAggregate (wiring protocol ke infrastructure).
 */

/* EXPORT — Root Container (wiring) */
#include "export/module.export.h"

/**
 * Bangun ExportAggregate.
 * Protocol di-buat via export_adapter_create() (infrastructure),
 * disimpan di `static` agar alamatnya stabil untuk aggregate.
 *
 * @return ExportAggregate — berisi pointer protocol yang terisi.
 */
ExportAggregate root_export_build(void) {
    ExportAggregate agg;
    static ExportProtocol proto;
    proto = export_adapter_create();
    agg.protocol = &proto;
    return agg;
}
