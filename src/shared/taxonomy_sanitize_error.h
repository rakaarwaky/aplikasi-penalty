/**
 * @file taxonomy_sanitize_error.h
 * @brief Kode error pemeriksaan input teks/angka dari pengguna.
 */

#ifndef TAXONOMY_SANITIZE_ERROR_H
#define TAXONOMY_SANITIZE_ERROR_H

/** Hasil validasi input pengguna. */
typedef enum {
    SANITIZE_OK,                   /**< Input sah. */
    SANITIZE_ERROR_NULL_INPUT,     /**< Input kosong/null. */
    SANITIZE_ERROR_TOO_LONG,       /**< Melebihi panjang maksimum string. */
    SANITIZE_ERROR_INVALID_CHARS,  /**< Ada karakter tak sah. */
    SANITIZE_ERROR_OUT_OF_RANGE    /**< Angka di luar rentang [min,max]. */
} SanitizeError;

#endif /* TAXONOMY_SANITIZE_ERROR_H */
