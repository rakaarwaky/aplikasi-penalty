/**
 * @file capabilities_registration_appender.c
 * @brief Tambah peserta baru ke data lomba dan isi nilai awalnya.
 */

#include "registration/module.registration.h"

#include <string.h>

/**
 * Daftarkan peserta baru ke dalam data lomba.
 *
 * @param state  Data lomba yang akan diubah (tambah peserta baru).
 * @param name   Nama peserta yang sudah tervalidasi.
 * @return       REG_OK bila berhasil, REG_FULL bila kuota penuh,
 *               REG_NAME_EMPTY bila parameter tidak sah.
 */
RegistrationError capabilities_registration_append(CompetitionState *state,
                                                   const ParticipantNameVO *name) {
    /* Parameter wajib tidak boleh kosong. */
    if (state == NULL || name == NULL) return REG_NAME_EMPTY;

    /* Kuota peserta sudah penuh. */
    if (state->participant_count >= MAX_PARTICIPANTS) return REG_FULL;

    /* Ambil slot berikutnya (indeks = jumlah peserta saat ini). */
    int slot = state->participant_count;

    /* Isi data awal peserta baru. */
    state->participants[slot].id.value = slot;
    memcpy(state->participants[slot].name.value,
           name->value, MAX_NAME_LENGTH);
    state->participants[slot].name.value[MAX_NAME_LENGTH] = '\0'; /* jamin string berakhir null */
    state->participants[slot].total_score.value = 0;
    state->participants[slot].kick_count.value = 0;
    for (int z = 0; z <= MAX_ZONE; z++) state->participants[slot].zone_freq.freq[z] = 0; /* reset frekuensi zona */
    for (int k = 0; k < TOTAL_KICKS; k++) {
        state->participants[slot].kicks[k].zone = -1;   /* -1 = tendangan belum dilakukan */
        state->participants[slot].kicks[k].points = 0;
    }

    /* Catat jumlah peserta yang bertambah. */
    state->participant_count++;

    /* Bila sudah cukup peserta, tandai pendaftaran selesai. */
    if (state->state == STATE_INIT && state->participant_count >= MIN_PARTICIPANTS)
        state->state = STATE_REGISTERED;

    return REG_OK;
}
