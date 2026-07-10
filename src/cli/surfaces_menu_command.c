/**
 * @file surfaces_menu_command.c
 * @brief SMART surface — layar menu utama.
 *
 * Command memegang LOGIC: loop input, navigasi, guard ukuran terminal,
 * mapping fase lomba -> status tiap menu, dan validasi sebelum membuka fitur.
 * Ia MENYIAPKAN view model lalu memanggil page (dumb) untuk render. Tidak ada
 * gambar langsung ke DisplayPort di sini — semua via page/component.
 */

#include "cli/module.cli.h"
#include "cli/surfaces_menu_page.h"
#include "shared/taxonomy_game_constant.h"

#include <stdio.h>
#include <string.h>

#define MENU_ITEMS 6   /* 0=keluar, 1..5=fitur */
#define MIN_LINES  18
#define MIN_COLS   64

/* Fase lomba -> teks + warna info bawah. */
static void resolve_state_text(CompetitionStateKind state,
                               const char **out_text, int *out_color) {
    switch (state) {
        case STATE_INIT:       *out_text = "Fase: Pendaftaran";     *out_color = COLOR_INFO;    break;
        case STATE_REGISTERED: *out_text = "Fase: Input Tendangan"; *out_color = COLOR_WARNING; break;
        case STATE_COMPLETED:  *out_text = "Fase: Selesai";         *out_color = COLOR_SUCCESS; break;
        default:               *out_text = "Fase: ?";              *out_color = COLOR_DIM;     break;
    }
}

/* Hitung badge status untuk satu item menu (1..5). */
static void resolve_item_status(int item, CompetitionStateKind state,
                                const char **out_status, int *out_color) {
    const char *aktif   = "[Aktif]";
    const char *kunci   = "[Terkunci]";
    const char *selesai = "[Selesai]";

    switch (item) {
        case 1:
            if (state == STATE_INIT)      { *out_status = aktif;   *out_color = COLOR_SUCCESS; }
            else                          { *out_status = selesai; *out_color = COLOR_WARNING; }
            break;
        case 2:
            if (state == STATE_INIT)          { *out_status = kunci;   *out_color = COLOR_DIM; }
            else if (state == STATE_COMPLETED){ *out_status = selesai; *out_color = COLOR_WARNING; }
            else                              { *out_status = aktif;   *out_color = COLOR_SUCCESS; }
            break;
        case 3:
        case 5:
            if (state != STATE_COMPLETED) { *out_status = kunci;   *out_color = COLOR_DIM; }
            else                          { *out_status = aktif;   *out_color = COLOR_SUCCESS; }
            break;
        case 4:
            if (state == STATE_INIT)      { *out_status = kunci;   *out_color = COLOR_DIM; }
            else                          { *out_status = aktif;   *out_color = COLOR_SUCCESS; }
            break;
        default:
            *out_status = ""; *out_color = COLOR_DIM; break;
    }
}

/* Susun view model dari state lomba + pilihan. */
static void build_menu_view(CompetitionState *state, int selected,
                            MenuPageView *vm) {
    static const char *labels[MENU_ITEMS] = {
        "Keluar",
        "Pendaftaran Peserta",
        "Input Tendangan dan Skor",
        "Tampilkan Ranking",
        "Cari Peserta",
        "Rekapitulasi Lengkap"
    };

    vm->selected = selected;
    vm->participant_count = state->participant_count;

    int i;
    for (i = 0; i < MENU_ITEMS; i++) {
        vm->rows[i].label = labels[i];
        vm->rows[i].selected = (i == selected);

        if (i == 0) {
            /* Baris keluar: tidak punya badge status. */
            vm->rows[i].status = "";
            vm->rows[i].status_color = COLOR_DIM;
        } else {
            resolve_item_status(i, state->state,
                                &vm->rows[i].status, &vm->rows[i].status_color);
        }
    }

    resolve_state_text(state->state, &vm->state_text, &vm->state_color);
    /* participant_max diambil dari MAX_PARTICIPANTS (konstanta domain). */
    vm->participant_max = MAX_PARTICIPANTS;
}

/* Tampilkan layar bantuan (page dumb). */
static void show_help_screen(DisplayPort *dp) {
    const char *rules[] = {
        "- Jumlah peserta : 5 sampai 7 orang",
        "- Setiap peserta : 7 tendangan penalti",
        "- Nilai zona     : 0=Miss  1=Mudah  2=Sedang",
        "                   3=Sulit 4=Sangat Sulit  5=Top!",
        "- Pemenang       : peserta dengan total skor tertinggi",
        NULL
    };
    const char *navs[] = {
        "[Panah ↑/↓]  : Pindah pilihan menu",
        "[1-5]        : Langsung pilih menu 1-5",
        "[ENTER]      : Konfirmasi pilihan",
        "[q / 0]      : Keluar dari aplikasi",
        "[h]          : Tampilkan layar bantuan ini",
        NULL
    };
    int rc = 0, nc = 0, k;
    for (k = 0; rules[k] != NULL; k++) rc++;
    for (k = 0; navs[k] != NULL; k++) nc++;
    menu_page_render_help(dp, rules, rc, navs, nc);
    dp->readkey();
}

/* Buka fitur terpilih bila fase lomba mengizinkan. */
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

/* Loop menu utama. */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          CompetitionState *state,
                          DisplayPort *dp,
                          SanitizeAggregate *sn) {
    int selected = 1;
    int running = 1;

    while (running) {
        /* Guard ukuran terminal. */
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

        MenuPageView vm;
        build_menu_view(state, selected, &vm);
        menu_page_render(dp, &vm);

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
                break;  /* loop ulang, page pakai LINES/COLS baru */

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
                    dispatch_feature(selected, reg, sc, rk, sr, rc,
                                     state, dp, sn);
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
                show_help_screen(dp);
                break;
        }
    }
    return 0;
}
