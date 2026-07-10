/**
 * @file taxonomy_total_score_vo.h
 * @brief Pembungkus total skor peserta agar lewat sebagai tipe domain.
 */

#ifndef SHARED_TAXONOMY_TOTAL_SCORE_VO_H
#define SHARED_TAXONOMY_TOTAL_SCORE_VO_H

/** Total akumulasi poin dari seluruh tendangan peserta. */
typedef struct {
    int value;  /**< Nilai total skor (0..35 = 7 tendangan x max 5 poin). */
} TotalScoreVO;

#endif /* SHARED_TAXONOMY_TOTAL_SCORE_VO_H */
