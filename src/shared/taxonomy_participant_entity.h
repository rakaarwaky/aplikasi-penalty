#ifndef SHARED_TAXONOMY_PARTICIPANT_ENTITY_H
#define SHARED_TAXONOMY_PARTICIPANT_ENTITY_H

/* Entity peserta — representasi lengkap satu peserta dalam kompetisi. */

#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_game_constant.h"

/** Entity peserta — AES401: field nama memakai VO, bukan char mentah. */
typedef struct {
    int id;                                   /**< ID unik peserta (indeks dalam array). */
    ParticipantNameVO name;                   /**< Nama peserta (VO). */
    int kicks[TOTAL_KICKS];                  /**< Hasil tendangan (zona per tendangan). */
    int total_score;                          /**< Akumulasi total skor. */
    int zone_freq[MAX_ZONE + 1];             /**< Frekuensi zona (indeks 0..5). */
    int kick_count;                           /**< Jumlah tendangan yang sudah dilakukan. */
} ParticipantEntity;

#endif /* SHARED_TAXONOMY_PARTICIPANT_ENTITY_H */
