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
        tui_print_centered_colored(10, "[GAGAL] Tendangan belum selesai.", COLOR_ERROR, 1);
        refresh();
        tui_getch();
        return;
    }

    /* Siapkan data peringkat & detail */
    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agent_recap_prepare(agg, state, ranking, details, MAX_PARTICIPANTS);
    if (e != RC_OK) {
        tui_clear();
        tui_print_centered_colored(10, "[GAGAL] Rekap belum siap.", COLOR_ERROR, 1);
        refresh();
        tui_getch();
        return;
    }

    tui_clear();

    /* Judul dengan dekorasi */
    tui_print_centered_colored(0, "============================================", COLOR_GOLD, 1);
    tui_print_centered_colored(1, "REKAPITULASI LENGKAP", COLOR_TITLE, 1);
    tui_print_centered_colored(2, "============================================", COLOR_GOLD, 1);

    /* Dimensi & bingkai */
    int box_col = 1;
    int box_width = 66;
    int box_row = 4;
    int box_height = state->participant_count + 8;

    tui_box(box_row, box_col, box_width, box_height);

    /* Header kolom dengan background */
    tui_separator(box_row + 1, box_col, box_width);
    attron(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    mvprintw(box_row + 2, box_col + 2, " %-4s %-22s %-6s %s", "No", "Nama", "Skor", "Zona(0 1 2 3 4 5)");
    attroff(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    tui_separator(box_row + 3, box_col, box_width);

    /* Satu baris per peserta */
    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &ranking[i];
        const SearchResultVO *d = &details[r->participant_id];
        int row = box_row + 4 + i;

        /* Pilih warna berdasarkan peringkat */
        int row_color = COLOR_MENU;
        if (r->rank == 1) row_color = COLOR_GOLD;
        else if (r->rank == 2) row_color = COLOR_SILVER;
        else if (r->rank == 3) row_color = COLOR_BRONZE;
        else if (i % 2 == 1) row_color = COLOR_DIM;

        attron(COLOR_PAIR(row_color) | (r->rank <= 3 ? A_BOLD : 0));
        mvprintw(row, box_col + 2, " %-4d %-22s %-6d ", r->rank, d->name, d->total_score);
        int z;
        for (z = 0; z <= MAX_ZONE; z++) printw("%d ", d->zone_freq[z]);
        attroff(COLOR_PAIR(row_color) | (r->rank <= 3 ? A_BOLD : 0));

        /* Garis pemisah antar baris */
        if (i < state->participant_count - 1) {
            tui_separator(row + 1, box_col, box_width);
        }
    }

    /* Separator footer */
    tui_separator(box_row + 4 + state->participant_count, box_col, box_width);

    /* Ringkasan */
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(box_row + 5 + state->participant_count, box_col + 2,
             "Total peserta: %d", state->participant_count);
    attroff(COLOR_PAIR(COLOR_MENU));

    /* Tampilkan juara */
    if (state->participant_count > 0) {
        const char *winner = state->participants[ranking[0].participant_id].name.value;
        attron(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
        mvprintw(box_row + 6 + state->participant_count, box_col + 2,
                 "JUARA: %s dengan %d poin!", winner, ranking[0].total_score);
        attroff(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
    }

    tui_footer("Tekan ENTER untuk kembali ke menu utama");

    refresh();
    tui_getch();
}
