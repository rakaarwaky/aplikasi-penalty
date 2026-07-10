/**
 * @file taxonomy_participant_vo.h
 * @brief Pembungkus nama peserta agar lewat sebagai tipe domain, bukan char mentah.
 */

#ifndef SHARED_TAXONOMY_PARTICIPANT_VO_H
#define SHARED_TAXONOMY_PARTICIPANT_VO_H

#include "shared/taxonomy_game_constant.h"

/** Nama peserta sebagai tipe sendiri (bukan char* telanjang) supaya ukuran maksimum jelas. */
typedef struct {
    char value[MAX_NAME_LENGTH + 1]; /**< Isi nama + null-terminator. */
} ParticipantNameVO;

#endif /* SHARED_TAXONOMY_PARTICIPANT_VO_H */
