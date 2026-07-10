/**
 * @file taxonomy_rank_vo.h
 * @brief Satu baris hasil peringkat peserta (untuk ranking & recap).
 */

#ifndef SHARED_TAXONOMY_RANK_VO_H
#define SHARED_TAXONOMY_RANK_VO_H

#include "shared/taxonomy_game_constant.h"

/** Hasil peringkat satu peserta — dipakai ranking & recap. */
typedef struct {
    int participant_id;              /**< Nomor peserta yang diranking (indeks data). */
    int total_score;                 /**< Skor total peserta ini. */
    int zone_freq[MAX_ZONE + 1];    /**< Frekuensi zona 0..5 — dipakai pemecah seri. */
    int rank;                        /**< Posisi (1 = terbaik, seri berbagi nomor). */
} RankingEntryVO;

#endif /* SHARED_TAXONOMY_RANK_VO_H */
