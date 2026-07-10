/**
 * @file taxonomy_participant_id_vo.h
 * @brief Pembungkus nomor urut peserta (indeks dalam array data).
 */

#ifndef SHARED_TAXONOMY_PARTICIPANT_ID_VO_H
#define SHARED_TAXONOMY_PARTICIPANT_ID_VO_H

/** Nomor urut peserta sebagai tipe sendiri agar tidak tertukar dengan skor/zona. */
typedef struct {
    int value;  /**< Indeks peserta dalam array CompetitionState. */
} ParticipantIdVO;

#endif /* SHARED_TAXONOMY_PARTICIPANT_ID_VO_H */
