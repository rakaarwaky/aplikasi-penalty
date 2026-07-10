/**
 * @file taxonomy_competition_error.h
 * @brief Kode error tiap fitur (dipakai agar pesan gagal bisa ditampilkan).
 */

#ifndef SHARED_TAXONOMY_COMPETITION_ERROR_H
#define SHARED_TAXONOMY_COMPETITION_ERROR_H

/* Error pendaftaran peserta. */
typedef enum {
    REG_OK = 0,                /**< Berhasil. */
    REG_NAME_EMPTY,            /**< Nama kosong. */
    REG_NAME_TOO_LONG,         /**< Nama kepanjangan. */
    REG_NAME_INVALID_CHAR,     /**< Ada karakter tak sah (bukan huruf/spasi). */
    REG_NAME_DUPLICATE,        /**< Nama sudah dipakai. */
    REG_FULL,                  /**< Kuota peserta penuh. */
    REG_TOO_FEW                /**< Belum cukup peserta untuk selesai. */
} RegistrationError;

/* Error pencatatan tendangan. */
typedef enum {
    SC_OK = 0,                 /**< Berhasil. */
    SC_INVALID_ZONE,           /**< Zona di luar 0..5. */
    SC_NOT_REGISTERED,         /**< Data belum siap (belum terdaftar). */
    SC_ALREADY_DONE,           /**< Peserta sudah 7 tendangan. */
    SC_PARTICIPANT_NOT_FOUND   /**< Nomor peserta tak ada. */
} ScoringError;

/* Error perhitungan peringkat. */
typedef enum {
    RK_OK = 0,                 /**< Berhasil. */
    RK_NOT_READY,              /**< Lomba belum selesai (belum STATE_COMPLETED). */
    RK_NO_PARTICIPANT          /**< Tidak ada peserta / ruang kurang. */
} RankingError;

/* Error pencarian peserta. */
typedef enum {
    SR_OK = 0,                 /**< Berhasil. */
    SR_NOT_FOUND,              /**< Peserta tak ditemukan. */
    SR_EMPTY_QUERY             /**< Nama dicari kosong. */
} SearchError;

/* Error rekapitulasi. */
typedef enum {
    RC_OK = 0,                 /**< Berhasil. */
    RC_NOT_READY               /**< Lomba belum selesai. */
} RecapError;

#endif /* SHARED_TAXONOMY_COMPETITION_ERROR_H */
