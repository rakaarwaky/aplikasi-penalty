/**
 * @file surfaces_menu_command.c
 * @brief Smart surface: navigasi menu utama & dispatch ke fitur.
 */

#include "cli/module.cli.h"

#include <stdio.h>

#define MENU_ITEMS 6
#define BOX_WIDTH  60
#define BOX_START_ROW 3
#define BOX_START_COL 2
#define MIN_LINES  18
#define MIN_COLS   64

static const char *menu_labels[MENU_ITEMS] = {
    "Keluar",
    "Pendaftaran Peserta",
    "Input Tendangan dan Skor",
    "Tampilkan Ranking",
    "Cari Peserta",
    "Rekapitulasi Lengkap"
};

static void resolve_status(int item, CompetitionStateKind state,
                           const char **out_status, int *out_color) {
    const char *aktif   = "[Aktif]";
    const char *kunci   = "[Terkunci]";
    const char *selesai = "[Selesai]";
    switch (item) {
        case 1:
            if (state == STATE_INIT) { *out_status = aktif; *out_color = COLOR_SUCCESS; }
            else { *out_status = selesai; *out_color = COLOR_WARNING; }
            break;
        case 2:
            if (state == STATE_INIT) { *out_status = kunci; *out_color = COLOR_DIM; }
            else if (state == STATE_COMPLETED) { *out_status = selesai; *out_color = COLOR_WARNING; }
            else { *out_status = aktif; *out_color = COLOR_SUCCESS; }
            break;
        case 3: case 5:
            if (state != STATE_COMPLETED) { *out_status = kunci; *out_color = COLOR_DIM; }
            else { *out_status = aktif; *out_color = COLOR_SUCCESS; }
            break;
        case 4:
            if (state == STATE_INIT) { *out_status = kunci; *out_color = COLOR_DIM; }
            else { *out_status = aktif; *out_color = COLOR_SUCCESS; }
            break;
        default:
            *out_status = ""; *out_color = COLOR_DIM; break;
    }
}

static void resolve_state_text(CompetitionStateKind state,
                               const char **out_text, int *out_color) {
    switch (state) {
        case STATE_INIT:       *out_text = "Fase: Pendaftaran";     *out_color = COLOR_INFO;    break;
        case STATE_REGISTERED: *out_text = "Fase: Input Tendangan"; *out_color = COLOR_WARNING; break;
        case STATE_COMPLETED:  *out_text = "Fase: Selesai";         *out_color = COLOR_SUCCESS; break;
        default:               *out_text = "Fase: ?";              *out_color = COLOR_DIM;     break;
    }
}

