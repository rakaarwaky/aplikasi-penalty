/**
 * @file agent_export_orchestrator.c
 * @brief Ekspor hasil peringkat ke file melalui fungsi penulisan.
 */

/* EXPORT — Agent Orkestrator */
#include "module.export.h"

/**
 * Tulis hasil peringkat ke file.
 */
ExportError agent_export_ranking(const ExportAggregate *agg,
                                 const char *filename,
                                 const CompetitionState *state,
                                 const RankingEntryVO *entries,
                                 int count) {
    /* Jangan jalankan bila penghubung belum siap. */
    if (agg == NULL || agg->protocol == NULL) {
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->export_ranking == NULL) {
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    return agg->protocol->export_ranking(filename, state, entries, count);
}
