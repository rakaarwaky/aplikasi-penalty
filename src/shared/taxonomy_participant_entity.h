/**
 * @file taxonomy_participant_entity.h
 * @brief Satu peserta lengkap: identitas, riwayat tendangan, skor, & frekuensi zona.
 */

#ifndef SHARED_TAXONOMY_PARTICIPANT_ENTITY_H
#define SHARED_TAXONOMY_PARTICIPANT_ENTITY_H

#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_participant_id_vo.h"
#include "shared/taxonomy_kick_vo.h"
#include "shared/taxonomy_total_score_vo.h"
#include "shared/taxonomy_zone_freq_vo.h"
#include "shared/taxonomy_kick_count_vo.h"
#include "shared/taxonomy_game_constant.h"

/** Data utuh satu peserta — dipakai di seluruh fitur sebagai unit peserta. */
typedef struct {
    ParticipantIdVO id;                       /**< Nomor urut peserta (indeks dalam array data). */
    ParticipantNameVO name;                   /**< Nama, dibungkus VO agar batas panjang terjaga. */
    KickVO kicks[TOTAL_KICKS];               /**< Hasil 7 tendangan; zone = -1 berarti belum dilakukan. */
    TotalScoreVO total_score;                 /**< Akumulasi poin dari seluruh tendangan. */
    ZoneFreqVO zone_freq;                     /**< Hitungan tiap zona — dipakai pemecah seri peringkat. */
    KickCountVO kick_count;                   /**< Berapa tendangan yang sudah dilakukan (0..7). */
} ParticipantEntity;

#endif /* SHARED_TAXONOMY_PARTICIPANT_ENTITY_H */
