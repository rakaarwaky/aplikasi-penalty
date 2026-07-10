#ifndef SHARED_TAXONOMY_STORAGE_ERROR_H
#define SHARED_TAXONOMY_STORAGE_ERROR_H

/* Error enum untuk operasi save/load state kompetisi ke file. */

/** Error enum untuk operasi storage. */
typedef enum {
    ST_OK = 0,                /**< Operasi storage berhasil. */
    ST_ERROR_FILE_NOT_FOUND,  /**< File tidak ditemukan (untuk load). */
    ST_ERROR_PERMISSION,      /**< Tidak memiliki izin akses file. */
    ST_ERROR_CORRUPT,         /**< File rusak atau format tidak sesuai. */
    ST_ERROR_FULL             /**< Penyimpanan penuh (tidak bisa menulis). */
} StorageError;

#endif /* SHARED_TAXONOMY_STORAGE_ERROR_H */
