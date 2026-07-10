/**
 * @file surfaces_menu_command.c
 * @brief Layar menu utama: tampilkan daftar menu & alihkan ke fitur yang dipilih.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>

#define MENU_ITEMS 6
#define BOX_WIDTH  52
#define BOX_START_ROW 3
#define BOX_START_COL 2

static const char *menu_labels[MENU_ITEMS] = {
    "  Keluar",
    "  Pendaftaran Peserta",
    "  Input Tendangan dan Skor",
    "  Tampilkan Ranking",
    "  Cari Peserta",
    "  Rekapitulasi Lengkap"
};

static const char *menu_icons[MENU_ITEMS] = {
    "[X]",
    "[>]",
    "[*]",
    "[#]",
    "[?]",
    "[=]"
};

/* Gambar ulang layar menu; tambahkan label status sesuai kondisi lomba. */
static void draw_menu(int selected, CompetitionStateKind state) {
    tui_clear();

    /* Dekorasi header */
    tui_print_centered_colored(0, "+----------------------------------------------------------+", COLOR_BORDER, 0);
    tui_print_centered_colored(1, "|", COLOR_BORDER, 0);

    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "  LOMBA TENDANGAN PENALTI  ");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    /* Tutup garis header */
    int title_len = 32;
    int pad = (COLS - title_len) / 2;
    tui_print_colored(1, pad + title_len, "|", COLOR_BORDER);

    tui_print_centered_colored(2, "+----------------------------------------------------------+", COLOR_BORDER, 0);

    /* Bingkai kotak. */
    tui_box(BOX_START_ROW, BOX_START_COL, BOX_WIDTH, MENU_ITEMS + 4);

    /* Garis pemisah setelah judul box */
    tui_separator(BOX_START_ROW + 1, BOX_START_COL, BOX_WIDTH);

    /* Tiap pilihan + label statusnya */
    int i;
    for (i = 0; i < MENU_ITEMS; i++) {
        int row = BOX_START_ROW + 2 + i;
        char label[80];
        snprintf(label, sizeof label, "%s %s", menu_icons[i], menu_labels[i]);

        /* Status tags */
        const char *status = "";

        if (i == 0) {
            /* Keluar - tidak ada status */
        } else if (i == 1) {
            if (state == STATE_INIT) status = "[AKTIF]";
            else status = "[SUDAH]";
        } else if (i == 2) {
            if (state == STATE_INIT) status = "[KUNCI]";
            else if (state == STATE_COMPLETED) status = "[SELESAI]";
            else status = "[AKTIF]";
        } else if (i == 3) {
            if (state != STATE_COMPLETED) status = "[KUNCI]";
            else status = "[AKTIF]";
        } else if (i == 4) {
            if (state == STATE_INIT) status = "[KUNCI]";
            else status = "[AKTIF]";
        } else if (i == 5) {
            if (state != STATE_COMPLETED) status = "[KUNCI]";
            else status = "[AKTIF]";
        }

        char full_label[128];
        snprintf(full_label, sizeof full_label, "%-28s %s", label, status);

        if (i == selected) {
            tui_highlight_row(row, BOX_START_COL, BOX_WIDTH, full_label);
        } else {
            tui_normal_row(row, BOX_START_COL, BOX_WIDTH, full_label);
        }
    }

    /* Separator sebelum footer */
    tui_separator(BOX_START_ROW + MENU_ITEMS + 2, BOX_START_COL, BOX_WIDTH);

    /* Info status lomba */
    const char *state_text = "";
    int state_color = COLOR_DIM;
    switch (state) {
        case STATE_INIT:      state_text = "Fase: Pendaftaran"; state_color = COLOR_INFO; break;
        case STATE_REGISTERED: state_text = "Fase: Input Tendangan"; state_color = COLOR_WARNING; break;
        case STATE_COMPLETED: state_text = "Fase: Selesai"; state_color = COLOR_SUCCESS; break;
    }
    tui_print_colored(BOX_START_ROW + MENU_ITEMS + 3, BOX_START_COL + 2, state_text, state_color);

    /* Petunjuk navigasi */
    tui_footer("[v/^] Pilih  [ENTER] OK  [1-5] Langsung  [h] Bantuan  [q/0/ESC] Keluar");

    refresh();
}

/* Layar bantuan singkat "Cara Bermain". */
static void draw_help(void) {
    tui_clear();
    tui_print_centered_colored(1, "CARA BERMAIN", COLOR_TITLE, 1);
    tui_box(3, 2, 70, 18);
    tui_separator(4, 2, 70);

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(5, 4, "1. Daftar 5-7 peserta (Menu 1). Nama huruf & spasi, maks 30.");
    mvprintw(6, 4, "2. Tiap peserta menendang 7 kali (Menu 2). Zona 0-5 = poin.");
    mvprintw(7, 4, "3. Zona tinggi (5) dapat lebih banyak poin.");
    mvprintw(8, 4, "4. Setelah semua selesai, lihat Ranking (Menu 3) & Rekap (Menu 5).");
    mvprintw(9, 4, "5. Cari peserta tertentu lewat Menu 4.");
    attroff(COLOR_PAIR(COLOR_MENU));

    tui_separator(10, 2, 70);

    attron(COLOR_PAIR(COLOR_WARNING) | A_BOLD);
    mvprintw(11, 4, "Navigasi:");
    attroff(COLOR_PAIR(COLOR_WARNING) | A_BOLD);
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(12, 4, "  ^ / v        : pindah pilihan");
    mvprintw(13, 4, "  ENTER / 1-5 : pilih menu");
    mvprintw(14, 4, "  h           : layar bantuan ini");
    mvprintw(15, 4, "  q / 0 / ESC : keluar (dengan konfirmasi)");
    attroff(COLOR_PAIR(COLOR_MENU));

    tui_footer("Tekan tombol apa saja untuk kembali");
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
    int selected = 1;
    int running = 1;

    /* Splash sudah ditampilkan di main(); langsung masuk loop menu. */

    while (running) {
        draw_menu(selected, state->state);
        int key = tui_getch();

        switch (key) {
            case TUI_KEY_UP:
                selected--;
                if (selected < 0) selected = MENU_ITEMS - 1;
                break;
            case TUI_KEY_DOWN:
                selected++;
                if (selected >= MENU_ITEMS) selected = 0;
                break;
            case 'h':
            case 'H':
                draw_help();
                tui_getch();
                break;
            case '1': case '2': case '3': case '4': case '5':
                if (key >= '1' && key <= '5')
                    selected = key - '0';
                /* lanjut ke penanganan seperti ENTER di bawah */
                /* fallthrough */
            case TUI_KEY_ENTER:
                if (selected == 0) {
                    if (tui_confirm("Yakin ingin keluar?"))
                        running = 0;
                } else if (selected == 1) {
                    if (state->state == STATE_INIT || state->state == STATE_REGISTERED)
                        cli_surfaces_registration_execute(reg, state);
                } else if (selected == 2) {
                    if (state->state == STATE_REGISTERED)
                        cli_surfaces_scoring_execute(sc, state);
                } else if (selected == 3) {
                    if (state->state == STATE_COMPLETED)
                        cli_surfaces_ranking_execute(rk, state);
                } else if (selected == 4) {
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
            case '0':
                if (tui_confirm("Yakin ingin keluar?"))
                    running = 0;
                break;
            case 'q':
                if (tui_confirm("Yakin ingin keluar?"))
                    running = 0;
                break;
        }
    }
    return 0;
}
