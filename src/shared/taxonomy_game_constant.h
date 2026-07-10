#ifndef SHARED_TAXONOMY_GAME_CONSTANT_H
#define SHARED_TAXONOMY_GAME_CONSTANT_H

/* Konstanta domain untuk seluruh batas permainan penalty shootout. */

/* ──────────────────────────────────────────────
 * Batas Peserta
 * ────────────────────────────────────────────── */
/** Jumlah minimum peserta yang harus terdaftar sebelum lomba boleh dimulai. */
#define MIN_PARTICIPANTS 5

/** Jumlah maksimum peserta yang didukung oleh kompetisi. */
#define MAX_PARTICIPANTS 7

/* ──────────────────────────────────────────────
 * Batas Tendangan
 * ────────────────────────────────────────────── */
/** Jumlah total tendangan yang diberikan oleh setiap peserta. */
#define TOTAL_KICKS 7

/* ──────────────────────────────────────────────
 * Zona Tendangan
 * ────────────────────────────────────────────── */
/** Zona minimum yang sah (0 = miss). */
#define MIN_ZONE 0

/** Zona maksimum yang sah (5 = sudut tertinggi). */
#define MAX_ZONE 5

/* ──────────────────────────────────────────────
 * Batas Nama
 * ────────────────────────────────────────────── */
/** Panjang maksimum karakter nama peserta (tanpa null-terminator). */
#define MAX_NAME_LENGTH 30

/* ──────────────────────────────────────────────
 * Kode Menu CLI
 * ────────────────────────────────────────────── */
/** Keluar dari aplikasi. */
#define MENU_EXIT 0

/** Masuk ke menu pendaftaran peserta. */
#define MENU_REGISTRATION 1

/** Masuk ke menu pencatatan tendangan (scoring). */
#define MENU_SCORING 2

/** Masuk ke menu ranking. */
#define MENU_RANKING 3

/** Masuk ke menu pencarian peserta. */
#define MENU_SEARCH 4

/** Masuk ke menu rekapitulasi hasil. */
#define MENU_RECAP 5

#endif /* SHARED_TAXONOMY_GAME_CONSTANT_H */
