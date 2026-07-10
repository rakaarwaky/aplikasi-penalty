#ifndef SHARED_TAXONOMY_COMPETITION_ERROR_H
#define SHARED_TAXONOMY_COMPETITION_ERROR_H

/* Error enum per fitur: registration, scoring, ranking, search, recap. */

/* ──────────────────────────────────────────────
 * RegistrationError — error pendaftaran peserta
 * ────────────────────────────────────────────── */
typedef enum {
    REG_OK = 0,                /**< Operasi berhasil. */
    REG_NAME_EMPTY,            /**< Nama kosong (0 karakter). */
    REG_NAME_TOO_LONG,         /**< Nama melebihi MAX_NAME_LENGTH. */
    REG_NAME_INVALID_CHAR,     /**< Nama mengandung karakter tidak valid. */
    REG_NAME_DUPLICATE,        /**< Nama sudah terdaftar. */
    REG_FULL,                  /**< Jumlah peserta sudah MAX_PARTICIPANTS. */
    REG_TOO_FEW                /**< Jumlah peserta kurang dari MIN_PARTICIPANTS (untuk finalisasi). */
} RegistrationError;

/* ──────────────────────────────────────────────
 * ScoringError — error pencatatan tendangan
 * ────────────────────────────────────────────── */
typedef enum {
    SC_OK = 0,                 /**< Operasi berhasil. */
    SC_INVALID_ZONE,           /**< Zona di luar rentang [MIN_ZONE, MAX_ZONE]. */
    SC_NOT_REGISTERED,         /**< Peserta belum terdaftar. */
    SC_ALREADY_DONE,           /**< Peserta sudah selesai TOTAL_KICKS tendangan. */
    SC_PARTICIPANT_NOT_FOUND   /**< Peserta dengan ID tersebut tidak ditemukan. */
} ScoringError;

/* ──────────────────────────────────────────────
 * RankingError — error perhitungan ranking
 * ────────────────────────────────────────────── */
typedef enum {
    RK_OK = 0,                 /**< Operasi berhasil. */
    RK_NOT_READY,              /**< State belum STATE_COMPLETED. */
    RK_NO_PARTICIPANT          /**< Tidak ada peserta terdaftar. */
} RankingError;

/* ──────────────────────────────────────────────
 * SearchError — error pencarian peserta
 * ────────────────────────────────────────────── */
typedef enum {
    SR_OK = 0,                 /**< Operasi berhasil. */
    SR_NOT_FOUND,              /**< Peserta tidak ditemukan. */
    SR_EMPTY_QUERY             /**< Query pencarian kosong. */
} SearchError;

/* ──────────────────────────────────────────────
 * RecapError — error rekapitulasi
 * ────────────────────────────────────────────── */
typedef enum {
    RC_OK = 0,                 /**< Operasi berhasil. */
    RC_NOT_READY               /**< State belum STATE_COMPLETED. */
} RecapError;

#endif /* SHARED_TAXONOMY_COMPETITION_ERROR_H */
