#ifndef SHARED_TAXONOMY_ZONE_VO_H
#define SHARED_TAXONOMY_ZONE_VO_H

/* VO zona tendangan — membungkus int menjadi tipe domain scoring. */

/** VO zona tendangan (batas domain scoring, AES402). */
typedef struct {
    int value;  /**< Nilai zona (0 = miss, 1-5 = zona target). */
} ZoneVO;

#endif /* SHARED_TAXONOMY_ZONE_VO_H */
