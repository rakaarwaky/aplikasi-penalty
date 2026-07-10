/**
 * @file surfaces_ranking_page.c
 * @brief Dumb surface: render tabel ranking peserta.
 */

#include "cli/module.cli.h"

#include <stdio.h>

void ranking_page_draw(DisplayPort *dp, RankingEntryVO *entries,
                       CompetitionState *state, int max_score) {
    char buf[128];

    dp->cls();

    dp->print_centered_colored(0, "Menu Utama > Ranking Peserta", COLOR_DIM, 0);
    dp->print_centered_colored(1, UTF_DOUBLE_H_40, COLOR_DIM, 0);
    dp->print_centered_colored(2, "             RANKING PESERTA              ", COLOR_TITLE, 1);
    dp->print_centered_colored(3, UTF_DOUBLE_H_40, COLOR_DIM, 0);

    int box_height = state->participant_count + 8 + 2;
    int max_h = dp->get_lines() - 4;
    if (box_height > max_h) box_height = max_h;
    dp->box(4, 2, 64, box_height);

    snprintf(buf, sizeof buf, " %-4s " UTF_LIGHT_VLINE " %-6s " UTF_LIGHT_VLINE " %-20s " UTF_LIGHT_VLINE " %-5s " UTF_LIGHT_VLINE " %s",
             "No", "Medali", "Nama Peserta", "Skor", "Zona (5|4|3|2|1|0)");
    dp->draw_colored(5, 4, COLOR_INFO, 1, buf);
    dp->separator(6, 2, 64);

    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &entries[i];
        const char *name = state->participants[r->participant_id].name.value;
        int row = 8 + i * 2;

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

        if (r->rank > 3 && i % 2 == 0)
            row_color = COLOR_DIM;

        snprintf(buf, sizeof buf, " %-4d " UTF_LIGHT_VLINE " %-6s " UTF_LIGHT_VLINE " %-20s " UTF_LIGHT_VLINE " %-5d " UTF_LIGHT_VLINE " ",
                 r->rank, medal, name, r->total_score);
        dp->draw_colored(row, 4, row_color, (r->rank <= 3), buf);

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

        int bar_pct = (max_score > 0) ? (r->total_score * 100) / max_score : 0;
        dp->progress_bar(row + 1, 10, 30, bar_pct,
                         (r->rank == 1) ? COLOR_GOLD : COLOR_SUCCESS);

        if (i < state->participant_count - 1)
            dp->separator(row + 2, 2, 64);
    }

    int footer_sep_row = 8 + state->participant_count * 2;
    dp->separator(footer_sep_row, 2, 64);

    if (state->participant_count > 0) {
        const char *winner = state->participants[entries[0].participant_id].name.value;
        snprintf(buf, sizeof buf, "JUARA: %s dengan %d poin!", winner, entries[0].total_score);
        dp->draw_colored(footer_sep_row + 1, 4, COLOR_GOLD, 1, buf);

        if (state->participant_count >= 2) {
            const RankingEntryVO *first  = &entries[0];
            const RankingEntryVO *second = &entries[1];
            if (first->total_score == second->total_score) {
                const char *name2 = state->participants[second->participant_id].name.value;
                snprintf(buf, sizeof buf, "Seri! %s & %s sama-sama %d poin.",
                         winner, name2, first->total_score);
                dp->draw_colored(footer_sep_row + 2, 4, COLOR_INFO, 0, buf);
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
}
