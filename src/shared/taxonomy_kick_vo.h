/**
 * @file taxonomy_kick_vo.h
 * @brief Satu tendangan: zona & poin (sama nilainya menurut aturan lomba).
 */

#ifndef SHARED_TAXONOMY_KICK_VO_H
#define SHARED_TAXONOMY_KICK_VO_H

/** Hasil satu tendangan. Zona 0 = 0 poin, zona 5 = 5 poin (PRD 2.2). */
typedef struct {
    int zone;    /**< Zona tendangan (0-5). */
    int points;  /**< Poin yang diperoleh (sama dengan zone). */
} KickVO;

#endif /* SHARED_TAXONOMY_KICK_VO_H */
