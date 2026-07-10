/**
 * @file contract_ranking_protocol.h
 * @brief Daftar fungsi yang harus ada untuk hitung peringkat.
 */

#ifndef SHARED_CONTRACT_RANKING_PROTOCOL_H
#define SHARED_CONTRACT_RANKING_PROTOCOL_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_rank_vo.h"
#include "shared/taxonomy_competition_error.h"

/* Tipe fungsi: susun peringkat seluruh peserta dari data lomba. */
typedef RankingError (*compute_ranking_fn)(const CompetitionState *state,
                                           RankingEntryVO *out, int capacity);

/** Kumpulan fungsi ranking — diisi oleh root container. */
typedef struct {
    compute_ranking_fn compute_ranking;  /**< Hitung peringkat. */
} RankingProtocol;

#endif /* SHARED_CONTRACT_RANKING_PROTOCOL_H */
