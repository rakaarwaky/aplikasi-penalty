/**
 * @file surfaces_search_command.c
 * @brief Smart surface: input nama & delegasi pencarian ke agent.
 */

#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_ROW 3
#define BOX_COL 2

void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state,
                                 DisplayPort *dp) {
    if (agg == NULL || state == NULL) return;
    if (state->state == STATE_INIT) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Daftar peserta dulu (Menu 1).", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    search_page_draw_input(dp);

    char buffer[64];
    dp->input_string(BOX_ROW + 4, BOX_COL + 8, buffer, 30);

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

    if (len == 0) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Nama pencarian kosong!", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
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
