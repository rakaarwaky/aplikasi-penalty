/**
 * @file taxonomy_participant_entity.h
 * @brief Satu peserta lengkap: identitas, riwayat tendangan, skor, & frekuensi zona.
 */

#ifndef SHARED_TAXONOMY_PARTICIPANT_ENTITY_H
#define SHARED_TAXONOMY_PARTICIPANT_ENTITY_H

#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_game_constant.h"

/** Data utuh satu peserta — dipakai di seluruh fitur sebagai unit peserta. */
typedef struct {
    int id;                                   /**< Nomor urut peserta (indeks dalam array data). */
    ParticipantNameVO name;                   /**< Nama, dibungkus VO agar batas panjang terjaga. */
    int kicks[TOTAL_KICKS];                  /**< Hasil 7 tendangan; -1 berarti tendangan itu belum dilakukan. */
    int total_score;                          /**< Akumulasi poin dari seluruh tendangan. */
    int zone_freq[MAX_ZONE + 1];             /**< Hitungan tiap zona (indeks 0..5) — dipakai pemecah seri peringkat. */
    int kick_count;                           /**< Berapa tendangan yang sudah dilakukan (0..7). */
} ParticipantEntity;

#endif /* SHARED_TAXONOMY_PARTICIPANT_ENTITY_H */
