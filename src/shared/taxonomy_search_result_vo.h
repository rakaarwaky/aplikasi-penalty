/**
 * @file taxonomy_search_result_vo.h
 * @brief Hasil pencarian peserta (dipakai layar cari).
 */

#ifndef SHARED_TAXONOMY_SEARCH_RESULT_VO_H
#define SHARED_TAXONOMY_SEARCH_RESULT_VO_H

#include "shared/taxonomy_game_constant.h"

/** Isi balikan cari peserta: apa ketemu & data lengkapnya bila ya. */
typedef struct {
    int found;                            /**< 1 = ketemu, 0 = tidak. */
    int participant_id;                   /**< Nomor peserta (bila found == 1). */
    char name[MAX_NAME_LENGTH + 1];       /**< Salinan nama peserta. */
    int total_score;                      /**< Skor total peserta. */
    int kicks[TOTAL_KICKS];              /**< Riwayat 7 tendangannya. */
    int zone_freq[MAX_ZONE + 1];         /**< Frekuensi zona 0..5. */
} SearchResultVO;

#endif /* SHARED_TAXONOMY_SEARCH_RESULT_VO_H */
