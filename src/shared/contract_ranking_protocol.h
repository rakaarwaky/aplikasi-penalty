#ifndef SHARED_CONTRACT_RANKING_PROTOCOL_H
#define SHARED_CONTRACT_RANKING_PROTOCOL_H

/* Inbound protocol untuk perhitungan ranking. */

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_rank_vo.h"
#include "shared/taxonomy_competition_error.h"

/**
 * Hitung ranking seluruh peserta.
 * @param state    Pointer ke state kompetisi (read-only).
 * @param out      Array RankingEntryVO untuk menampung hasil.
 * @param capacity Kapasitas maksimum array out.
 * @return         RankingError — RK_OK jika berhasil.
 */
typedef RankingError (*compute_ranking_fn)(const CompetitionState *state,
                                           RankingEntryVO *out, int capacity);

/** Protocol ranking. */
typedef struct {
    compute_ranking_fn compute_ranking;  /**< Hitung ranking dari state. */
} RankingProtocol;

#endif /* SHARED_CONTRACT_RANKING_PROTOCOL_H */
