/**
 * @file taxonomy_competition_state_vo.h
 * @brief Satu-satunya wadah status lomba yang dikirim ke tiap fungsi.
 */

#ifndef SHARED_TAXONOMY_COMPETITION_STATE_VO_H
#define SHARED_TAXONOMY_COMPETITION_STATE_VO_H

#include <stddef.h> /* NULL, size_t */

#include "shared/taxonomy_participant_entity.h"
#include "shared/taxonomy_game_constant.h"

/** Tahap lomba — mengatur menu & fitur mana yang boleh dibuka. */
typedef enum {
    STATE_INIT = 0,       /**< Awal: belum ada peserta. Hanya pendaftaran yang boleh. */
    STATE_REGISTERED = 1, /**< Peserta cukup & terdaftar. Boleh input tendangan & cari. */
    STATE_COMPLETED = 2   /**< Semua peserta sudah 7 tendangan. Boleh ranking & recap. */
} CompetitionStateKind;

/** Status keseluruhan lomba. Disimpan di main() lalu di-pass via pointer (tanpa global). */
typedef struct {
    ParticipantEntity participants[MAX_PARTICIPANTS]; /**< Semua peserta (ukuran tetap MAX_PARTICIPANTS). */
    int participant_count;                             /**< Jumlah peserta yang benar-benar terdaftar. */
    CompetitionStateKind state;                        /**< Tahap lomba saat ini. */
} CompetitionState;

#endif /* SHARED_TAXONOMY_COMPETITION_STATE_VO_H */
