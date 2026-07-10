/**
 * @file surfaces_menu_command.c
 * @brief Layar menu utama: tampilkan daftar menu & alihkan ke fitur yang dipilih.
 */

#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>

#define MENU_ITEMS 6
#define BOX_WIDTH  60
#define BOX_START_ROW 3
#define BOX_START_COL 2

/* Label menu (indeks 0 = keluar, 1-5 = fitur). */
static const char *menu_labels[MENU_ITEMS] = {
    "Keluar",
    "Pendaftaran Peserta",
    "Input Tendangan dan Skor",
    "Tampilkan Ranking",
    "Cari Peserta",
    "Rekapitulasi Lengkap"
};

/* Gambar ulang layar menu; 2 kolom: MENU (kiri) + STATUS (kanan). */
static void draw_menu(DisplayPort *dp, int selected, CompetitionStateKind state) {
    char buf[128];
    dp->cls();

    /* Header dekorasi */
    dp->print_centered_colored(0, "+----------------------------------------------------------+", COLOR_BORDER, 0);
    dp->print_centered_colored(1, "|", COLOR_BORDER, 0);
    dp->print_centered_colored(1, "  LOMBA TENDANGAN PENALTI  ", COLOR_TITLE, 1);
    int title_len = 32;
    int pad = (80 - title_len) / 2;
    dp->print_colored(1, pad + title_len, "|", COLOR_BORDER);
    dp->print_centered_colored(2, "+----------------------------------------------------------+", COLOR_BORDER, 0);

    /* Bingkai utama */
    int grid_width = BOX_WIDTH;
    int grid_height = 12;
    dp->box(BOX_START_ROW, BOX_START_COL, grid_width, grid_height);
    dp->separator(BOX_START_ROW + 1, BOX_START_COL, grid_width);

    /* ── Layout 2 kolom ── */
    int col1_x = BOX_START_COL + 2;                          /* Kolom kiri: menu   */
    int col2_x = BOX_START_COL + grid_width - 16;            /* Kolom kanan: status */
    int col2_width = 14;

    /* Header kolom */
    snprintf(buf, sizeof buf, "%-*s", col2_x - col1_x, "  MENU");
    dp->draw_colored(BOX_START_ROW + 2, col1_x, COLOR_HEADER, 1, buf);
    snprintf(buf, sizeof buf, "%-*s", col2_width, "STATUS");
    dp->draw_colored(BOX_START_ROW + 2, col2_x, COLOR_HEADER, 1, buf);
    dp->separator(BOX_START_ROW + 3, BOX_START_COL, grid_width);

    /* ── Baris menu 1-5 ── */
    int i;
    for (i = 1; i <= 5; i++) {
        int row = BOX_START_ROW + 3 + i;

        /* Kolom 1: nomor + nama menu */
        int item_color = (i == selected) ? COLOR_HIGHLIGHT : COLOR_MENU;
        snprintf(buf, sizeof buf, "[%d] %s", i, menu_labels[i]);
        dp->draw_colored(row, col1_x, item_color, 1, buf);

        /* Kolom 2: status */
        const char *status = "";
        int status_color = COLOR_DIM;

        if (i == 1) {
            if (state == STATE_INIT)      { status = "[AKTIF]";  status_color = COLOR_SUCCESS; }
            else                          { status = "[SUDAH]";  status_color = COLOR_WARNING; }
        } else if (i == 2) {
            if (state == STATE_INIT)      { status = "[KUNCI]";  status_color = COLOR_ERROR;   }
            else if (state == STATE_COMPLETED) { status = "[SELESAI]"; status_color = COLOR_WARNING; }
            else                          { status = "[AKTIF]";  status_color = COLOR_SUCCESS; }
        } else if (i == 3) {
            if (state != STATE_COMPLETED) { status = "[KUNCI]";  status_color = COLOR_ERROR;   }
            else                          { status = "[AKTIF]";  status_color = COLOR_SUCCESS; }
        } else if (i == 4) {
            if (state == STATE_INIT)      { status = "[KUNCI]";  status_color = COLOR_ERROR;   }
            else                          { status = "[AKTIF]";  status_color = COLOR_SUCCESS; }
        } else if (i == 5) {
            if (state != STATE_COMPLETED) { status = "[KUNCI]";  status_color = COLOR_ERROR;   }
            else                          { status = "[AKTIF]";  status_color = COLOR_SUCCESS; }
        }

        snprintf(buf, sizeof buf, "%-*s", col2_width, status);
        dp->draw_colored(row, col2_x, status_color, 0, buf);
    }

    /* Separator sebelum footer */
    dp->separator(BOX_START_ROW + 9, BOX_START_COL, grid_width);

    /* Status fase lomba */
    const char *state_text = "";
    int state_color = COLOR_DIM;
    switch (state) {
        case STATE_INIT:       state_text = "Fase: Pendaftaran";     state_color = COLOR_INFO;    break;
        case STATE_REGISTERED: state_text = "Fase: Input Tendangan"; state_color = COLOR_WARNING; break;
        case STATE_COMPLETED:  state_text = "Fase: Selesai";         state_color = COLOR_SUCCESS; break;
    }
    dp->draw_colored(BOX_START_ROW + 10, BOX_START_COL + 2, state_color, 0, state_text);

    /* Tombol keluar */
    int exit_color = (0 == selected) ? COLOR_HIGHLIGHT : COLOR_ERROR;
    dp->draw_colored(BOX_START_ROW + 10, BOX_START_COL + grid_width - 12, exit_color, 1, "[0] Keluar");

    /* Petunjuk navigasi */
    dp->footer("[\xe2\x86\x91/\xe2\x86\x93] Pilih  [ENTER] OK  [1-5] Shortcut  [h] Bantuan  [q] Keluar");

    dp->screen_refresh();
}

