/**
 * @file agent_registration_orchestrator.c
 * @brief Urutkan proses pendaftaran: validasi dulu, lalu simpan.
 */

#include "registration/module.registration.h"

/**
 * Daftarkan peserta: pastikan nama sah, lalu simpan ke data lomba.
 *
 * @param agg   Penghubung ke fungsi cek & simpan peserta.
 * @param state Data lomba yang akan diubah.
 * @param name  Nama peserta.
 * @return      Hasil validasi/simpan, atau REG_NAME_EMPTY bila parameter kosong.
 */
RegistrationError agent_registration_add(RegistrationAggregate *agg,
                                         CompetitionState *state,
                                         const ParticipantNameVO *name) {
    /* Parameter wajib tidak boleh kosong. */
    if (agg == NULL || state == NULL || name == NULL) return REG_NAME_EMPTY;

    /* Langkah 1: periksa dulu apakah nama sah. */
    RegistrationError ve = agg->protocol->validate_name(state, name);
    if (ve != REG_OK) return ve; /* gagal validasi -> hentikan */

    /* Langkah 2: nama sah -> simpan peserta. */
    return agg->protocol->append_participant(state, name);
}
