/**
 * @file taxonomy_storage_error.h
 * @brief Kode error baca/tulis file penyimpanan data.
 */

#ifndef SHARED_TAXONOMY_STORAGE_ERROR_H
#define SHARED_TAXONOMY_STORAGE_ERROR_H

/** Hasil operasi simpan/muat file lomba. */
typedef enum {
    ST_OK = 0,                /**< Berhasil. */
    ST_ERROR_FILE_NOT_FOUND,  /**< File tak ada / nama kosong. */
    ST_ERROR_PERMISSION,      /**< Tidak boleh akses file. */
    ST_ERROR_CORRUPT,         /**< File rusak / tak utuh saat baca/tulis. */
    ST_ERROR_FULL             /**< Tempat simpanan penuh. */
} StorageError;

#endif /* SHARED_TAXONOMY_STORAGE_ERROR_H */
