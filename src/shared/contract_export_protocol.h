/**
 * @file contract_export_protocol.h
 * @brief Daftar fungsi yang harus ada untuk ekspor hasil ke file.
 */

#ifndef SHARED_CONTRACT_EXPORT_PROTOCOL_H
#define SHARED_CONTRACT_EXPORT_PROTOCOL_H

#include "shared/taxonomy_export_error.h"
#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_rank_vo.h"
#include "shared/taxonomy_search_result_vo.h"
#include <stddef.h>

/* Tipe fungsi: tulis hasil peringkat peserta ke file. */
typedef ExportError (*export_ranking_fn)(const char *filename,
                                         const CompetitionState *state,
                                         const RankingEntryVO *entries,
                                         int count);

/* Tipe fungsi: tulis rekapitulasi lengkap ke file. */
typedef ExportError (*export_recap_fn)(const char *filename,
                                       const CompetitionState *state,
                                       const RankingEntryVO *ranking,
                                       const SearchResultVO *details,
                                       int count,
                                       int total_score,
                                       int avg_score,
                                       int highest_score);

/* Tipe fungsi: tulis data satu peserta ke file. */
typedef ExportError (*export_participant_fn)(const char *filename,
                                             const SearchResultVO *participant);

/** Kumpulan fungsi ekspor — diisi oleh root container. */
typedef struct {
    export_ranking_fn export_ranking;        /**< Tulis peringkat ke file. */
    export_recap_fn export_recap;            /**< Tulis rekapitulasi ke file. */
    export_participant_fn export_participant; /**< Tulis data peserta ke file. */
} ExportProtocol;

#endif /* SHARED_CONTRACT_EXPORT_PROTOCOL_H */
