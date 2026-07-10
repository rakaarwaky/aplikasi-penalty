/**
 * @file surfaces_recap_command.c
 * @brief Layar rekap: tampilkan tabel lengkap seluruh peserta.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"
#include "export/module.export.h"

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

    /* Breadcrumb */
    tui_print_centered_colored(0, "Menu Utama > Rekapitulasi Lengkap", COLOR_DIM, 0);

    /* Judul dengan dekorasi */
    tui_print_centered_colored(1, "============================================", COLOR_GOLD, 1);
    tui_print_centered_colored(2, "REKAPITULASI LENGKAP", COLOR_TITLE, 1);
    tui_print_centered_colored(3, "============================================", COLOR_GOLD, 1);

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
        attroff(COLOR_PAIR(row_color) | (r->rank <= 3 ? A_BOLD : 0));

        /* B2: Cetak frekuensi zona dengan warna — zona 5 hijau, zona 0 merah, lainnya normal. */
        int z;
        for (z = 0; z <= MAX_ZONE; z++) {
            int zcolor;
            if (z == MAX_ZONE)  zcolor = COLOR_SUCCESS;
            else if (z == 0)    zcolor = COLOR_ERROR;
            else                zcolor = COLOR_MENU;
            int zbold = (z == MAX_ZONE) ? A_BOLD : 0;
            attron(COLOR_PAIR(zcolor) | zbold);
            printw("%d ", d->zone_freq[z]);
            attroff(COLOR_PAIR(zcolor) | zbold);
        }

        /* Garis pemisah antar baris */
        if (i < state->participant_count - 1) {
            tui_separator(row + 1, box_col, box_width);
        }
    }

    /* Separator footer */
    tui_separator(box_row + 4 + state->participant_count, box_col, box_width);

    /* Hitung statistik */
    int total_score_all = 0;
    int highest_score = 0;
    int i_stat;
    for (i_stat = 0; i_stat < state->participant_count; i_stat++) {
        total_score_all += ranking[i_stat].total_score;
        if (ranking[i_stat].total_score > highest_score)
            highest_score = ranking[i_stat].total_score;
    }
    int avg_score = (state->participant_count > 0) ? total_score_all / state->participant_count : 0;

    /* Ringkasan statistik */
    attron(COLOR_PAIR(COLOR_INFO));
    mvprintw(box_row + 5 + state->participant_count, box_col + 2,
             "Peserta: %d | Total Skor: %d | Rata-rata: %d | Tertinggi: %d",
             state->participant_count, total_score_all, avg_score, highest_score);
    attroff(COLOR_PAIR(COLOR_INFO));

    /* Tampilkan juara */
    if (state->participant_count > 0) {
        const char *winner = state->participants[ranking[0].participant_id].name.value;
        attron(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
        mvprintw(box_row + 6 + state->participant_count, box_col + 2,
                 "JUARA: %s dengan %d poin!", winner, ranking[0].total_score);
        attroff(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
    }

    /* Penjelasan tie-breaker */
    if (state->participant_count >= 2) {
        const RankingEntryVO *first = &ranking[0];
        const RankingEntryVO *second = &ranking[1];
        if (first->total_score == second->total_score) {
            const char *n1 = state->participants[first->participant_id].name.value;
            const char *n2 = state->participants[second->participant_id].name.value;
            attron(COLOR_PAIR(COLOR_INFO));
            mvprintw(box_row + 7 + state->participant_count, box_col + 2,
                     "Seri! %s & %s sama-sama %d poin.", n1, n2, first->total_score);
            int z;
            for (z = 5; z >= 0; z--) {
                if (first->zone_freq[z] != second->zone_freq[z]) {
                    mvprintw(box_row + 8 + state->participant_count, box_col + 2,
                             "%s menang: zona %d lebih banyak (%d vs %d)",
                             n1, z, first->zone_freq[z], second->zone_freq[z]);
                    break;
                }
            }
            attroff(COLOR_PAIR(COLOR_INFO));
        }
    }

    tui_footer("[ENTER] Kembali  [e] Ekspor ke file");

    refresh();

    /* Tunggu input user: ENTER atau 'e' untuk ekspor */
    int key;
    while ((key = tui_getch()) != TUI_KEY_ENTER) {
        if (key == 'e' || key == 'E') {
            /* Ekspor ke file (pakai wiring resmi root_export_build). */
            ExportAggregate exp_agg = root_export_build();
            ExportError exp_e = agent_export_ranking(&exp_agg, "ranking.txt", state, ranking, state->participant_count);
            if (exp_e == EXP_OK) {
                tui_clear();
                tui_print_centered_colored(10, "[OK] Berhasil diekspor ke ranking.txt", COLOR_SUCCESS, 1);
            } else {
                tui_clear();
                tui_print_centered_colored(10, "[GAGAL] Gagal mengekspor file!", COLOR_ERROR, 1);
            }
            tui_footer("[ENTER] Kembali");
            refresh();
            tui_getch();
            break;
        }
    }
}