/* C1: Layar bantuan "Cara Bermain" — tampilkan aturan & navigasi. */
static void show_help_screen(DisplayPort *dp) {
    char buf[128];
    dp->cls();

    dp->print_centered_colored(0, "+----------------------------------------------------------+", COLOR_BORDER, 0);
    dp->print_centered_colored(1, "  PANDUAN PENGGUNAAN   ", COLOR_TITLE, 1);
    dp->print_centered_colored(2, "+----------------------------------------------------------+", COLOR_BORDER, 0);

    int box_col = 2;
    int box_width = BOX_WIDTH;
    dp->box(3, box_col, box_width, 18);
    dp->separator(4, box_col, box_width);

    dp->draw_colored(5, box_col + 2, COLOR_GOLD, 1, "ATURAN LOMBA:");

    const char *rules[] = {
        "- Jumlah peserta : 5 sampai 7 orang",
        "- Setiap peserta : 7 tendangan penalti",
        "- Nilai zona     : 0=Miss  1=Mudah  2=Sedang",
        "                   3=Sulit 4=Sangat Sulit  5=Top!",
        "- Pemenang       : peserta dengan total skor tertinggi",
        NULL
    };
    int ri;
    for (ri = 0; rules[ri] != NULL; ri++) {
        dp->draw_colored(6 + ri, box_col + 4, COLOR_MENU, 0, rules[ri]);
    }

    dp->separator(11, box_col, box_width);

    dp->draw_colored(12, box_col + 2, COLOR_GOLD, 1, "NAVIGASI:");

    const char *navs[] = {
        "[Panah \xe2\x86\x91/\xe2\x86\x93]  : Pindah pilihan menu",
        "[1-5]        : Langsung pilih menu 1-5",
        "[ENTER]      : Konfirmasi pilihan",
        "[q / 0]      : Keluar dari aplikasi",
        "[h]          : Tampilkan layar bantuan ini",
        NULL
    };
    int ni;
    for (ni = 0; navs[ni] != NULL; ni++) {
        dp->draw_colored(13 + ni, box_col + 4, COLOR_MENU, 0, navs[ni]);
    }

    dp->separator(18, box_col, box_width);

    dp->footer("Tekan sembarang tombol untuk kembali ke menu");
    dp->screen_refresh();

    (void)buf; /* suppress unused warning */
    dp->readkey();
}

