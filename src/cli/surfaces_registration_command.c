/**
 * @file surfaces_registration_command.c
 * @brief Layar pendaftaran: baca nama peserta & simpan ke data lomba.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_WIDTH 56
#define BOX_ROW 3
#define BOX_COL 2

static size_t trim_spaces(char *str) {
    if (str == NULL) return 0;
    char *start = str;
    while (*start == ' ') start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    size_t len = strlen(str);
    while (len > 0 && str[len - 1] == ' ') str[--len] = '\0';
    return len;
}

static void show_error(const char *msg, int row) {
    attron(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
    mvprintw(row, BOX_COL + 2, "  [!] %s", msg);
    attroff(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
}

static void draw_registration_screen(CompetitionState *state) {
    tui_clear();

    int count = state->participant_count;
    int box_height = count + 13;

    tui_print_centered_colored(1, "PENDAFTARAN PESERTA", COLOR_TITLE, 1);
    tui_box(BOX_ROW, BOX_COL, BOX_WIDTH, box_height);
    tui_separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    int pct = (count * 100) / MAX_PARTICIPANTS;
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(BOX_ROW + 2, BOX_COL + 2, "Kuota peserta: %d / %d", count, MAX_PARTICIPANTS);
    attroff(COLOR_PAIR(COLOR_MENU));
    tui_progress_bar(BOX_ROW + 3, BOX_COL + 2, 30, pct, COLOR_SUCCESS);

    attron(COLOR_PAIR(COLOR_DIM));
    mvprintw(BOX_ROW + 5, BOX_COL + 2, "Ketik nama lalu Enter. Kosongkan untuk selesai.");
    attroff(COLOR_PAIR(COLOR_DIM));

    tui_separator(BOX_ROW + 6, BOX_COL, BOX_WIDTH);

    attron(COLOR_PAIR(COLOR_WARNING) | A_BOLD);
    mvprintw(BOX_ROW + 7, BOX_COL + 2, "Peserta terdaftar:");
    attroff(COLOR_PAIR(COLOR_WARNING) | A_BOLD);

    int i;
    for (i = 0; i < count; i++) {
        attron(COLOR_PAIR(COLOR_SUCCESS));
        mvprintw(BOX_ROW + 8 + i, BOX_COL + 4, "%d. %s", i + 1, state->participants[i].name.value);
        attroff(COLOR_PAIR(COLOR_SUCCESS));
    }

    tui_separator(BOX_ROW + 9 + count, BOX_COL, BOX_WIDTH);

    refresh();
}

void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;

    draw_registration_screen(state);

    int count = state->participant_count;
    int row = BOX_ROW + 10 + count;
    int error_row = BOX_ROW + count + 13 - 2;
    char buffer[64];

    while (state->participant_count < MAX_PARTICIPANTS) {
        count = state->participant_count;
        row = BOX_ROW + 10 + count;
        error_row = BOX_ROW + count + 13 - 2;

        attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
        mvprintw(row, BOX_COL + 2, "Nama peserta #%d: ", count + 1);
        attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
        refresh();

        echo();
        curs_set(1);
        memset(buffer, 0, sizeof buffer);
        mvgetnstr(row, BOX_COL + 21, buffer, 30);
        curs_set(0);
        noecho();

        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        trim_spaces(buffer);
        len = strlen(buffer);

        if (len == 0) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            show_error("Minimal 5 peserta untuk melanjutkan!", error_row);
            refresh();
            continue;
        }

        ParticipantNameVO name;
        strncpy(name.value, buffer, MAX_NAME_LENGTH);
        name.value[MAX_NAME_LENGTH] = '\0';

        RegistrationError e = agent_registration_add(agg, state, &name);
        if (e == REG_OK) {
            draw_registration_screen(state);

            attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
            mvprintw(error_row, BOX_COL + 2, "  [+] %s berhasil terdaftar!", name.value);
            attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
            refresh();
        } else {
            const char *emsg = "Kesalahan pendaftaran!";
            switch (e) {
                case REG_NAME_EMPTY:        emsg = "Nama tidak boleh kosong!"; break;
                case REG_NAME_TOO_LONG:     emsg = "Nama maksimal 30 karakter!"; break;
                case REG_NAME_INVALID_CHAR: emsg = "Nama hanya boleh huruf dan spasi!"; break;
                case REG_NAME_DUPLICATE:    emsg = "Nama sudah terdaftar!"; break;
                case REG_FULL:              emsg = "Kuota peserta penuh!"; break;
                default: break;
            }
            show_error(emsg, error_row);
            refresh();
        }
    }

    draw_registration_screen(state);
    int final_count = state->participant_count;
    int final_error_row = BOX_ROW + final_count + 13 - 2;

    attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
    mvprintw(final_error_row, BOX_COL + 2, "Total peserta: %d", final_count);
    attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);

    tui_footer("Tekan ENTER untuk kembali ke menu utama");
    refresh();
    tui_getch();
}
