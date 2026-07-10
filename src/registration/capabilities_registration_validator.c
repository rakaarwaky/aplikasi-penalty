/**
 * @file capabilities_registration_validator.c
 * @brief Cek validitas nama peserta sebelum didaftarkan.
 */

#include "registration/module.registration.h"

#include <string.h>
#include <ctype.h>

/* Bandingkan dua string tanpa peduli huruf besar/kecil. */
static int ci_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

/**
 * Periksa apakah nama peserta sah untuk didaftarkan.
 *
 * Urutan cek: kosong -> terlalu panjang -> ada karakter tak sah ->
 * tidak ada huruf sama sekali -> nama sudah dipakai peserta lain.
 *
 * @param state  Data lomba (hanya dibaca, untuk cek duplikat nama).
 * @param name   Nama yang akan diperiksa.
 * @return       REG_OK bila sah, atau kode error yang menjelaskan penyebabnya.
 */
RegistrationError capabilities_registration_validate_name(const CompetitionState *state,
                                                          const ParticipantNameVO *name) {
    /* Nama belum diisi sama sekali. */
    if (name == NULL || name->value[0] == '\0') return REG_NAME_EMPTY;

    /* Melebihi batas panjang maksimum nama (safe: tanpa strlen). */
    size_t len = 0;
    while (len <= (size_t)MAX_NAME_LENGTH && name->value[len] != '\0') {
        len++;
    }
    if (len > (size_t)MAX_NAME_LENGTH) return REG_NAME_TOO_LONG;

    /* Tiap huruf harus alfabet atau spasi; butuh minimal satu huruf. */
    int has_letter = 0;
    for (size_t i = 0; i < len; i++) {
        char c = name->value[i];
        if (isalpha((unsigned char)c)) has_letter = 1;
        else if (c != ' ') return REG_NAME_INVALID_CHAR;
    }
    if (!has_letter) return REG_NAME_EMPTY;

    /* Cek apakah nama sudah dipakai peserta yang terdaftar. */
    if (state != NULL) {
        for (int i = 0; i < state->participant_count; i++) {
            if (ci_equal(state->participants[i].name.value, name->value))
                return REG_NAME_DUPLICATE;
        }
    }
    return REG_OK;
}
