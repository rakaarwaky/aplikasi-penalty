/**
 * @file surfaces_ranking_command.c
 * @brief Layar peringkat: hitung & tampilkan urutan peserta.
 */

#include "cli/module.cli.h"

#include <stdio.h>

/**
 * Layar ranking: hitung peringkat lalu tampilkan tabel
 * (peringkat, nama, skor, frekuensi zona). Tolak bila lomba belum selesai.
 */
void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state,
                                  DisplayPort *dp) {
    char buf[128];
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        dp->clear();
        dp->print_centered_colored(10, "[GAGAL] Tendangan belum selesai.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->getch();
        return;
    }

    /* Hitung peringkat */
    RankingEntryVO entries[MAX_PARTICIPANTS];
    RankingError e = agent_ranking_compute(agg, state, entries, MAX_PARTICIPANTS);
    if (e != RK_OK) {
        dp->clear();
        dp->print_centered_colored(10, "[GAGAL] Ranking belum siap.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->getch();
        return;
    }

    dp->clear();

    /* Breadcrumb */
    dp->print_centered_colored(0, "Menu Utama > Ranking Peserta", COLOR_DIM, 0);

    /* Judul dengan dekorasi */
    dp->print_centered_colored(1, "============================================", COLOR_GOLD, 1);
    dp->print_centered_colored(2, "RANKING PESERTA", COLOR_TITLE, 1);
    dp->print_centered_colored(3, "============================================", COLOR_GOLD, 1);

    /* Bingkai */
    int box_height = state->participant_count + 8 + 2; /* +2 untuk baris bar chart */
    dp->box(4, 2, 64, box_height);

    /* Separator header */
    dp->separator(5, 2, 64);

    /* Header kolom dengan warna */
    snprintf(buf, sizeof buf, " %-5s %-6s %-22s %-6s %s",
             "No", "Medali", "Nama", "Skor", "Zona(5|4|3|2|1|0)");
    dp->draw_colored(6, 4, COLOR_HEADER, 1, buf);

    /* Separator setelah header */
    dp->separator(7, 2, 64);

    /* Hitung skor maksimum untuk normalisasi bar chart (B1). */
    int max_score = 1;
    {
        int ms;
        for (ms = 0; ms < state->participant_count; ms++)
            if (entries[ms].total_score > max_score) max_score = entries[ms].total_score;
    }

    /* Satu baris per peserta */
    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &entries[i];
        const char *name = state->participants[r->participant_id].name.value;
        int row = 8 + i * 2; /* *2 karena tiap peserta 2 baris: teks + bar */

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

        snprintf(buf, sizeof buf, " %-5d %-6s %-22s %-6d ",
                 r->rank, medal, name, r->total_score);
        dp->draw_colored(row, 4, row_color, (r->rank <= 3), buf);

        /* B2: Cetak frekuensi zona dengan warna berbeda. */
        int z;
        for (z = 5; z >= 0; z--) {
            int zcolor;
            if (z == 5)      zcolor = COLOR_SUCCESS;
            else if (z == 0) zcolor = COLOR_ERROR;
            else             zcolor = COLOR_MENU;
            int zbold = (z == 5);
            snprintf(buf, sizeof buf, "%d", r->zone_freq[z]);
            dp->draw_append_colored(zcolor, zbold, buf);
            if (z > 0) dp->draw_append("|");
        }

        /* B1: Bar chart skor di baris kedua. */
        int bar_pct = (max_score > 0) ? (r->total_score * 100) / max_score : 0;
        dp->progress_bar(row + 1, 10, 30, bar_pct,
                         (r->rank == 1) ? COLOR_GOLD : COLOR_SUCCESS);

        /* Separator antar peserta */
        if (i < state->participant_count - 1) {
            dp->separator(row + 2, 2, 64);
        }
    }

    /* Footer */
    int footer_sep_row = 8 + state->participant_count * 2;
    dp->separator(footer_sep_row, 2, 64);

    /* Tampilkan juara jika ada */
    if (state->participant_count > 0) {
        const char *winner = state->participants[entries[0].participant_id].name.value;
        snprintf(buf, sizeof buf, "JUARA: %s dengan %d poin!", winner, entries[0].total_score);
        dp->draw_colored(footer_sep_row + 1, 4, COLOR_GOLD, 1, buf);

        /* Penjelasan tie-breaker untuk posisi 1 vs 2 */
        if (state->participant_count >= 2) {
            const RankingEntryVO *first  = &entries[0];
            const RankingEntryVO *second = &entries[1];
            if (first->total_score == second->total_score) {
                const char *name2 = state->participants[second->participant_id].name.value;
                snprintf(buf, sizeof buf, "Seri! %s & %s sama-sama %d poin.",
                         winner, name2, first->total_score);
                dp->draw_colored(footer_sep_row + 2, 4, COLOR_INFO, 0, buf);
                /* Cari zona penentu */
                int z;
                for (z = 5; z >= 0; z--) {
                    if (first->zone_freq[z] != second->zone_freq[z]) {
                        snprintf(buf, sizeof buf,
                                 "%s menang: zona %d lebih banyak (%d vs %d)",
                                 winner, z, first->zone_freq[z], second->zone_freq[z]);
                        dp->draw_colored(footer_sep_row + 3, 4, COLOR_INFO, 0, buf);
                        break;
                    }
                }
            }
        }
    }

    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->getch();
}
