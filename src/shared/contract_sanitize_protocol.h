#ifndef CONTRACT_SANITIZE_PROTOCOL_H
#define CONTRACT_SANITIZE_PROTOCOL_H

/* Inbound protocol untuk validasi/sanitasi input pengguna. */

#include "shared/taxonomy_sanitize_error.h"
#include <stddef.h>

/**
 * Validasi string input.
 * @param input      String input dari pengguna (bisa NULL).
 * @param max_length Panjang maksimum yang diizinkan.
 * @return           SanitizeError — SANITIZE_OK jika valid.
 */
typedef SanitizeError (*validate_string_fn)(const char *input, size_t max_length);

/**
 * Validasi integer dari string.
 * @param input   String yang akan di-parse ke integer.
 * @param min_val Nilai minimum yang diizinkan.
 * @param max_val Nilai maksimum yang diizinkan.
 * @return        SanitizeError — SANITIZE_OK jika valid.
 */
typedef SanitizeError (*validate_int_fn)(const char *input, int min_val, int max_val);

/** Protocol sanitasi. */
typedef struct {
    validate_string_fn validate_string;  /**< Validasi string input. */
    validate_int_fn validate_int;        /**< Validasi integer dari string. */
} SanitizeProtocol;

#endif /* CONTRACT_SANITIZE_PROTOCOL_H */
