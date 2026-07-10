#ifndef SHARED_TAXONOMY_RANK_VO_H
#define SHARED_TAXONOMY_RANK_VO_H

#include "taxonomy_game_constant.h"

typedef struct {
    int participant_id;
    int total_score;
    int zone_freq[MAX_ZONE + 1];
    int rank;
} RankingEntryVO;

#endif /* SHARED_TAXONOMY_RANK_VO_H */
