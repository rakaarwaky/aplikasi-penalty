#include "cli/module.cli.h"
#include "infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>
#include "shared/taxonomy_string_utility.h"

static void show_error(RegistrationError e) {
    const char *msg;
    switch (e) {
        case REG_NAME_EMPTY:        msg = "Nama tidak boleh kosong."; break;
        case REG_NAME_TOO_LONG:     msg = "Nama melebihi 30 karakter."; break;
        case REG_NAME_INVALID_CHAR: msg = "Nama hanya huruf dan spasi."; break;
        case REG_NAME_DUPLICATE:    msg = "Nama sudah terdaftar."; break;
        case REG_FULL:              msg = "Kuota penuh."; break;
        case REG_TOO_FEW:           msg = "Minimal 5 peserta."; break;
        default:                    msg = "Kesalahan pendaftaran.";
    }
    attron(COLOR_PAIR(COLOR_ERROR));
    mvprintw(20, 4, "[GAGAL] %s", msg);
    attroff(COLOR_PAIR(COLOR_ERROR));
}

void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;

    tui_clear();

    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "PENDAFTARAN PESERTA");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    tui_box(3, 2, 56, 16);

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(4, 4, "Kuota: 5 - 7 peserta. Ketik nama atau SELESAI.");
    mvprintw(5, 4, "Peserta terdaftar: %d", state->participant_count);
    attroff(COLOR_PAIR(COLOR_MENU));

    /* Show registered participants */
    int i;
    for (i = 0; i < state->participant_count; i++) {
        attron(COLOR_PAIR(COLOR_SUCCESS));
        mvprintw(7 + i, 4, "%d. %s", i + 1, state->participants[i].name.value);
        attroff(COLOR_PAIR(COLOR_SUCCESS));
    }

    /* Input area */
    int row = 7 + state->participant_count + 1;
    char buffer[64];
    while (state->participant_count < MAX_PARTICIPANTS) {
        attron(COLOR_PAIR(COLOR_MENU));
        mvprintw(row, 4, "Nama peserta #%d: ", state->participant_count + 1);
        attroff(COLOR_PAIR(COLOR_MENU));
        refresh();

        echo();
        curs_set(1);
        memset(buffer, 0, sizeof buffer);
        mvgetnstr(row, 22, buffer, 30);
        curs_set(0);
        noecho();

        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';

        if (len == 0) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            show_error(REG_TOO_FEW);
            continue;
        }

        /* Check for "selesai" */
        char lower[64];
        for (i = 0; i < (int)len; i++) lower[i] = buffer[i];
        lower[len] = '\0';
        for (i = 0; lower[i]; i++) {
            if (lower[i] >= 'A' && lower[i] <= 'Z') lower[i] += 32;
        }
        if (strcmp(lower, "selesai") == 0) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            show_error(REG_TOO_FEW);
            continue;
        }

        ParticipantNameVO name;
        strncpy(name.value, buffer, MAX_NAME_LENGTH);
        name.value[MAX_NAME_LENGTH] = '\0';

        RegistrationError e = agent_registration_add(agg, state, &name);
        if (e == REG_OK) {
            attron(COLOR_PAIR(COLOR_SUCCESS));
            mvprintw(row, 4, "[OK] %s terdaftar.              ", name.value);
            attroff(COLOR_PAIR(COLOR_SUCCESS));
            row++;
        } else {
            show_error(e);
        }
    }

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(22, 4, "Total peserta: %d. Tekan Enter untuk kembali.", state->participant_count);
    attroff(COLOR_PAIR(COLOR_MENU));
    refresh();
    tui_getch();
}
