#ifndef TAXONOMY_SANITIZE_ERROR_H
#define TAXONOMY_SANITIZE_ERROR_H

/* Error enum untuk validasi/sanitasi input pengguna. */

/** Error enum untuk sanitasi input. */
typedef enum {
    SANITIZE_OK,                   /**< Input valid. */
    SANITIZE_ERROR_NULL_INPUT,     /**< Input adalah NULL. */
    SANITIZE_ERROR_TOO_LONG,       /**< Input melebihi panjang maksimum. */
    SANITIZE_ERROR_INVALID_CHARS   /**< Input mengandung karakter tidak valid. */
} SanitizeError;

#endif /* TAXONOMY_SANITIZE_ERROR_H */
