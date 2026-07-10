/**
 * @file contract_registration_protocol.h
 * @brief Daftar fungsi yang harus ada untuk fitur pendaftaran.
 */

#ifndef SHARED_CONTRACT_REGISTRATION_PROTOCOL_H
#define SHARED_CONTRACT_REGISTRATION_PROTOCOL_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_competition_error.h"

/* Tipe fungsi: periksa sah/tidaknya nama sebelum didaftarkan. */
typedef RegistrationError (*validate_name_fn)(const CompetitionState *state,
                                              const ParticipantNameVO *name);

/* Tipe fungsi: tambah peserta baru ke data lomba. */
typedef RegistrationError (*append_participant_fn)(CompetitionState *state,
                                                   const ParticipantNameVO *name);

/** Kumpulan fungsi pendaftaran — diisi oleh root container. */
typedef struct {
    validate_name_fn validate_name;          /**< Cek nama. */
    append_participant_fn append_participant; /**< Tambah peserta. */
} RegistrationProtocol;

#endif /* SHARED_CONTRACT_REGISTRATION_PROTOCOL_H */
