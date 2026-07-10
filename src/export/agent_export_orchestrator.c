/**
 * @file agent_export_orchestrator.c
 * @brief Agent: orkestrasi ekspor ranking via protocol (tanpa I/O langsung).
 */

/* EXPORT — Agent Orchestrator */
#include "module.export.h"

/**
 * Ekspor hasil ranking ke file via protocol (delegate ke infrastructure).
 * Guard NULL agar tidak dereferensi pointer kosong.
 */
ExportError agent_export_ranking(const ExportAggregate *agg,
                                 const char *filename,
                                 const CompetitionState *state,
                                 const RankingEntryVO *entries,
                                 int count) {
    if (agg == NULL || agg->protocol == NULL) {
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->export_ranking == NULL) {
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    return agg->protocol->export_ranking(filename, state, entries, count);
}
