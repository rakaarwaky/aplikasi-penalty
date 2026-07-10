/**
 * @file contract_export_protocol.h
 * @brief Daftar fungsi yang harus ada untuk ekspor hasil peringkat ke file.
 */

#ifndef SHARED_CONTRACT_EXPORT_PROTOCOL_H
#define SHARED_CONTRACT_EXPORT_PROTOCOL_H

#include "shared/taxonomy_export_error.h"
#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_rank_vo.h"
#include <stddef.h>

/* Tipe fungsi: tulis hasil peringkat peserta ke file. */
typedef ExportError (*export_ranking_fn)(const char *filename,
                                         const CompetitionState *state,
                                         const RankingEntryVO *entries,
                                         int count);

/** Kumpulan fungsi ekspor — diisi oleh root container. */
typedef struct {
    export_ranking_fn export_ranking;  /**< Tulis peringkat ke file. */
} ExportProtocol;

#endif /* SHARED_CONTRACT_EXPORT_PROTOCOL_H */
