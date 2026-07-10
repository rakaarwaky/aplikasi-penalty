/**
 * @file surfaces_search_command.c
 * @brief Layar cari: baca nama & tampilkan data peserta yang cocok.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>

/**
 * Layar pencarian: baca nama, cari di data, tampilkan detail peserta
 * atau pesan "tidak ditemukan". Tolak bila belum ada peserta terdaftar.
 */
void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;
    if (state->state == STATE_INIT) {
        tui_clear();
        attron(COLOR_PAIR(COLOR_ERROR));
        tui_print_centered(10, "[GAGAL] Daftar peserta dulu.");
        attroff(COLOR_PAIR(COLOR_ERROR));
        refresh();
        tui_getch();
        return;
    }

    tui_clear();

    /* Judul + prompt. */
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "CARI PESERTA");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    tui_box(3, 2, 56, 14);

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(4, 4, "Masukkan nama peserta:");
    attroff(COLOR_PAIR(COLOR_MENU));
    refresh();

    /* Baca nama. */
    char buffer[64];
    echo();
    curs_set(1);
    memset(buffer, 0, sizeof buffer);
    mvgetnstr(5, 4, buffer, 30);
    curs_set(0);
    noecho();

    /* Buang newline. */
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
        buffer[--len] = '\0';

    /* Nama kosong. */
    if (len == 0) {
        tui_clear();
        attron(COLOR_PAIR(COLOR_ERROR));
        tui_print_centered(10, "[GAGAL] Nama pencarian kosong.");
        attroff(COLOR_PAIR(COLOR_ERROR));
        refresh();
        tui_getch();
        return;
    }

    /* Cari peserta. */
    ParticipantNameVO name;
    strncpy(name.value, buffer, MAX_NAME_LENGTH);
    name.value[MAX_NAME_LENGTH] = '\0';

    SearchResultVO r;
    SearchError e = agent_search_find(agg, state, &name, &r);

    tui_clear();

    /* Hasil. */
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "HASIL PENCARIAN");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    tui_box(3, 2, 56, 14);

    if (e == SR_OK) {
        attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
        mvprintw(4, 4, "[DITEMUKAN]");
        attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);

        attron(COLOR_PAIR(COLOR_MENU));
        mvprintw(6, 4, "Nama       : %s", r.name);
        mvprintw(7, 4, "Total Skor : %d", r.total_score);
        mvprintw(8, 4, "Tendangan  : ");
        /* Tampilkan riwayat (- bila belum dilakukan). */
        int k;
        for (k = 0; k < TOTAL_KICKS; k++) {
            if (r.kicks[k] == -1) printw("- ");
            else printw("%d ", r.kicks[k]);
        }
        printw("\n");
        mvprintw(9, 4, "Zona       : ");
        int z;
        for (z = 0; z <= MAX_ZONE; z++) printw("Z%d:%d ", z, r.zone_freq[z]);
        attroff(COLOR_PAIR(COLOR_MENU));
    } else {
        attron(COLOR_PAIR(COLOR_ERROR));
        mvprintw(6, 4, "Peserta '%s' tidak ditemukan.", buffer);
        attroff(COLOR_PAIR(COLOR_ERROR));
    }

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(11, 4, "Tekan Enter untuk kembali...");
    attroff(COLOR_PAIR(COLOR_MENU));
    refresh();
    tui_getch();
}
