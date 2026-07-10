/**
 * @file surfaces_menu_command.c
 * @brief Layar menu utama: tampilkan daftar menu & alihkan ke fitur yang dipilih.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>

#define MENU_ITEMS 6     /* jumlah pilihan (0=keluar + 5 fitur) */
#define BOX_WIDTH  50     /* lebar kotak menu */
#define BOX_START_ROW 2   /* baris awal kotak */
#define BOX_START_COL 2   /* kolom awal kotak */

/* Teks tiap pilihan menu. */
static const char *menu_labels[MENU_ITEMS] = {
    "0. Keluar",
    "1. Pendaftaran Peserta",
    "2. Input Tendangan dan Skor",
    "3. Tampilkan Ranking",
    "4. Cari Peserta",
    "5. Rekapitulasi Lengkap"
};

/* Gambar ulang layar menu; tambahkan label status [AKTIF]/[BLOKIR]/[SUDAH]
   pada tiap menu sesuai kondisi lomba saat ini. */
static void draw_menu(int selected, CompetitionStateKind state) {
    tui_clear();

    /* Judul. */
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "APLIKASI LOMBA TENDANGAN PENALTI");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    /* Bingkai kotak. */
    tui_box(BOX_START_ROW, BOX_START_COL, BOX_WIDTH, MENU_ITEMS + 4);

    /* Tiap pilihan + label statusnya. */
    int i;
    for (i = 0; i < MENU_ITEMS; i++) {
        int row = BOX_START_ROW + 2 + i;
        char label[64];
        snprintf(label, sizeof label, "%s", menu_labels[i]);

        if (i == 1) { /* Pendaftaran */
            if (state == STATE_INIT) strcat(label, "  [AKTIF]");
            else strcat(label, "  [SUDAH]");
        } else if (i == 2) { /* Input tendangan */
            if (state == STATE_INIT) strcat(label, "  [BLOKIR]");
            else if (state == STATE_COMPLETED) strcat(label, "  [SELESAI]");
            else strcat(label, "  [AKTIF]");
        } else if (i == 3) { /* Ranking */
            if (state != STATE_COMPLETED) strcat(label, "  [BLOKIR]");
            else strcat(label, "  [AKTIF]");
        } else if (i == 4) { /* Cari */
            if (state == STATE_INIT) strcat(label, "  [BLOKIR]");
            else strcat(label, "  [AKTIF]");
        } else if (i == 5) { /* Rekap */
            if (state != STATE_COMPLETED) strcat(label, "  [BLOKIR]");
            else strcat(label, "  [AKTIF]");
        }

        if (i == selected)
            tui_highlight_row(row, BOX_START_COL, BOX_WIDTH, label);
        else
            tui_normal_row(row, BOX_START_COL, BOX_WIDTH, label);
    }

    /* Petunjuk navigasi. */
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(BOX_START_ROW + MENU_ITEMS + 3, BOX_START_COL + 2,
             "Navigasi: v/^  Pilih: Enter  Keluar: 0");
    attroff(COLOR_PAIR(COLOR_MENU));

    refresh();
}

/**
 * Loop menu utama: baca tombol, pindah pilihan, lalu buka layar
 * fitur yang dipilih (hanya bila kondisi lomba mengizinkan).
 */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          CompetitionState *state) {
    int selected = 1;   /* pilihan awal: menu 1 */
    int running = 1;

    while (running) {
        draw_menu(selected, state->state);
        int key = tui_getch();

        switch (key) {
            case TUI_KEY_UP:
                selected--;
                if (selected < 0) selected = MENU_ITEMS - 1; /* putar ke bawah */
                break;
            case TUI_KEY_DOWN:
                selected++;
                if (selected >= MENU_ITEMS) selected = 0;       /* putar ke atas */
                break;
            case TUI_KEY_ENTER:
                if (selected == 0) {
                    running = 0; /* Keluar */
                } else if (selected == 1) {
                    /* Pendaftaran: buka bila belum/masuk tahap pendaftaran. */
                    if (state->state == STATE_INIT || state->state == STATE_REGISTERED)
                        cli_surfaces_registration_execute(reg, state);
                } else if (selected == 2) {
                    /* Input tendangan: buka bila pendaftaran selesai. */
                    if (state->state == STATE_REGISTERED)
                        cli_surfaces_scoring_execute(sc, state);
                } else if (selected == 3) {
                    if (state->state == STATE_COMPLETED)
                        cli_surfaces_ranking_execute(rk, state);
                } else if (selected == 4) {
                    /* Cari: buka bila sudah ada peserta terdaftar. */
                    if (state->state == STATE_REGISTERED || state->state == STATE_COMPLETED)
                        cli_surfaces_search_execute(sr, state);
                } else if (selected == 5) {
                    if (state->state == STATE_COMPLETED)
                        cli_surfaces_recap_execute(rc, state);
                }
                break;
            case TUI_KEY_ESC:
                running = 0;
                break;
        }
    }
    return 0;
}
