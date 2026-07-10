/**
 * @file surfaces_menu_command.c
 * @brief Layar menu utama: tampilkan daftar menu & alihkan ke fitur yang dipilih.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"
#include "storage/module.storage.h"

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

    /* Bingkai kotak - grid 2 kolom */
    int grid_width = 56;
    int grid_height = 10;
    tui_box(BOX_START_ROW, BOX_START_COL, grid_width, grid_height);

    /* Garis pemisah setelah judul box */
    tui_separator(BOX_START_ROW + 1, BOX_START_COL, grid_width);

    /* Grid 2 kolom: item 0 di kiri, 1-5 di grid */
    int col_width = (grid_width - 4) / 2;
    int col_left = BOX_START_COL + 2;
    int col_right = BOX_START_COL + col_width + 3;

    /* Header kolom */
    attron(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    mvprintw(BOX_START_ROW + 2, col_left, "%-*s", col_width, "  MENU");
    mvprintw(BOX_START_ROW + 2, col_right, "%-*s", col_width, "  STATUS");
    attroff(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    tui_separator(BOX_START_ROW + 3, BOX_START_COL, grid_width);

    /* Grid: 2 item per baris, 3 baris untuk 5 menu items (1-5) */
    int grid_items[3][2] = {
        {1, 2},  /* Pendaftaran, Input Skor */
        {3, 4},  /* Ranking, Cari */
        {5, 0}   /* Rekap, (kosong) */
    };

    int row_idx;
    for (row_idx = 0; row_idx < 3; row_idx++) {
        int row = BOX_START_ROW + 4 + row_idx;

        int col_idx;
        for (col_idx = 0; col_idx < 2; col_idx++) {
            int item = grid_items[row_idx][col_idx];
            if (item == 0) continue;

            int cx = (col_idx == 0) ? col_left : col_right;

            /* Status tags */
            const char *status = "";
            int status_color = COLOR_DIM;

            if (item == 1) {
                if (state == STATE_INIT) { status = "[AKTIF]"; status_color = COLOR_SUCCESS; }
                else { status = "[SUDAH]"; status_color = COLOR_WARNING; }
            } else if (item == 2) {
                if (state == STATE_INIT) { status = "[KUNCI]"; status_color = COLOR_ERROR; }
                else if (state == STATE_COMPLETED) { status = "[SELESAI]"; status_color = COLOR_WARNING; }
                else { status = "[AKTIF]"; status_color = COLOR_SUCCESS; }
            } else if (item == 3) {
                if (state != STATE_COMPLETED) { status = "[KUNCI]"; status_color = COLOR_ERROR; }
                else { status = "[AKTIF]"; status_color = COLOR_SUCCESS; }
            } else if (item == 4) {
                if (state == STATE_INIT) { status = "[KUNCI]"; status_color = COLOR_ERROR; }
                else { status = "[AKTIF]"; status_color = COLOR_SUCCESS; }
            } else if (item == 5) {
                if (state != STATE_COMPLETED) { status = "[KUNCI]"; status_color = COLOR_ERROR; }
                else { status = "[AKTIF]"; status_color = COLOR_SUCCESS; }
            }

            /* Highlight jika dipilih */
            if (item == selected) {
                attron(COLOR_PAIR(COLOR_HIGHLIGHT));
                int w;
                for (w = 0; w < col_width + 4; w++) mvaddch(row, cx - 1 + w, ' ');
                attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
            }

            /* Nomor + nama menu */
            int item_color = (item == selected) ? COLOR_HIGHLIGHT : COLOR_MENU;
            attron(COLOR_PAIR(item_color) | ((item <= 3) ? A_BOLD : 0));
            mvprintw(row, cx, "[%d] %s", item, menu_labels[item] + 2);
            attroff(COLOR_PAIR(item_color) | ((item <= 3) ? A_BOLD : 0));

            /* Status */
            attron(COLOR_PAIR(status_color));
            mvprintw(row, cx + col_width - 6, "%s", status);
            attroff(COLOR_PAIR(status_color));
        }
    }

    /* Garis pemisah */
    tui_separator(BOX_START_ROW + 7, BOX_START_COL, grid_width);

    /* Info status lomba */
    const char *state_text = "";
    int state_color = COLOR_DIM;
    switch (state) {
        case STATE_INIT:      state_text = "Fase: Pendaftaran"; state_color = COLOR_INFO; break;
        case STATE_REGISTERED: state_text = "Fase: Input Tendangan"; state_color = COLOR_WARNING; break;
        case STATE_COMPLETED: state_text = "Fase: Selesai"; state_color = COLOR_SUCCESS; break;
    }
    tui_print_colored(BOX_START_ROW + 8, BOX_START_COL + 2, state_text, state_color);

    /* Keluar button */
    int exit_color = (selected == 0) ? COLOR_HIGHLIGHT : COLOR_ERROR;
    attron(COLOR_PAIR(exit_color) | A_BOLD);
    mvprintw(BOX_START_ROW + 9, BOX_START_COL + 2, "[0] Keluar");
    attroff(COLOR_PAIR(exit_color) | A_BOLD);

    /* Petunjuk navigasi */
    tui_footer("[↑/↓] Pilih  [ENTER] OK  [1-5] Shortcut  [h] Bantuan  [q] Keluar");

    refresh();
}

/* C1: Layar bantuan "Cara Bermain" — tampilkan aturan & navigasi. */
static void show_help_screen(void) {
    tui_clear();

    tui_print_centered_colored(0, "+----------------------------------------------------------+", COLOR_BORDER, 0);
    tui_print_centered_colored(1, "  PANDUAN PENGGUNAAN   ", COLOR_TITLE, 1);
    tui_print_centered_colored(2, "+----------------------------------------------------------+", COLOR_BORDER, 0);

    int box_col = 2;
    int box_width = BOX_WIDTH;
    tui_box(3, box_col, box_width, 18);
    tui_separator(4, box_col, box_width);

    attron(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
    mvprintw(5, box_col + 2, "ATURAN LOMBA:");
    attroff(COLOR_PAIR(COLOR_GOLD) | A_BOLD);

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(6,  box_col + 4, "- Jumlah peserta : 5 sampai 7 orang");
    mvprintw(7,  box_col + 4, "- Setiap peserta : 7 tendangan penalti");
    mvprintw(8,  box_col + 4, "- Nilai zona     : 0=Miss  1=Mudah  2=Sedang");
    mvprintw(9,  box_col + 4, "                   3=Sulit 4=Sangat Sulit  5=Top!");
    mvprintw(10, box_col + 4, "- Pemenang       : peserta dengan total skor tertinggi");
    attroff(COLOR_PAIR(COLOR_MENU));

    tui_separator(11, box_col, box_width);

    attron(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
    mvprintw(12, box_col + 2, "NAVIGASI:");
    attroff(COLOR_PAIR(COLOR_GOLD) | A_BOLD);

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(13, box_col + 4, "[Panah ↑/↓]  : Pindah pilihan menu");
    mvprintw(14, box_col + 4, "[1-5]        : Langsung pilih menu 1-5");
    mvprintw(15, box_col + 4, "[ENTER]      : Konfirmasi pilihan");
    mvprintw(16, box_col + 4, "[q / 0]      : Keluar dari aplikasi");
    mvprintw(17, box_col + 4, "[h]          : Tampilkan layar bantuan ini");
    attroff(COLOR_PAIR(COLOR_MENU));

    tui_separator(18, box_col, box_width);

    tui_footer("Tekan sembarang tombol untuk kembali ke menu");
    refresh();
    tui_getch();
}

/**
 * Loop menu utama: baca tombol, pindah pilihan, lalu buka layar
 * fitur yang dipilih (hanya bila kondisi lomba mengizinkan).
 */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          StorageAggregate *st,
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

            /* C3: Shortcut angka langsung aktifkan menu (1-5). */
            case '1': selected = 1; goto do_enter;
            case '2': selected = 2; goto do_enter;
            case '3': selected = 3; goto do_enter;
            case '4': selected = 4; goto do_enter;
            case '5': selected = 5; goto do_enter;

            case TUI_KEY_ENTER:
            do_enter:
                if (selected == 0) {
                    if (tui_confirm("Yakin ingin keluar?"))
                        running = 0;
                } else if (selected == 1) {
                    if (state->state == STATE_INIT || state->state == STATE_REGISTERED) {
                        cli_surfaces_registration_execute(reg, state);
                        agent_storage_save(st, "lomba.penalty", state);
                    }
                } else if (selected == 2) {
                    if (state->state == STATE_REGISTERED) {
                        cli_surfaces_scoring_execute(sc, state);
                        agent_storage_save(st, "lomba.penalty", state);
                    }
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

            /* A2: Konfirmasi sebelum keluar via ESC. */
            case TUI_KEY_ESC:
                if (tui_confirm("Yakin ingin keluar?"))
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

            /* C1: Tampilkan layar bantuan. */
            case 'h':
            case 'H':
                show_help_screen();
                break;
        }
    }
    return 0;
}
