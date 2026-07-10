/**
 * @file taxonomy_export_error.h
 * @brief Kode error penulisan file ekspor hasil peringkat.
 */

#ifndef SHARED_TAXONOMY_EXPORT_ERROR_H
#define SHARED_TAXONOMY_EXPORT_ERROR_H

/** Hasil operasi ekspor ke file. */
typedef enum {
    EXP_OK = 0,                /**< Berhasil. */
    EXP_ERROR_FILE_NOT_FOUND,  /**< Nama file kosong / data null. */
    EXP_ERROR_PERMISSION,      /**< Tidak boleh tulis file. */
    EXP_ERROR_FORMAT,          /**< Format output tak valid. */
    EXP_ERROR_EMPTY_DATA       /**< Tidak ada data untuk diekspor. */
} ExportError;

#endif /* SHARED_TAXONOMY_EXPORT_ERROR_H */
