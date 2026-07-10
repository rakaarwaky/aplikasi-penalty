/**
 * @file capabilities_registration_validator.c
 * @brief Capability: validasi nama peserta sebelum registrasi (murni, tanpa I/O).
 */

#include "registration/module.registration.h"

#include <string.h>
#include <ctype.h>

/* ──────────────────────────────────────────────
 * Helper lokal (static): perbandingan string
 * case-insensitive. Mengembalikan 1 jika sama.
 * Dipakai untuk deteksi duplikat nama peserta.
 * ────────────────────────────────────────────── */
static int ci_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

/**
 * Validasi nama peserta menurut aturan registrasi.
 *
 * Urutan pengecekan (short-circuit):
 *   1. NULL / string kosong            → REG_NAME_EMPTY
 *   2. Lebih dari MAX_NAME_LENGTH     → REG_NAME_TOO_LONG
 *   3. Karakter selain huruf/spasi    → REG_NAME_INVALID_CHAR
 *   4. Tidak ada satupun huruf        → REG_NAME_EMPTY
 *   5. Sudah terdaftar (case-insens)  → REG_NAME_DUPLICATE
 *
 * @param state  Pointer ke state kompetisi (const — hanya dibaca untuk cek duplikat).
 * @param name   Pointer ke VO nama yang akan divalidasi.
 * @return       RegistrationError — REG_OK jika seluruh aturan terpenuhi.
 */
RegistrationError capabilities_registration_validate_name(const CompetitionState *state,
                                                          const ParticipantNameVO *name) {
    /* 1. Nama NULL atau string kosong. */
    if (name == NULL || name->value[0] == '\0') return REG_NAME_EMPTY;

    /* 2. Panjang melebihi batas maksimum nama (tanpa strlen untuk hindari UB). */
    size_t len = 0;
    while (len <= (size_t)MAX_NAME_LENGTH && name->value[len] != '\0') {
        len++;
    }
    if (len > (size_t)MAX_NAME_LENGTH) return REG_NAME_TOO_LONG;

    /* 3 & 4. Scan karakter: hanya huruf/spasi sah; butuh >=1 huruf. */
    int has_letter = 0;
    for (size_t i = 0; i < len; i++) {
        char c = name->value[i];
        if (isalpha((unsigned char)c)) has_letter = 1;
        else if (c != ' ') return REG_NAME_INVALID_CHAR;
    }
    if (!has_letter) return REG_NAME_EMPTY;

    /* 5. Duplikat: bandingkan terhadap seluruh peserta terdaftar. */
    if (state != NULL) {
        for (int i = 0; i < state->participant_count; i++) {
            if (ci_equal(state->participants[i].name.value, name->value))
                return REG_NAME_DUPLICATE;
        }
    }
    return REG_OK;
}
