#ifndef SHARED_TAXONOMY_EXPORT_ERROR_H
#define SHARED_TAXONOMY_EXPORT_ERROR_H

/* Error enum untuk operasi ekspor file hasil ranking/recap. */

/** Error enum untuk operasi ekspor. */
typedef enum {
    EXP_OK = 0,                /**< Operasi ekspor berhasil. */
    EXP_ERROR_FILE_NOT_FOUND,  /**< File target tidak ditemukan. */
    EXP_ERROR_PERMISSION,      /**< Tidak memiliki izin menulis ke file. */
    EXP_ERROR_FORMAT,          /**< Format output tidak valid. */
    EXP_ERROR_EMPTY_DATA       /**< Data yang akan diekspor kosong. */
} ExportError;

#endif /* SHARED_TAXONOMY_EXPORT_ERROR_H */
