/**
 * @file agent_export_orchestrator.c
 * @brief Ekspor hasil ke file melalui fungsi penulisan.
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

/**
 * Tulis rekapitulasi lengkap ke file.
 */
ExportError agent_export_recap(const ExportAggregate *agg,
                               const char *filename,
                               const CompetitionState *state,
                               const RankingEntryVO *ranking,
                               const SearchResultVO *details,
                               int count,
                               int total_score,
                               int avg_score,
                               int highest_score) {
    LOG_INFO("export_recap: memulai ekspor");
    if (agg == NULL || agg->protocol == NULL) {
        LOG_ERROR("export_recap: aggregate/protocol NULL");
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->export_recap == NULL) {
        LOG_ERROR("export_recap: fungsi export_recap NULL");
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    ExportError result = agg->protocol->export_recap(filename, state, ranking,
                                                     details, count, total_score,
                                                     avg_score, highest_score);
    if (result == EXP_OK)
        LOG_INFO("export_recap: berhasil");
    else
        LOG_ERROR("export_recap: gagal");
    return result;
}

/**
 * Tulis data satu peserta ke file.
 */
ExportError agent_export_participant(const ExportAggregate *agg,
                                     const char *filename,
                                     const SearchResultVO *participant) {
    LOG_INFO("export_participant: memulai ekspor");
    if (agg == NULL || agg->protocol == NULL) {
        LOG_ERROR("export_participant: aggregate/protocol NULL");
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->export_participant == NULL) {
        LOG_ERROR("export_participant: fungsi export_participant NULL");
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    ExportError result = agg->protocol->export_participant(filename, participant);
    if (result == EXP_OK)
        LOG_INFO("export_participant: berhasil");
    else
        LOG_ERROR("export_participant: gagal");
    return result;
}
