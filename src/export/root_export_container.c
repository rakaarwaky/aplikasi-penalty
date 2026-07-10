/**
 * @file root_export_container.c
 * @brief Siapkan struct ekspor dan sambungkan ke fungsi tulis file.
 */

/* EXPORT — Root Container (wiring) */
#include "export/module.export.h"

/**
 * Bangun struct ekspor: isi dengan fungsi tulis file teks
 * agar siap dipakai seluruh program.
 *
 * @return ExportAggregate yang sudah terisi.
 */
ExportAggregate root_export_build(void) {
    ExportAggregate agg;
    static ExportProtocol proto;
    proto = export_adapter_create();
    agg.protocol = &proto;
    return agg;
}
