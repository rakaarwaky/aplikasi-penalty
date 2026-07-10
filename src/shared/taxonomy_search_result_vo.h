#ifndef SHARED_TAXONOMY_SEARCH_RESULT_VO_H
#define SHARED_TAXONOMY_SEARCH_RESULT_VO_H

#include "shared/taxonomy_game_constant.h"

typedef struct {
    int found;
    int participant_id;
    char name[MAX_NAME_LENGTH + 1];
    int total_score;
    int kicks[TOTAL_KICKS];
    int zone_freq[MAX_ZONE + 1];
} SearchResultVO;

#endif /* SHARED_TAXONOMY_SEARCH_RESULT_VO_H */
