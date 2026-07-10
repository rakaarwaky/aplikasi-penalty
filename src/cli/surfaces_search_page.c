/**
 * @file surfaces_search_page.c
 * @brief Dumb surface: render hasil pencarian peserta.
 */

#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_HEIGHT 16

static void draw_double_line(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++)
        dp->draw_at(row, col + i, "\xe2\x95\x90");
}

void search_page_draw_input(DisplayPort *dp) {
    dp->cls();

    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > BOX_WIDTH_MAX) gw = BOX_WIDTH_MAX;
    if (gw < BOX_WIDTH_MIN) gw = BOX_WIDTH_MIN;
    int box_col = (cols - gw) / 2;
    int box_row = BOX_ROW;

    dp->print_centered_colored(0, "Menu Utama > Cari Peserta", COLOR_DIM, 0);
    draw_double_line(dp, 1, 2, cols - 4);
    dp->print_centered_colored(2, "          CARI PESERTA          ", COLOR_TITLE, 1);
    draw_double_line(dp, 3, 2, cols - 4);

    dp->box(box_row, box_col, gw, BOX_HEIGHT);
    dp->separator(box_row + 1, box_col, gw);

    dp->draw_colored(box_row + 2, box_col + 2, COLOR_MENU, 0,
                     "Masukkan nama peserta yang dicari:");
    dp->draw_colored(box_row + 4, box_col + 2, COLOR_INFO, 0, "Nama: ");
    dp->footer("[ENTER] Cari  \xe2\x94\x82  [q] Kembali");
    dp->screen_refresh();
}

void search_page_draw_found(DisplayPort *dp, SearchResultVO *r,
                            ExportAggregate *ex) {
    char buf[128];
    dp->cls();

    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > BOX_WIDTH_MAX) gw = BOX_WIDTH_MAX;
    if (gw < BOX_WIDTH_MIN) gw = BOX_WIDTH_MIN;
    int box_col = (cols - gw) / 2;
    int box_row = BOX_ROW;

    dp->print_centered_colored(0, "Menu Utama > Cari Peserta", COLOR_DIM, 0);
    draw_double_line(dp, 1, 2, cols - 4);
    dp->print_centered_colored(2, "          HASIL PENCARIAN          ", COLOR_TITLE, 1);
    draw_double_line(dp, 3, 2, cols - 4);

    dp->box(box_row, box_col, gw, BOX_HEIGHT);
    dp->separator(box_row + 1, box_col, gw);

    dp->draw_colored(box_row + 2, box_col + 2, COLOR_SUCCESS, 1,
                     "[DITEMUKAN] Peserta ditemukan!");
    dp->separator(box_row + 3, box_col, gw);

    snprintf(buf, sizeof buf, "Nama        : %s", r->name);
    dp->draw_colored(box_row + 4, box_col + 4, COLOR_GOLD, 1, buf);

    snprintf(buf, sizeof buf, "Total Skor  : %d poin", r->total_score);
    dp->draw_colored(box_row + 5, box_col + 4, COLOR_SUCCESS, 1, buf);

    dp->separator(box_row + 6, box_col, gw);

    dp->draw_colored(box_row + 7, box_col + 2, COLOR_MENU, 0, "Riwayat Tendangan:");

    int k;
    for (k = 0; k < TOTAL_KICKS; k++) {
        int cx = box_col + 4 + k * 5;
        int cy = box_row + 8;

        if (r->kicks[k] == -1) {
            dp->draw_colored(cy, cx, COLOR_DIM, 0, " - ");
        } else {
            int zone = r->kicks[k];
            int color = COLOR_DIM;
            if (zone >= 4) color = COLOR_SUCCESS;
            else if (zone >= 2) color = COLOR_WARNING;
            else if (zone == 0) color = COLOR_ERROR;
            snprintf(buf, sizeof buf, "Z%d ", zone);
            dp->draw_colored(cy, cx, color, 1, buf);
        }
    }

    dp->separator(box_row + 10, box_col, gw);

    dp->draw_colored(box_row + 11, box_col + 2, COLOR_MENU, 0, "Frekuensi Zona:");

    int z;
    for (z = 0; z <= MAX_ZONE; z++) {
        int cx = box_col + 4 + z * 7;
        int cy = box_row + 12;
        snprintf(buf, sizeof buf, "Z%d:%d", z, r->zone_freq[z]);
        dp->draw_colored(cy, cx, COLOR_MENU, 0, buf);
    }

    dp->footer("[ENTER] Kembali  \xe2\x94\x82  [E] Export ke File");
    dp->screen_refresh();

    for (;;) {
        int key = dp->readkey();
        if (key == '\n' || key == TUI_KEY_ENTER) break;
        if ((key == 'e' || key == 'E') && ex != NULL) {
            char defname[64];
            snprintf(defname, sizeof defname, "peserta_%s.txt", r->name);
            char filename[64];
            strncpy(filename, defname, sizeof filename - 1);
            filename[sizeof filename - 1] = '\0';

            char prompt[128];
            snprintf(prompt, sizeof prompt, "Nama file (ENTER = %s):", defname);
            dp->footer(prompt);
            dp->screen_refresh();
            dp->input_string(dp->get_lines() - 1, 30, filename, 50);
            if (strlen(filename) == 0) strcpy(filename, defname);

            ExportError err = agent_export_participant(ex, filename, r);
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

void search_page_draw_not_found(DisplayPort *dp, const char *query,
                                CompetitionState *state) {
    char buf[128];
    dp->cls();

    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > BOX_WIDTH_MAX) gw = BOX_WIDTH_MAX;
    if (gw < BOX_WIDTH_MIN) gw = BOX_WIDTH_MIN;
    int box_col = (cols - gw) / 2;
    int box_row = BOX_ROW;

    dp->print_centered_colored(0, "Menu Utama > Cari Peserta", COLOR_DIM, 0);
    draw_double_line(dp, 1, 2, cols - 4);
    dp->print_centered_colored(2, "          HASIL PENCARIAN          ", COLOR_TITLE, 1);
    draw_double_line(dp, 3, 2, cols - 4);

    dp->box(box_row, box_col, gw, BOX_HEIGHT);
    dp->separator(box_row + 1, box_col, gw);

    dp->draw_colored(box_row + 4, box_col + 4, COLOR_ERROR, 1, "[TIDAK DITEMUKAN]");

    snprintf(buf, sizeof buf, "Peserta '%s' tidak ditemukan.", query);
    dp->draw_colored(box_row + 6, box_col + 4, COLOR_MENU, 0, buf);

    int suggestions = 0;
    int si;
    for (si = 0; si < state->participant_count && suggestions < 3; si++) {
        const char *pname = state->participants[si].name.value;
        int matched = 0;
        int qlen = (int)strlen(query);
        size_t pi;
        for (pi = 0; pname[pi] != '\0'; pi++) {
            char lower_p = (char)tolower((unsigned char)pname[pi]);
            size_t bi;
            for (bi = 0; query[bi] != '\0'; bi++) {
                if (tolower((unsigned char)query[bi]) == lower_p) {
                    matched++;
                    break;
                }
            }
        }
        if (qlen > 0 && matched * 2 >= qlen) {
            snprintf(buf, sizeof buf, "- %s", pname);
            dp->draw_colored(box_row + 9 + suggestions, box_col + 6,
                             COLOR_SUCCESS, 0, buf);
            suggestions++;
        }
    }
    if (suggestions == 0) {
        dp->draw_colored(box_row + 9, box_col + 6, COLOR_DIM, 0,
                         "(tidak ada saran serupa)");
    }

    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}
