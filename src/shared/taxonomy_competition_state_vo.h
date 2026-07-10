#ifndef SHARED_TAXONOMY_COMPETITION_STATE_VO_H
#define SHARED_TAXONOMY_COMPETITION_STATE_VO_H

#include "taxonomy_participant_entity.h"
#include "taxonomy_game_constant.h"

typedef enum {
    STATE_INIT = 0,
    STATE_REGISTERED = 1,
    STATE_COMPLETED = 2
} CompetitionStateKind;

typedef struct {
    ParticipantEntity participants[MAX_PARTICIPANTS];
    int participant_count;
    CompetitionStateKind state;
} CompetitionState;

#endif /* SHARED_TAXONOMY_COMPETITION_STATE_VO_H */
