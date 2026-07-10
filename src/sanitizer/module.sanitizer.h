/**
 * @file module.sanitizer.h
 * @brief Kumpulan fungsi periksa input pengguna (cari lewat satu file ini).
 */

#ifndef MODULE_SANITIZER_H
#define MODULE_SANITIZER_H

#include "shared/taxonomy_sanitize_error.h"
#include "shared/contract_sanitize_protocol.h"

/* Fungsi cek string & angka (diisi alamatnya saat rakit). */
SanitizeProtocol sanitizer_create(void);

/* Pemanggilan langsung tanpa merakit struct. */
SanitizeError sanitizer_validate_string(const char *input, size_t max_length);
SanitizeError sanitizer_validate_int(const char *input, int min_val, int max_val);

#endif /* MODULE_SANITIZER_H */
