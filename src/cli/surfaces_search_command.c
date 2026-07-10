/**
 * @file surfaces_search_command.c
 * @brief Smart surface: input nama & delegasi pencarian ke agent.
 */

#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void draw_double_line(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++)
        dp->draw_at(row, col + i, "\xe2\x95\x90");
}

static void show_error_screen(DisplayPort *dp, const char *title,
                              const char *msg, int color) {
    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > 60) gw = 60;
    if (gw < 40) gw = 40;
    int box_col = (cols - gw) / 2;
    char hdr[64];
    snprintf(hdr, sizeof hdr, "  %s  ", title);

    dp->cls();
    draw_double_line(dp, 0, 2, cols - 4);
    dp->print_centered_colored(1, hdr, COLOR_TITLE, 1);
    draw_double_line(dp, 2, 2, cols - 4);
    dp->box(4, box_col, gw, 6);
    dp->print_centered_colored(6, msg, color, 1);
    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}

void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state,
                                 DisplayPort *dp) {
    if (agg == NULL || state == NULL) return;
    if (state->state == STATE_INIT) {
        show_error_screen(dp, "CARI PESERTA",
                          "[GAGAL] Daftar peserta dulu (Menu 1).", COLOR_ERROR);
        return;
    }

    search_page_draw_input(dp);

    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > 64) gw = 64;
    if (gw < 40) gw = 40;
    int box_col = (cols - gw) / 2;

    char buffer[64];
    dp->input_string(BOX_ROW + 4, box_col + 8, buffer, 30);

    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
        buffer[--len] = '\0';
    char *start = buffer;
    while (*start == ' ') start++;
    if (start != buffer) {
        memmove(buffer, start, strlen(start) + 1);
        len = strlen(buffer);
    }
    while (len > 0 && buffer[len - 1] == ' ') buffer[--len] = '\0';

    if (len == 1 && buffer[0] == 'q') return;

    if (len == 0) {
        show_error_screen(dp, "CARI PESERTA",
                          "[GAGAL] Nama pencarian kosong!", COLOR_ERROR);
        return;
    }

    ParticipantNameVO name;
    strncpy(name.value, buffer, MAX_NAME_LENGTH);
    name.value[MAX_NAME_LENGTH] = '\0';

    SearchResultVO r;
    SearchError e = agent_search_find(agg, state, &name, &r);

    if (e == SR_OK) {
        search_page_draw_found(dp, &r);
    } else {
        search_page_draw_not_found(dp, buffer, state);
    }
}
