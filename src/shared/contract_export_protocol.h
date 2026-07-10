#ifndef SHARED_CONTRACT_EXPORT_PROTOCOL_H
#define SHARED_CONTRACT_EXPORT_PROTOCOL_H

/* Inbound protocol untuk ekspor file hasil ranking/recap. */

#include "shared/taxonomy_export_error.h"
#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_rank_vo.h"
#include <stddef.h>

/**
 * Tulis hasil ranking ke file.
 * @param filename  Nama file target (path lengkap).
 * @param state     Pointer ke state kompetisi (read-only).
 * @param entries   Array RankingEntryVO yang akan ditulis.
 * @param count     Jumlah entry dalam array.
 * @return          ExportError — EXP_OK jika berhasil.
 */
typedef ExportError (*export_ranking_fn)(const char *filename,
                                         const CompetitionState *state,
                                         const RankingEntryVO *entries,
                                         int count);

/** Protocol export. */
typedef struct {
    export_ranking_fn export_ranking;  /**< Tulis ranking ke file. */
} ExportProtocol;

#endif /* SHARED_CONTRACT_EXPORT_PROTOCOL_H */
