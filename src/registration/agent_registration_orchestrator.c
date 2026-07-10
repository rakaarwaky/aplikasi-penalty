/**
 * @file agent_registration_orchestrator.c
 * @brief Agent: orkestrasi pipeline pendaftaran (validasi lalu append), tanpa I/O.
 */

#include "registration/module.registration.h"

/**
 * Tambah peserta: validasi lalu append, melalui protocol aggregate.
 *
 * @param agg   Pointer ke RegistrationAggregate (berisi protocol terrakit).
 * @param state Pointer ke state kompetisi (dimodifikasi bila append berhasil).
 * @param name  Pointer ke VO nama peserta.
 * @return      RegistrationError — hasil validasi/append, atau REG_NAME_EMPTY bila NULL.
 */
RegistrationError agent_registration_add(RegistrationAggregate *agg,
                                         CompetitionState *state,
                                         const ParticipantNameVO *name) {
    /* Guard parameter: aggregate/state/name tak boleh NULL. */
    if (agg == NULL || state == NULL || name == NULL) return REG_NAME_EMPTY;

    /* Langkah 1: delegasikan validasi ke capability via protocol. */
    RegistrationError ve = agg->protocol->validate_name(state, name);
    if (ve != REG_OK) return ve; /* gagal validasi -> hentikan, teruskan error */

    /* Langkah 2: validasi lolos -> tambah peserta via protocol. */
    return agg->protocol->append_participant(state, name);
}
