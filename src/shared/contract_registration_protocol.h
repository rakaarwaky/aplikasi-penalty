#ifndef SHARED_CONTRACT_REGISTRATION_PROTOCOL_H
#define SHARED_CONTRACT_REGISTRATION_PROTOCOL_H

/* Inbound protocol untuk pendaftaran peserta. */

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_competition_error.h"

/**
 * Validasi nama peserta sebelum registrasi.
 * @param state  Pointer ke state kompetisi (untuk cek duplikat/kapasitas).
 * @param name   Pointer ke VO nama yang akan divalidasi.
 * @return       RegistrationError — REG_OK jika valid.
 */
typedef RegistrationError (*validate_name_fn)(const CompetitionState *state,
                                              const ParticipantNameVO *name);

/**
 * Tambahkan peserta baru ke state kompetisi.
 * @param state  Pointer ke state kompetisi (akan dimodifikasi).
 * @param name   Pointer ke VO nama peserta baru.
 * @return       RegistrationError — REG_OK jika berhasil.
 */
typedef RegistrationError (*append_participant_fn)(CompetitionState *state,
                                                   const ParticipantNameVO *name);

/** Protocol pendaftaran. */
typedef struct {
    validate_name_fn validate_name;          /**< Validasi nama sebelum registrasi. */
    append_participant_fn append_participant; /**< Tambah peserta ke state. */
} RegistrationProtocol;

#endif /* SHARED_CONTRACT_REGISTRATION_PROTOCOL_H */
