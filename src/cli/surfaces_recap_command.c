/**
 * @file surfaces_recap_command.c
 * @brief Layar rekap: tampilkan tabel lengkap seluruh peserta.
 */

#include "cli/module.cli.h"

#include <stdio.h>

/**
 * Layar rekapitulasi: siapkan data (peringkat + detail) lalu tampilkan
 * tabel per peserta. Tolak bila lomba belum selesai.
 *
 * @param exp_agg  Aggregate ekspor (disiapkan oleh root container, bukan di sini).
 */
void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state,
                                DisplayPort *dp) {
    char buf[256];
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Tendangan belum selesai.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    /* Siapkan data peringkat & detail */
    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agent_recap_prepare(agg, state, ranking, details, MAX_PARTICIPANTS);
    if (e != RC_OK) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Rekap belum siap.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    dp->cls();

    /* Breadcrumb */
    dp->print_centered_colored(0, "Menu Utama > Rekapitulasi Lengkap", COLOR_DIM, 0);

    /* Judul dengan dekorasi (Unicode ═) */
    dp->print_centered_colored(1, "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90", COLOR_DIM, 0);
    dp->print_centered_colored(2, "REKAPITULASI LENGKAP", COLOR_TITLE, 1);
    dp->print_centered_colored(3, "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90", COLOR_DIM, 0);

    /* Dimensi & bingkai */
    int box_col = 1;
    int box_width = 66;
    int box_row = 4;
    int box_height = state->participant_count + 8;

    dp->box(box_row, box_col, box_width, box_height);

    /* Header kolom */
    dp->separator(box_row + 1, box_col, box_width);
    snprintf(buf, sizeof buf, " %-4s %-22s %-6s %s",
             "No", "Nama", "Skor", "Zona(0 1 2 3 4 5)");
    dp->draw_colored(box_row + 2, box_col + 2, COLOR_INFO, 1, buf);
    dp->separator(box_row + 3, box_col, box_width);

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

        snprintf(buf, sizeof buf, " %-4d %-22s %-6d ", r->rank, d->name, d->total_score);
        dp->draw_colored(row, box_col + 2, row_color, (r->rank <= 3), buf);

        /* B2: Frekuensi zona dengan warna — zona 5 hijau, zona 0 merah. */
        int z;
        for (z = 0; z <= MAX_ZONE; z++) {
            int zcolor;
            if (z == MAX_ZONE)  zcolor = COLOR_SUCCESS;
            else if (z == 0)    zcolor = COLOR_ERROR;
            else                zcolor = COLOR_MENU;
            int zbold = (z == MAX_ZONE);
            snprintf(buf, sizeof buf, "%d ", d->zone_freq[z]);
            dp->draw_append_colored(zcolor, zbold, buf);
        }

        /* Garis pemisah antar baris */
        if (i < state->participant_count - 1) {
            dp->separator(row + 1, box_col, box_width);
        }
    }

    /* Separator footer */
    dp->separator(box_row + 4 + state->participant_count, box_col, box_width);

    /* Hitung statistik */
    int total_score_all = 0;
    int highest_score   = 0;
    int i_stat;
    for (i_stat = 0; i_stat < state->participant_count; i_stat++) {
        total_score_all += ranking[i_stat].total_score;
        if (ranking[i_stat].total_score > highest_score)
            highest_score = ranking[i_stat].total_score;
    }
    int avg_score = (state->participant_count > 0)
                    ? total_score_all / state->participant_count : 0;

    /* Ringkasan statistik */
    snprintf(buf, sizeof buf,
             "Peserta: %d | Total Skor: %d | Rata-rata: %d | Tertinggi: %d",
             state->participant_count, total_score_all, avg_score, highest_score);
    dp->draw_colored(box_row + 5 + state->participant_count, box_col + 2,
                     COLOR_INFO, 0, buf);

    /* Tampilkan juara */
    if (state->participant_count > 0) {
        const char *winner = state->participants[ranking[0].participant_id].name.value;
        snprintf(buf, sizeof buf, "JUARA: %s dengan %d poin!", winner, ranking[0].total_score);
        dp->draw_colored(box_row + 6 + state->participant_count, box_col + 2,
                         COLOR_GOLD, 1, buf);
    }

    /* Penjelasan tie-breaker */
    if (state->participant_count >= 2) {
        const RankingEntryVO *first  = &ranking[0];
        const RankingEntryVO *second = &ranking[1];
        if (first->total_score == second->total_score) {
            const char *n1 = state->participants[first->participant_id].name.value;
            const char *n2 = state->participants[second->participant_id].name.value;
            snprintf(buf, sizeof buf, "Seri! %s & %s sama-sama %d poin.",
                     n1, n2, first->total_score);
            dp->draw_colored(box_row + 7 + state->participant_count, box_col + 2,
                             COLOR_INFO, 0, buf);
            int z;
            for (z = 5; z >= 0; z--) {
                if (first->zone_freq[z] != second->zone_freq[z]) {
                    snprintf(buf, sizeof buf,
                             "%s menang: zona %d lebih banyak (%d vs %d)",
                             n1, z, first->zone_freq[z], second->zone_freq[z]);
                    dp->draw_colored(box_row + 8 + state->participant_count, box_col + 2,
                                     COLOR_INFO, 0, buf);
                    break;
                }
            }
        }
    }

    dp->footer("[ENTER] Kembali");
    dp->screen_refresh();
    dp->readkey();
}
