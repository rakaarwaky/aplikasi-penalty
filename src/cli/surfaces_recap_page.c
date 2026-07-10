/**
 * @file surfaces_recap_page.c
 * @brief Dumb surface: render tabel rekapitulasi lengkap.
 */

#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>

static void draw_double_line(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++)
        dp->draw_at(row, col + i, "\xe2\x95\x90");
}

void recap_page_draw(DisplayPort *dp, RankingEntryVO *ranking,
                     SearchResultVO *details, CompetitionState *state,
                     int total_score, int avg_score, int highest_score,
                     ExportAggregate *ex) {
    char buf[256];
    dp->cls();

    int cols = dp->get_cols();

    int gw = cols - 4;
    if (gw > BOX_WIDTH_RECAP) gw = BOX_WIDTH_RECAP;
    if (gw < BOX_WIDTH_MIN) gw = BOX_WIDTH_MIN;
    int box_col = (cols - gw) / 2;
    int box_row = BOX_ROW;

    int box_height = state->participant_count + 10;
    int max_h = dp->get_lines() - 6;
    if (box_height > max_h) box_height = max_h;

    dp->print_centered_colored(0, "Menu Utama > Rekapitulasi Lengkap", COLOR_DIM, 0);
    draw_double_line(dp, 1, 2, cols - 4);
    dp->print_centered_colored(2, "           REKAPITULASI LENGKAP           ", COLOR_TITLE, 1);
    draw_double_line(dp, 3, 2, cols - 4);

    dp->box(box_row, box_col, gw, box_height);

    dp->separator(box_row + 1, box_col, gw);
    snprintf(buf, sizeof buf, " %-4s %-22s %-6s %s",
             "No", "Nama", "Skor", "Zona(0 1 2 3 4 5)");
    dp->draw_colored(box_row + 2, box_col + 2, COLOR_INFO, 1, buf);
    dp->separator(box_row + 3, box_col, gw);

    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &ranking[i];
        const SearchResultVO *d = &details[r->participant_id];
        int row = box_row + 4 + i;

        int row_color = COLOR_MENU;
        if (r->rank == 1) row_color = COLOR_GOLD;
        else if (r->rank == 2) row_color = COLOR_SILVER;
        else if (r->rank == 3) row_color = COLOR_BRONZE;
        else if (i % 2 == 0) row_color = COLOR_DIM;

        snprintf(buf, sizeof buf, " %-4d %-22s %-6d ", r->rank, d->name, d->total_score);
        dp->draw_colored(row, box_col + 2, row_color, (r->rank <= 3), buf);

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

        if (i < state->participant_count - 1)
            dp->separator(row + 1, box_col, gw);
    }

    dp->separator(box_row + 4 + state->participant_count, box_col, gw);

    snprintf(buf, sizeof buf,
             "Peserta: %d | Total Skor: %d | Rata-rata: %d | Tertinggi: %d",
             state->participant_count, total_score, avg_score, highest_score);
    dp->draw_colored(box_row + 5 + state->participant_count, box_col + 2,
                     COLOR_INFO, 0, buf);

    if (state->participant_count > 0) {
        const char *winner = state->participants[ranking[0].participant_id].name.value;
        snprintf(buf, sizeof buf, "JUARA: %s dengan %d poin!", winner, ranking[0].total_score);
        dp->draw_colored(box_row + 6 + state->participant_count, box_col + 2,
                         COLOR_GOLD, 1, buf);
    }

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

    dp->footer("[ENTER] Kembali  \xe2\x94\x82  [E] Export ke File");
    dp->screen_refresh();

    for (;;) {
        int key = dp->readkey();
        if (key == '\n' || key == TUI_KEY_ENTER) break;
        if ((key == 'e' || key == 'E') && ex != NULL) {
            char filename[64] = "rekap.txt";
            dp->footer("Nama file (ENTER = rekap.txt):");
            dp->screen_refresh();
            dp->input_string(dp->get_lines() - 1, 30, filename, 50);
            if (strlen(filename) == 0) strcpy(filename, "rekap.txt");

            ExportError err = agent_export_recap(ex, filename, state, ranking,
                                                 details, state->participant_count,
                                                 total_score, avg_score,
                                                 highest_score);
            dp->cls();
            if (err == EXP_OK) {
                char msg[128];
                snprintf(msg, sizeof msg, "Berhasil diekspor ke: %s", filename);
                dp->print_centered_colored(6, msg, COLOR_SUCCESS, 1);
            } else {
                dp->print_centered_colored(6, "[GAGAL] Export gagal!", COLOR_ERROR, 1);
            }
            dp->footer("[ENTER] Kembali ke menu");
            dp->screen_refresh();
            dp->readkey();
            break;
        }
    }
}
