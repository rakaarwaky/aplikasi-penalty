#ifndef SHARED_TAXONOMY_PARTICIPANT_ENTITY_H
#define SHARED_TAXONOMY_PARTICIPANT_ENTITY_H

#include "taxonomy_participant_vo.h"
#include "taxonomy_game_constant.h"

typedef struct {
    int id;
    ParticipantNameVO name;
    int kicks[TOTAL_KICKS];
    int total_score;
    int zone_freq[MAX_ZONE + 1];
    int kick_count;
} ParticipantEntity;

#endif /* SHARED_TAXONOMY_PARTICIPANT_ENTITY_H */
