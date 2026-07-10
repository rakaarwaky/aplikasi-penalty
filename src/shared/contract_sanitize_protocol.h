/**
 * @file contract_sanitize_protocol.h
 * @brief Daftar fungsi yang harus ada untuk periksa input pengguna.
 */

#ifndef CONTRACT_SANITIZE_PROTOCOL_H
#define CONTRACT_SANITIZE_PROTOCOL_H

#include "shared/taxonomy_sanitize_error.h"
#include <stddef.h>

/* Tipe fungsi: periksa string input (panjang & karakter). */
typedef SanitizeError (*validate_string_fn)(const char *input, size_t max_length);

/* Tipe fungsi: periksa string bisa dijadikan angka dalam rentang. */
typedef SanitizeError (*validate_int_fn)(const char *input, int min_val, int max_val);

/** Kumpulan fungsi sanitasi — diisi oleh root container. */
typedef struct {
    validate_string_fn validate_string;  /**< Cek string. */
    validate_int_fn validate_int;        /**< Cek angka. */
} SanitizeProtocol;

#endif /* CONTRACT_SANITIZE_PROTOCOL_H */
