/**
 * @file capabilities_sanitizer_validator.c
 * @brief Periksa keabsahan input teks & angka dari pengguna.
 */

/* SANITIZER — Capabilities (Validation Logic) */
#include "module.sanitizer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* Cek string: tidak kosong, tidak terlalu panjang, hanya karakter tercetak. */
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

/* Cek angka: string harus angka murni dan dalam rentang [min,max]. */
static SanitizeError validate_int_impl(const char *input, int min_val, int max_val) {
    if (input == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    char *endptr;
    long val = strtol(input, &endptr, 10);
    if (*endptr != '\0' || endptr == input) {
        return SANITIZE_ERROR_INVALID_CHARS; /* bukan angka */
    }
    if (val < min_val || val > max_val) {
        return SANITIZE_ERROR_TOO_LONG; /* di luar rentang */
    }
    return SANITIZE_OK;
}

/**
 * Siapkan struct sanitasi: sambungkan fungsi cek string & angka.
 */
SanitizeProtocol sanitizer_create(void) {
    SanitizeProtocol p;
    p.validate_string = validate_string_impl;
    p.validate_int = validate_int_impl;
    return p;
}

/* Cek string secara langsung (tanpa merakit struct). */
SanitizeError sanitizer_validate_string(const char *input, size_t max_length) {
    return validate_string_impl(input, max_length);
}

/* Cek angka secara langsung (tanpa merakit struct). */
SanitizeError sanitizer_validate_int(const char *input, int min_val, int max_val) {
    return validate_int_impl(input, min_val, max_val);
}
