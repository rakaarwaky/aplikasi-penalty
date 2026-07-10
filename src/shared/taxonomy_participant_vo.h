#ifndef SHARED_TAXONOMY_PARTICIPANT_VO_H
#define SHARED_TAXONOMY_PARTICIPANT_VO_H

#include "shared/taxonomy_game_constant.h"

/* VO nama peserta (AES402). */
typedef struct {
    char value[MAX_NAME_LENGTH + 1];
} ParticipantNameVO;

#endif /* SHARED_TAXONOMY_PARTICIPANT_VO_H */
