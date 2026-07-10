#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>

#define MENU_ITEMS 6
#define BOX_WIDTH  50
#define BOX_START_ROW 2
#define BOX_START_COL 2

static const char *menu_labels[MENU_ITEMS] = {
    "0. Keluar",
    "1. Pendaftaran Peserta",
    "2. Input Tendangan dan Skor",
    "3. Tampilkan Ranking",
    "4. Cari Peserta",
    "5. Rekapitulasi Lengkap"
};

static void draw_menu(int selected, CompetitionStateKind state) {
    tui_clear();

    /* Title */
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "APLIKASI LOMBA TENDANGAN PENALTI");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    /* Box */
    tui_box(BOX_START_ROW, BOX_START_COL, BOX_WIDTH, MENU_ITEMS + 4);

    /* Menu items */
    int i;
    for (i = 0; i < MENU_ITEMS; i++) {
        int row = BOX_START_ROW + 2 + i;
        char label[64];
        snprintf(label, sizeof label, "%s", menu_labels[i]);

        /* Status indicators sesuai PRD State Machine */
        if (i == 1) { /* Registration */
            if (state == STATE_INIT) strcat(label, "  [AKTIF]");
            else strcat(label, "  [SUDAH]");
        } else if (i == 2) { /* Scoring */
            if (state == STATE_INIT) strcat(label, "  [BLOKIR]");
            else if (state == STATE_COMPLETED) strcat(label, "  [SELESAI]");
            else strcat(label, "  [AKTIF]");
        } else if (i == 3) { /* Ranking */
            if (state != STATE_COMPLETED) strcat(label, "  [BLOKIR]");
            else strcat(label, "  [AKTIF]");
        } else if (i == 4) { /* Search */
            if (state == STATE_INIT) strcat(label, "  [BLOKIR]");
            else strcat(label, "  [AKTIF]");
        } else if (i == 5) { /* Recap */
            if (state != STATE_COMPLETED) strcat(label, "  [BLOKIR]");
            else strcat(label, "  [AKTIF]");
        }

        if (i == selected)
            tui_highlight_row(row, BOX_START_COL, BOX_WIDTH, label);
        else
            tui_normal_row(row, BOX_START_COL, BOX_WIDTH, label);
    }

    /* Footer */
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(BOX_START_ROW + MENU_ITEMS + 3, BOX_START_COL + 2,
             "Navigate: v/^  Select: Enter  Quit: 0");
    attroff(COLOR_PAIR(COLOR_MENU));

    refresh();
}

int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          CompetitionState *state) {
    int selected = 1;
    int running = 1;

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
            case TUI_KEY_ENTER:
                if (selected == 0) {
                    running = 0;
                } else if (selected == 1) {
                    /* Menu 1 aktif di INIT dan REGISTERED */
                    if (state->state == STATE_INIT || state->state == STATE_REGISTERED)
                        cli_surfaces_registration_execute(reg, state);
                } else if (selected == 2) {
                    /* Menu 2 aktif di REGISTERED */
                    if (state->state == STATE_REGISTERED)
                        cli_surfaces_scoring_execute(sc, state);
                } else if (selected == 3) {
                    if (state->state == STATE_COMPLETED)
                        cli_surfaces_ranking_execute(rk, state);
                } else if (selected == 4) {
                    /* Menu 4 aktif di REGISTERED dan COMPLETED */
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
