#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>

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

    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "REKAPITULASI LENGKAP");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    tui_box(3, 1, 64, state->participant_count + 4);

    attron(COLOR_PAIR(COLOR_HIGHLIGHT));
    mvprintw(4, 2, "%-4s %-24s %-6s %s", "Rank", "Nama", "Skor", "Zona(0 1 2 3 4 5)");
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT));

    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &ranking[i];
        const SearchResultVO *d = &details[r->participant_id];
        int row = 5 + i;

        if (i % 2 == 0)
            attron(COLOR_PAIR(COLOR_MENU));
        else
            attron(COLOR_PAIR(COLOR_BORDER));

        mvprintw(row, 2, "%-4d %-24s %-6d ", r->rank, d->name, d->total_score);
        int z;
        for (z = 0; z <= MAX_ZONE; z++) printw("%d ", d->zone_freq[z]);

        attroff(COLOR_PAIR(COLOR_MENU));
        attroff(COLOR_PAIR(COLOR_BORDER));
    }

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(5 + state->participant_count + 2, 2, "Tekan Enter untuk kembali...");
    attroff(COLOR_PAIR(COLOR_MENU));

    refresh();
    tui_getch();
}