static void draw_menu(DisplayPort *dp, int selected, CompetitionState *state) {
    char buf[128];
    dp->cls();

    dp->print_centered_colored(0, UTF_DOUBLE_H_40, COLOR_DIM, 0);
    dp->print_centered_colored(1, "    APLIKASI PERHITUNGAN PENALTI    ", COLOR_TITLE, 1);
    dp->print_centered_colored(2, UTF_DOUBLE_H_40, COLOR_DIM, 0);

    int gw = BOX_WIDTH;
    dp->box(BOX_START_ROW, BOX_START_COL, gw, 14);
    dp->separator(BOX_START_ROW + 1, BOX_START_COL, gw);

    int col1_x = BOX_START_COL + 3;
    int col2_x = BOX_START_COL + gw - 16;
    int col2_width = 14;

    dp->draw_colored(BOX_START_ROW + 2, col1_x, COLOR_INFO, 1, "MENU");
    dp->draw_colored(BOX_START_ROW + 2, col2_x, COLOR_INFO, 1, "STATUS");
    dp->separator(BOX_START_ROW + 3, BOX_START_COL, gw);

    int i;
    for (i = 1; i <= 5; i++) {
        int row = BOX_START_ROW + 3 + i;
        int item_sel = (i == selected);
        int item_color = item_sel ? COLOR_HIGHLIGHT : COLOR_MENU;

        if (item_sel) {
            int j;
            for (j = 1; j < gw - 1; j++)
                dp->draw_at(row, BOX_START_COL + j, " ");
        }

        snprintf(buf, sizeof buf, "  [%d]  %s", i, menu_labels[i]);
        dp->draw_colored(row, col1_x, item_color, item_sel, buf);

        const char *status = "";
        int status_color = COLOR_DIM;
        resolve_status(i, state->state, &status, &status_color);

        snprintf(buf, sizeof buf, "%-*s", col2_width, status);
        dp->draw_colored(row, col2_x, status_color, 0, buf);
    }

    dp->separator(BOX_START_ROW + 9, BOX_START_COL, gw);

    const char *state_text = "";
    int state_color = COLOR_DIM;
    resolve_state_text(state->state, &state_text, &state_color);
    dp->draw_colored(BOX_START_ROW + 10, BOX_START_COL + 2, state_color, 0, state_text);

    snprintf(buf, sizeof buf, "Peserta: %d/7", state->participant_count);
    dp->draw_colored(BOX_START_ROW + 10, BOX_START_COL + 24, COLOR_MENU, 0, buf);

    int exit_color = (0 == selected) ? COLOR_HIGHLIGHT : COLOR_ERROR;
    dp->draw_colored(BOX_START_ROW + 10, BOX_START_COL + gw - 18, exit_color, 1, "  [0] Keluar  ");

    dp->footer("[\xe2\x86\x91/\xe2\x86\x93] Navigasi  \xe2\x94\x82  [ENTER] Pilih  \xe2\x94\x82  [1-5] Shortcut  \xe2\x94\x82  [h] Bantuan");
    dp->screen_refresh();
}

static void draw_help(DisplayPort *dp) {
    dp->cls();
    dp->print_centered_colored(0, UTF_DOUBLE_H_40, COLOR_DIM, 0);
    dp->print_centered_colored(1, "  PANDUAN PENGGUNAAN  ", COLOR_TITLE, 1);
    dp->print_centered_colored(2, UTF_DOUBLE_H_40, COLOR_DIM, 0);

    int box_col = 2;
    dp->box(3, box_col, BOX_WIDTH, 18);
    dp->separator(4, box_col, BOX_WIDTH);

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
    for (ri = 0; rules[ri] != NULL; ri++)
        dp->draw_colored(6 + ri, box_col + 4, COLOR_MENU, 0, rules[ri]);

    dp->separator(11, box_col, BOX_WIDTH);

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
    for (ni = 0; navs[ni] != NULL; ni++)
        dp->draw_colored(13 + ni, box_col + 4, COLOR_MENU, 0, navs[ni]);

    dp->separator(18, box_col, BOX_WIDTH);
    dp->footer("Tekan sembarang tombol untuk kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}

static void dispatch_feature(int selected, RegistrationAggregate *reg,
                             ScoringAggregate *sc, RankingAggregate *rk,
                             SearchAggregate *sr, RecapAggregate *rc,
                             CompetitionState *state, DisplayPort *dp,
                             SanitizeAggregate *sn) {
    if (selected == 1) {
        if (state->state == STATE_INIT || state->state == STATE_REGISTERED)
            cli_surfaces_registration_execute(reg, state, dp, sn);
    } else if (selected == 2) {
        if (state->state == STATE_REGISTERED)
            cli_surfaces_scoring_execute(sc, state, dp, sn);
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
}

int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          CompetitionState *state,
                          DisplayPort *dp,
                          SanitizeAggregate *sn) {
    int selected = 1;
    int running = 1;

    while (running) {
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
            continue;
        }

        draw_menu(dp, selected, state);
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
            case TUI_KEY_RESIZE:
                break;
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
                } else {
                    dispatch_feature(selected, reg, sc, rk, sr, rc, state, dp, sn);
                }
                break;
            case TUI_KEY_ESC:
            case '0':
            case 'q':
            case 'Q':
                if (dp->confirm("Yakin ingin keluar?"))
                    running = 0;
                break;
            case 'h':
            case 'H':
                draw_help(dp);
                break;
        }
    }
    return 0;
}
