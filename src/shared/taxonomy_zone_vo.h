/**
 * @file taxonomy_zone_vo.h
 * @brief Pembungkus angka zona supaya tidak tertukar dengan id/poin.
 */

#ifndef SHARED_TAXONOMY_ZONE_VO_H
#define SHARED_TAXONOMY_ZONE_VO_H

/** Zona tendangan sebagai tipe sendiri agar salah kirim parameter ketahuan saat kompilasi. */
typedef struct {
    int value;  /**< Nilai zona 0..5 (0 = miss). */
} ZoneVO;

#endif /* SHARED_TAXONOMY_ZONE_VO_H */
