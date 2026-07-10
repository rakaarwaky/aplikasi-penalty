/**
 * @file surfaces_recap_command.c
 * @brief Layar rekap: tampilkan tabel lengkap seluruh peserta.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>

/**
 * Layar rekapitulasi: siapkan data (peringkat + detail) lalu tampilkan
 * tabel per peserta. Tolak bila lomba belum selesai.
 */
void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        tui_clear();
        attron(COLOR_PAIR(COLOR_ERROR));
        tui_print_centered(10, "[GAGAL] Tendangan belum selesai.");
        attroff(COLOR_PAIR(COLOR_ERROR));
        refresh();
        tui_getch();
        return;
    }

    /* Siapkan data peringkat & detail. */
    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agent_recap_prepare(agg, state, ranking, details, MAX_PARTICIPANTS);
    if (e != RC_OK) {
        tui_clear();
        attron(COLOR_PAIR(COLOR_ERROR));
        tui_print_centered(10, "[GAGAL] Rekap belum siap.");
        attroff(COLOR_PAIR(COLOR_ERROR));
        refresh();
        tui_getch();
        return;
    }

    tui_clear();

    /* Judul. */
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "REKAPITULASI LENGKAP");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    /* Dimensi & bingkai luar. */
    int box_col = 1;
    int box_width = 64;
    int box_row = 3;
    int box_height = state->participant_count + 6;

    tui_box(box_row, box_col, box_width, box_height);

    /* Header kolom. */
    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    mvprintw(box_row + 1, box_col + 2, "%-4s %-24s %-6s %s", "Rank", "Nama", "Skor", "Zona(0 1 2 3 4 5)");
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));

    /* Garis pemisah header. */
    attron(COLOR_PAIR(COLOR_BORDER));
    mvaddch(box_row + 2, box_col, ACS_LTEE);
    int c;
    for (c = 1; c < box_width - 1; c++) mvaddch(box_row + 2, box_col + c, ACS_HLINE);
    mvaddch(box_row + 2, box_col + box_width - 1, ACS_RTEE);
    attroff(COLOR_PAIR(COLOR_BORDER));

    /* Satu baris per peserta. */
    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &ranking[i];
        const SearchResultVO *d = &details[r->participant_id];
        int row = box_row + 3 + i;

        if (i % 2 == 0)
            attron(COLOR_PAIR(COLOR_MENU));
        else
            attron(COLOR_PAIR(COLOR_BORDER));

        mvprintw(row, box_col + 2, "%-4d %-24s %-6d ", r->rank, d->name, d->total_score);
        int z;
        for (z = 0; z <= MAX_ZONE; z++) printw("%d ", d->zone_freq[z]);

        attroff(COLOR_PAIR(COLOR_MENU));
        attroff(COLOR_PAIR(COLOR_BORDER));

        /* Garis pemisah antar baris (kecuali baris terbawah). */
        if (i < state->participant_count - 1) {
            attron(COLOR_PAIR(COLOR_BORDER));
            mvaddch(row + 1, box_col, ACS_LTEE);
            for (c = 1; c < box_width - 1; c++) mvaddch(row + 1, box_col + c, ACS_HLINE);
            mvaddch(row + 1, box_col + box_width - 1, ACS_RTEE);
            attroff(COLOR_PAIR(COLOR_BORDER));
        }
    }

    /* Penutup. */
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(box_row + box_height - 1, box_col + 2, "Tekan Enter untuk kembali...");
    attroff(COLOR_PAIR(COLOR_MENU));

    refresh();
    tui_getch();
}
