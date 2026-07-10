#ifndef SHARED_TAXONOMY_KICK_VO_H
#define SHARED_TAXONOMY_KICK_VO_H

/* VO untuk satu tendangan penalti: zona == poin (PRD 2.2). */

/** Satu tendangan: zona == poin (PRD 2.2). */
typedef struct {
    int zone;    /**< Zona tendangan (0-5). */
    int points;  /**< Poin yang diperoleh (sama dengan zone). */
} KickVO;

#endif /* SHARED_TAXONOMY_KICK_VO_H */
