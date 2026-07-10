#ifndef SHARED_TAXONOMY_PARTICIPANT_ENTITY_H
#define SHARED_TAXONOMY_PARTICIPANT_ENTITY_H

#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_game_constant.h"

/* AES401: entity memakai VO (ParticipantNameVO), bukan primitif nama. */
typedef struct {
    int id;
    ParticipantNameVO name;
    int kicks[TOTAL_KICKS];
    int total_score;
    int zone_freq[MAX_ZONE + 1];
    int kick_count;
} ParticipantEntity;

#endif /* SHARED_TAXONOMY_PARTICIPANT_ENTITY_H */
