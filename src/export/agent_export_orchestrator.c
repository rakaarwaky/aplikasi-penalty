/**
 * @file agent_export_orchestrator.c
 * @brief Ekspor hasil peringkat ke file melalui fungsi penulisan.
 */

/* EXPORT — Agent Orkestrator */
#include "module.export.h"
#include "shared/taxonomy_logger.h"

/**
 * Tulis hasil peringkat ke file.
 */
ExportError agent_export_ranking(const ExportAggregate *agg,
                                 const char *filename,
                                 const CompetitionState *state,
                                 const RankingEntryVO *entries,
                                 int count) {
    LOG_INFO("export_ranking: memulai ekspor");
    /* Jangan jalankan bila penghubung belum siap. */
    if (agg == NULL || agg->protocol == NULL) {
        LOG_ERROR("export_ranking: aggregate/protocol NULL");
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->export_ranking == NULL) {
        LOG_ERROR("export_ranking: fungsi export_ranking NULL");
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    ExportError result = agg->protocol->export_ranking(filename, state, entries, count);
    if (result == EXP_OK)
        LOG_INFO("export_ranking: berhasil");
    else
        LOG_ERROR("export_ranking: gagal");
    return result;
}
