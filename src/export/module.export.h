/**
 * @file module.export.h
 * @brief Kumpulan fungsi ekspor hasil ke file (cari lewat satu file ini).
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

ExportError agent_export_recap(const ExportAggregate *agg,
                               const char *filename,
                               const CompetitionState *state,
                               const RankingEntryVO *ranking,
                               const SearchResultVO *details,
                               int count,
                               int total_score,
                               int avg_score,
                               int highest_score);

ExportError agent_export_participant(const ExportAggregate *agg,
                                     const char *filename,
                                     const SearchResultVO *participant);

/* Siapkan struct ekspor. */
ExportAggregate root_export_build(void);

#endif /* MODULE_EXPORT_H */
