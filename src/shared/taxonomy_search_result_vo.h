#ifndef SHARED_TAXONOMY_SEARCH_RESULT_VO_H
#define SHARED_TAXONOMY_SEARCH_RESULT_VO_H

/* VO hasil pencarian peserta — dikembalikan oleh find_participant(). */

#include "shared/taxonomy_game_constant.h"

/** VO hasil pencarian peserta. */
typedef struct {
    int found;                            /**< 1 jika peserta ditemukan, 0 jika tidak. */
    int participant_id;                   /**< ID peserta (berlaku jika found == 1). */
    char name[MAX_NAME_LENGTH + 1];       /**< Nama peserta (salinan). */
    int total_score;                      /**< Total skor peserta. */
    int kicks[TOTAL_KICKS];              /**< Array hasil tendangan. */
    int zone_freq[MAX_ZONE + 1];         /**< Frekuensi zona. */
} SearchResultVO;

#endif /* SHARED_TAXONOMY_SEARCH_RESULT_VO_H */
