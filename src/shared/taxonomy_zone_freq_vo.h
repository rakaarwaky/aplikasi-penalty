/**
 * @file taxonomy_zone_freq_vo.h
 * @brief Pembungkus frekuensi zona tendangan peserta.
 */

#ifndef SHARED_TAXONOMY_ZONE_FREQ_VO_H
#define SHARED_TAXONOMY_ZONE_FREQ_VO_H

#include "shared/taxonomy_game_constant.h"

/** Frekuensi tiap zona (0..5) — dipakai pemecah seri peringkat. */
typedef struct {
    int freq[MAX_ZONE + 1];  /**< Hitungan tendangan per zona (indeks 0..5). */
} ZoneFreqVO;

#endif /* SHARED_TAXONOMY_ZONE_FREQ_VO_H */
