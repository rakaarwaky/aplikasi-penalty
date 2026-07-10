#ifndef SHARED_TAXONOMY_PARTICIPANT_VO_H
#define SHARED_TAXONOMY_PARTICIPANT_VO_H

/* VO nama peserta — membungkus char array dengan batas MAX_NAME_LENGTH. */

#include "shared/taxonomy_game_constant.h"

/** VO nama peserta (AES402). */
typedef struct {
    char value[MAX_NAME_LENGTH + 1]; /**< Nama peserta + null-terminator. */
} ParticipantNameVO;

#endif /* SHARED_TAXONOMY_PARTICIPANT_VO_H */