/**
 * Loop menu utama: baca tombol, pindah pilihan, lalu buka layar
 * fitur yang dipilih (hanya bila kondisi lomba mengizinkan).
 */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          CompetitionState *state,
                          DisplayPort *dp) {
    int selected = 1;
    int running = 1;

    /* Minimum terminal size */
    const int MIN_LINES = 18;
    const int MIN_COLS  = 64;

    while (running) {
        /* Cek ukuran terminal — tampilkan pesan bila terlalu kecil */
        int lines = dp->get_lines();
        int cols  = dp->get_cols();
        if (lines < MIN_LINES || cols < MIN_COLS) {
            dp->cls();
            dp->print_centered_colored(4, "TERMINAL TERLALU KECIL!", COLOR_ERROR, 1);
            char msg[64];
            snprintf(msg, sizeof msg, "Minimum: %dx%d  Saat ini: %dx%d",
                     MIN_COLS, MIN_LINES, cols, lines);
            dp->print_centered_colored(6, msg, COLOR_WARNING, 0);
            dp->print_centered_colored(8, "Perbesar jendela terminal, lalu tekan apa saja.",
                                       COLOR_INFO, 0);
            dp->screen_refresh();
            int key = dp->readkey();
            if (key == 'q' || key == 'Q' || key == TUI_KEY_ESC) running = 0;
            continue;  /* ulang cek ukuran */
        }

        draw_menu(dp, selected, state->state);
        int key = dp->readkey();

        switch (key) {
            case TUI_KEY_UP:
                selected--;
                if (selected < 0) selected = MENU_ITEMS - 1;
                break;
            case TUI_KEY_DOWN:
                selected++;
                if (selected >= MENU_ITEMS) selected = 0;
                break;

            /* Resize: cukup loop ulang (draw_menu akan pakai LINES/COLS baru) */
            case TUI_KEY_RESIZE:
                break;

            /* C3: Shortcut angka langsung aktifkan menu (1-5). */
            case '1': selected = 1; goto do_enter;
            case '2': selected = 2; goto do_enter;
            case '3': selected = 3; goto do_enter;
            case '4': selected = 4; goto do_enter;
            case '5': selected = 5; goto do_enter;

            case TUI_KEY_ENTER:
            do_enter:
                if (selected == 0) {
                    if (dp->confirm("Yakin ingin keluar?"))
                        running = 0;
                } else if (selected == 1) {
                    if (state->state == STATE_INIT || state->state == STATE_REGISTERED)
                        cli_surfaces_registration_execute(reg, state, dp);
                } else if (selected == 2) {
                    if (state->state == STATE_REGISTERED)
                        cli_surfaces_scoring_execute(sc, state, dp);
                } else if (selected == 3) {
                    if (state->state == STATE_COMPLETED)
                        cli_surfaces_ranking_execute(rk, state, dp);
                } else if (selected == 4) {
                    if (state->state == STATE_REGISTERED || state->state == STATE_COMPLETED)
                        cli_surfaces_search_execute(sr, state, dp);
                } else if (selected == 5) {
                    if (state->state == STATE_COMPLETED)
                        cli_surfaces_recap_execute(rc, state, dp);
                }
                break;

            /* A2: Konfirmasi sebelum keluar via ESC. */
            case TUI_KEY_ESC:
                if (dp->confirm("Yakin ingin keluar?"))
                    running = 0;
                break;
            case '0':
                if (dp->confirm("Yakin ingin keluar?"))
                    running = 0;
                break;
            case 'q':
                if (dp->confirm("Yakin ingin keluar?"))
                    running = 0;
                break;

            /* C1: Tampilkan layar bantuan. */
            case 'h':
            case 'H':
                show_help_screen(dp);
                break;
        }
    }
    return 0;
}
