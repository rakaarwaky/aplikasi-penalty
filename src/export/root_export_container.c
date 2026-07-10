/* EXPORT — Root Container (wiring) */
#include "module.export.h"

ExportAggregate root_export_build(void) {
    ExportAggregate agg;
    static ExportProtocol proto;
    proto = export_adapter_create();
    agg.protocol = &proto;
    return agg;
}
