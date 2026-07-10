/**
 * @file surfaces_search_command.c
 * @brief Layar cari: baca nama & tampilkan data peserta yang cocok.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_WIDTH 58
#define BOX_HEIGHT 16
#define BOX_ROW 3
#define BOX_COL 2

/**
 * Layar pencarian: baca nama, cari di data, tampilkan detail peserta
 * atau pesan "tidak ditemukan". Tolak bila belum ada peserta terdaftar.
 */
void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;
    if (state->state == STATE_INIT) {
        tui_clear();
        tui_print_centered_colored(10, "[GAGAL] Daftar peserta dulu (Menu 1).", COLOR_ERROR, 1);
        refresh();
        tui_getch();
        return;
    }

    tui_clear();

    /* Breadcrumb */
    tui_print_centered_colored(0, "Menu Utama > Cari Peserta", COLOR_DIM, 0);

    /* Judul */
    tui_print_centered_colored(1, "CARI PESERTA", COLOR_TITLE, 1);

    /* Bingkai */
    tui_box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);

    /* Separator */
    tui_separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    /* Instruksi */
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(BOX_ROW + 2, BOX_COL + 2, "Masukkan nama peserta yang dicari:");
    attroff(COLOR_PAIR(COLOR_MENU));

    /* Input field */
    attron(COLOR_PAIR(COLOR_INFO));
    mvprintw(BOX_ROW + 4, BOX_COL + 2, "Nama: ");
    attroff(COLOR_PAIR(COLOR_INFO));
    refresh();

    /* Baca nama */
    char buffer[64];
    echo();
    curs_set(1);
    memset(buffer, 0, sizeof buffer);
    mvgetnstr(BOX_ROW + 4, BOX_COL + 8, buffer, 30);
    curs_set(0);
    noecho();

    /* Buang newline */
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
        buffer[--len] = '\0';

    /* Nama kosong */
    if (len == 0) {
        tui_clear();
        tui_print_centered_colored(10, "[GAGAL] Nama pencarian kosong!", COLOR_ERROR, 1);
        refresh();
        tui_getch();
        return;
    }

    /* Cari peserta */
    ParticipantNameVO name;
    strncpy(name.value, buffer, MAX_NAME_LENGTH);
    name.value[MAX_NAME_LENGTH] = '\0';

    SearchResultVO r;
    SearchError e = agent_search_find(agg, state, &name, &r);

    /* Tampilkan hasil */
    tui_clear();

    if (e == SR_OK) {
        tui_print_centered_colored(1, "HASIL PENCARIAN", COLOR_TITLE, 1);
        tui_box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
        tui_separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

        /* Status ditemukan */
        attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
        mvprintw(BOX_ROW + 2, BOX_COL + 2, "[DITEMUKAN] Peserta ditemukan!");
        attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);

        /* Separator */
        tui_separator(BOX_ROW + 3, BOX_COL, BOX_WIDTH);

        /* Detail peserta */
        attron(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
        mvprintw(BOX_ROW + 4, BOX_COL + 4, "Nama        : %s", r.name);
        attroff(COLOR_PAIR(COLOR_GOLD) | A_BOLD);

        attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
        mvprintw(BOX_ROW + 5, BOX_COL + 4, "Total Skor  : %d poin", r.total_score);
        attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);

        /* Separator */
        tui_separator(BOX_ROW + 6, BOX_COL, BOX_WIDTH);

        /* Riwayat tendangan dengan visual */
        attron(COLOR_PAIR(COLOR_MENU));
        mvprintw(BOX_ROW + 7, BOX_COL + 2, "Riwayat Tendangan:");
        attroff(COLOR_PAIR(COLOR_MENU));

        int k;
        for (k = 0; k < TOTAL_KICKS; k++) {
            int cx = BOX_COL + 4 + k * 5;
            int cy = BOX_ROW + 8;

            if (r.kicks[k] == -1) {
                attron(COLOR_PAIR(COLOR_DIM));
                mvprintw(cy, cx, " - ");
                attroff(COLOR_PAIR(COLOR_DIM));
            } else {
                int zone = r.kicks[k];
                int color = COLOR_DIM;
                if (zone >= 4) color = COLOR_SUCCESS;
                else if (zone >= 2) color = COLOR_WARNING;
                else if (zone == 0) color = COLOR_ERROR;

                attron(COLOR_PAIR(color) | A_BOLD);
                mvprintw(cy, cx, "Z%d ", zone);
                attroff(COLOR_PAIR(color) | A_BOLD);
            }
        }

        /* Separator */
        tui_separator(BOX_ROW + 10, BOX_COL, BOX_WIDTH);

        /* Frekuensi zona */
        attron(COLOR_PAIR(COLOR_MENU));
        mvprintw(BOX_ROW + 11, BOX_COL + 2, "Frekuensi Zona:");
        attroff(COLOR_PAIR(COLOR_MENU));

        int z;
        for (z = 0; z <= MAX_ZONE; z++) {
            int cx = BOX_COL + 4 + z * 7;
            int cy = BOX_ROW + 12;
            attron(COLOR_PAIR(COLOR_MENU));
            mvprintw(cy, cx, "Z%d:%d", z, r.zone_freq[z]);
            attroff(COLOR_PAIR(COLOR_MENU));
        }
    } else {
        tui_print_centered_colored(1, "HASIL PENCARIAN", COLOR_TITLE, 1);
        tui_box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
        tui_separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

        attron(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
        mvprintw(BOX_ROW + 4, BOX_COL + 4, "[TIDAK DITEMUKAN]");
        attroff(COLOR_PAIR(COLOR_ERROR) | A_BOLD);

        attron(COLOR_PAIR(COLOR_MENU));
        mvprintw(BOX_ROW + 6, BOX_COL + 4, "Peserta '%s' tidak ditemukan.", buffer);
        attroff(COLOR_PAIR(COLOR_MENU));

        attron(COLOR_PAIR(COLOR_DIM));
        mvprintw(BOX_ROW + 8, BOX_COL + 4, "Pastikan nama yang dimasukkan benar.");
        attroff(COLOR_PAIR(COLOR_DIM));
    }

    tui_footer("Tekan ENTER untuk kembali ke menu utama");
    refresh();
    tui_getch();
}
