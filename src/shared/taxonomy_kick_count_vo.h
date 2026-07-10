/**
 * @file taxonomy_kick_count_vo.h
 * @brief Pembungkus jumlah tendangan yang sudah dilakukan peserta.
 */

#ifndef SHARED_TAXONOMY_KICK_COUNT_VO_H
#define SHARED_TAXONOMY_KICK_COUNT_VO_H

/** Berapa tendangan yang sudah dilakukan (0..TOTAL_KICKS). */
typedef struct {
    int value;  /**< Jumlah tendangan tercatat (0 sampai TOTAL_KICKS). */
} KickCountVO;

#endif /* SHARED_TAXONOMY_KICK_COUNT_VO_H */
