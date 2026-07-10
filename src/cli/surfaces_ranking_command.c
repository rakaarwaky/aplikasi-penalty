/**
 * @file surfaces_ranking_command.c
 * @brief Layar peringkat: hitung & tampilkan urutan peserta.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>

/**
 * Layar ranking: hitung peringkat lalu tampilkan tabel
 * (peringkat, nama, skor, frekuensi zona). Tolak bila lomba belum selesai.
 */
void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        tui_clear();
        tui_print_centered_colored(10, "[GAGAL] Tendangan belum selesai.", COLOR_ERROR, 1);
        refresh();
        tui_getch();
        return;
    }

    /* Hitung peringkat */
    RankingEntryVO entries[MAX_PARTICIPANTS];
    RankingError e = agent_ranking_compute(agg, state, entries, MAX_PARTICIPANTS);
    if (e != RK_OK) {
        tui_clear();
        tui_print_centered_colored(10, "[GAGAL] Ranking belum siap.", COLOR_ERROR, 1);
        refresh();
        tui_getch();
        return;
    }

    tui_clear();

    /* Breadcrumb */
    tui_print_centered_colored(0, "Menu Utama > Ranking Peserta", COLOR_DIM, 0);

    /* Judul dengan dekorasi */
    tui_print_centered_colored(1, "============================================", COLOR_GOLD, 1);
    tui_print_centered_colored(2, "RANKING PESERTA", COLOR_TITLE, 1);
    tui_print_centered_colored(3, "============================================", COLOR_GOLD, 1);

    /* Bingkai */
    int box_height = state->participant_count + 8;
    tui_box(4, 2, 64, box_height);

    /* Separator header */
    tui_separator(5, 2, 64);

    /* Header kolom dengan warna */
    attron(COLOR_PAIR(COLOR_HEADER) | A_BOLD);
    mvprintw(6, 4, " %-5s %-6s %-22s %-6s %s", "No", "Medali", "Nama", "Skor", "Zona(5|4|3|2|1|0)");
    attroff(COLOR_PAIR(COLOR_HEADER) | A_BOLD);

    /* Separator setelah header */
    tui_separator(7, 2, 64);

    /* Satu baris per peserta */
    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &entries[i];
        const char *name = state->participants[r->participant_id].name.value;
        int row = 8 + i;

        /* Pilih warna berdasarkan peringkat */
        int row_color = COLOR_MENU;
        const char *medal = "";
        if (r->rank == 1) {
            row_color = COLOR_GOLD;
            medal = "  *1* ";
        } else if (r->rank == 2) {
            row_color = COLOR_SILVER;
            medal = "  *2* ";
        } else if (r->rank == 3) {
            row_color = COLOR_BRONZE;
            medal = "  *3* ";
        }

        /* Zebra stripe untuk baris selain juara */
        if (r->rank > 3 && i % 2 == 0)
            row_color = COLOR_DIM;

        attron(COLOR_PAIR(row_color) | (r->rank <= 3 ? A_BOLD : 0));
        mvprintw(row, 4, " %-5d %-6s %-22s %-6d %d|%d|%d|%d|%d|%d",
                 r->rank, medal, name, r->total_score,
                 r->zone_freq[5], r->zone_freq[4], r->zone_freq[3],
                 r->zone_freq[2], r->zone_freq[1], r->zone_freq[0]);
        attroff(COLOR_PAIR(row_color) | (r->rank <= 3 ? A_BOLD : 0));

        /* Bar skor relatif terhadap skor tertinggi. */
        int max_score = entries[0].total_score;
        int score_pct = (max_score > 0) ? (r->total_score * 100) / max_score : 0;
        tui_progress_bar(row, 52, 10, score_pct, COLOR_SUCCESS);

        /* Separator antar baris */
        if (i < state->participant_count - 1) {
            tui_separator(row + 1, 2, 64);
        }
    }

    /* Footer */
    tui_separator(8 + state->participant_count, 2, 64);

    /* Tampilkan juara jika ada */
    if (state->participant_count > 0) {
        const char *winner = state->participants[entries[0].participant_id].name.value;
        attron(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
        mvprintw(8 + state->participant_count + 1, 4, "JUARA: %s dengan %d poin!", winner, entries[0].total_score);
        attroff(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
    }

    tui_footer("Tekan ENTER untuk kembali ke menu utama");

    refresh();
    tui_getch();
}
