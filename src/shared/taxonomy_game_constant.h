/**
 * @file taxonomy_game_constant.h
 * @brief Batas & kode tetap permainan penalti (1 sumber ubah untuk seluruh aturan).
 */

#ifndef SHARED_TAXONOMY_GAME_CONSTANT_H
#define SHARED_TAXONOMY_GAME_CONSTANT_H

/* ── Batas jumlah peserta ── */

/** Minimal 5 peserta supaya ada peringkat yang bermakna. */
#define MIN_PARTICIPANTS 5

/** Maksimal 7 peserta — batas array data lomba. */
#define MAX_PARTICIPANTS 7

/* ── Batas tendangan ── */

/** Tiap peserta menendang persis 7 kali. Dipakai untuk ukuran array kicks[]. */
#define TOTAL_KICKS 7

/* ── Zona tendangan (0 = miss, 1..5 = sasaran) ── */

/** Zona terrendah: tembakan meleset sama sekali. */
#define MIN_ZONE 0

/** Zona tertinggi: pojok atas, nilai poin maksimum. */
#define MAX_ZONE 5

/* ── Batas nama ── */

/** Panjang nama terpanjang yang diterima (belum termasu null). Dipakai ukuran buffer nama. */
#define MAX_NAME_LENGTH 30

/** Nama file penyimpanan lomba (default, pakai di auto-load & menu simpan). */
#define DEFAULT_STORAGE_FILENAME "data_lomba.bin"

/* ── Kode pilihan menu utama (cocok dengan nomor di layar menu) ── */

#define MENU_EXIT 0        /**< Pilihan 0: keluar dari program. */
#define MENU_REGISTRATION 1 /**< Pilihan 1: layar pendaftaran peserta. */
#define MENU_SCORING 2     /**< Pilihan 2: layar input tendangan & skor. */
#define MENU_RANKING 3     /**< Pilihan 3: layar tampilkan peringkat. */
#define MENU_SEARCH 4       /**< Pilihan 4: layar cari peserta. */
#define MENU_RECAP 5        /**< Pilihan 5: layar rekapitulasi lengkap. */

#endif /* SHARED_TAXONOMY_GAME_CONSTANT_H */
