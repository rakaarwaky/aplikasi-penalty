/**
 * @file surfaces_ranking_command.c
 * @brief Surface: layar tampilkan ranking (I/O ncurses + delegasi ke agent).
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>

/**
 * Layar ranking: delegasikan perhitungan ke agent_ranking_compute(),
 * lalu render tabel (rank, nama, skor, frekuensi zona 5..0).
 * Guard state: blokir bila belum STATE_COMPLETED.
 */
void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state) {
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

    /* Hitung ranking via agent. */
    RankingEntryVO entries[MAX_PARTICIPANTS];
    RankingError e = agent_ranking_compute(agg, state, entries, MAX_PARTICIPANTS);
    if (e != RK_OK) {
        tui_clear();
        attron(COLOR_PAIR(COLOR_ERROR));
        tui_print_centered(10, "[GAGAL] Ranking belum siap.");
        attroff(COLOR_PAIR(COLOR_ERROR));
        refresh();
        tui_getch();
        return;
    }

    tui_clear();

    /* Judul. */
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "RANKING PESERTA");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    /* Bingkai + header kolom. */
    tui_box(3, 2, 62, state->participant_count + 4);

    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    mvprintw(4, 4, "%-5s %-25s %-6s %s", "Rank", "Nama", "Skor", "Zona(5|4|3|2|1|0)");
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));

    /* Baris per peserta (warna selang-seling). */
    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &entries[i];
        const char *name = state->participants[r->participant_id].name.value;
        int row = 5 + i;

        if (i % 2 == 0)
            attron(COLOR_PAIR(COLOR_MENU));
        else
            attron(COLOR_PAIR(COLOR_BORDER));

        mvprintw(row, 4, "%-5d %-25s %-6d %d|%d|%d|%d|%d|%d",
                 r->rank, name, r->total_score,
                 r->zone_freq[5], r->zone_freq[4], r->zone_freq[3],
                 r->zone_freq[2], r->zone_freq[1], r->zone_freq[0]);

        attroff(COLOR_PAIR(COLOR_MENU));
        attroff(COLOR_PAIR(COLOR_BORDER));
    }

    /* Penutup. */
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(5 + state->participant_count + 2, 4, "Tekan Enter untuk kembali...");
    attroff(COLOR_PAIR(COLOR_MENU));

    refresh();
    tui_getch();
}
