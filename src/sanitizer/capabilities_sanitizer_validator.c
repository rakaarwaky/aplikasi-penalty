/**
 * @file capabilities_sanitizer_validator.c
 * @brief Sanitizer: validasi input string & integer (murni, tanpa I/O terminal).
 *
 * Catatan: file ini berada di folder sanitizer/ dengan awalan
 * capabilities_ (bukan infrastructure_) — ia berisi logika validasi
 * murni; pemanggilan langsung (sanitizer_validate_*) juga disediakan
 * sebagai convenience wrapper di bawah.
 */

/* SANITIZER — Capabilities (Validation Logic) */
#include "module.sanitizer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* ──────────────────────────────────────────────
 * Validasi string: NULL, panjang > max, atau
 * karakter non-printable = tidak sah.
 * ────────────────────────────────────────────── */
static SanitizeError validate_string_impl(const char *input, size_t max_length) {
    if (input == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    size_t len = strlen(input);
    if (len > max_length) {
        return SANITIZE_ERROR_TOO_LONG;
    }
    for (size_t i = 0; i < len; i++) {
        if (!isprint((unsigned char)input[i])) {
            return SANITIZE_ERROR_INVALID_CHARS;
        }
    }
    return SANITIZE_OK;
}

/* ──────────────────────────────────────────────
 * Validasi integer dari string: pakai strtol,
 * cek sisa (endptr) harus '\0' (input murni
 * angka), lalu cek rentang [min_val, max_val].
 * ────────────────────────────────────────────── */
static SanitizeError validate_int_impl(const char *input, int min_val, int max_val) {
    if (input == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    char *endptr;
    long val = strtol(input, &endptr, 10);
    if (*endptr != '\0' || endptr == input) {
        return SANITIZE_ERROR_INVALID_CHARS;
    }
    if (val < min_val || val > max_val) {
        return SANITIZE_ERROR_TOO_LONG;
    }
    return SANITIZE_OK;
}

/**
 * Bangun SanitizeProtocol: sambungkan function-pointer ke impl di atas.
 */
SanitizeProtocol sanitizer_create(void) {
    SanitizeProtocol p;
    p.validate_string = validate_string_impl;
    p.validate_int = validate_int_impl;
    return p;
}

/* ──────────────────────────────────────────────
 * Convenience wrapper: pemanggilan langsung tanpa
 * harus merakit protocol. Didelegasikan ke impl.
 * ────────────────────────────────────────────── */
SanitizeError sanitizer_validate_string(const char *input, size_t max_length) {
    return validate_string_impl(input, max_length);
}

SanitizeError sanitizer_validate_int(const char *input, int min_val, int max_val) {
    return validate_int_impl(input, min_val, max_val);
}
