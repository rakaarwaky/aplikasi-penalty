#ifndef SHARED_TAXONOMY_COMPETITION_STATE_VO_H
#define SHARED_TAXONOMY_COMPETITION_STATE_VO_H

/* State kompetisi — di-pass via pointer ke semua layer, tanpa global. */

#include <stddef.h> /* NULL, size_t untuk pengguna hilir */

#include "shared/taxonomy_participant_entity.h"
#include "shared/taxonomy_game_constant.h"

/** State machine kompetisi. */
typedef enum {
    STATE_INIT = 0,       /**< Awal aplikasi, belum ada peserta. */
    STATE_REGISTERED = 1, /**< Pendaftaran selesai, siap scoring. */
    STATE_COMPLETED = 2   /**< Semua tendangan selesai, siap ranking/recap. */
} CompetitionStateKind;

/**
 * State kompetisi — di-pass via pointer ke semua layer (tanpa global).
 * Alokasi: stack di root_cli_main_entry.c, pointer ke setiap fungsi.
 */
typedef struct {
    ParticipantEntity participants[MAX_PARTICIPANTS]; /**< Array peserta (max MAX_PARTICIPANTS). */
    int participant_count;                             /**< Jumlah peserta terdaftar. */
    CompetitionStateKind state;                        /**< State machine saat ini. */
} CompetitionState;

#endif /* SHARED_TAXONOMY_COMPETITION_STATE_VO_H */
