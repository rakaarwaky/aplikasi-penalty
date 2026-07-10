#ifndef SHARED_TAXONOMY_RANK_VO_H
#define SHARED_TAXONOMY_RANK_VO_H

/* VO satu baris ranking — dipakai output ranking dan recap. */

#include "shared/taxonomy_game_constant.h"

/** Satu baris ranking (dipakai ranking + recap). */
typedef struct {
    int participant_id;              /**< ID peserta yang diranking. */
    int total_score;                 /**< Total skor peserta. */
    int zone_freq[MAX_ZONE + 1];    /**< Frekuensi zona (untuk tie-breaker). */
    int rank;                        /**< Posisi ranking (1 = terbaik). */
} RankingEntryVO;

#endif /* SHARED_TAXONOMY_RANK_VO_H */
