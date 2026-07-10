/**
 * @file module.export.h
 * @brief Kumpulan fungsi ekspor hasil peringkat ke file (cari lewat satu file ini).
 */

#ifndef MODULE_EXPORT_H
#define MODULE_EXPORT_H

#include "shared/taxonomy_export_error.h"
#include "shared/contract_export_protocol.h"

/* Fungsi tulis file (diisi alamatnya saat rakit). */
ExportProtocol export_adapter_create(void);

/* Penghubung ke fungsi ekspor. */
typedef struct {
    const ExportProtocol *protocol;
} ExportAggregate;

ExportError agent_export_ranking(const ExportAggregate *agg,
                                 const char *filename,
                                 const CompetitionState *state,
                                 const RankingEntryVO *entries,
                                 int count);

/* Siapkan struct ekspor. */
ExportAggregate root_export_build(void);

#endif /* MODULE_EXPORT_H */
