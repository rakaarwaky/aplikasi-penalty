/**
 * @file surfaces_registration_command.c
 * @brief Smart surface: input loop pendaftaran peserta.
 */

#include "cli/module.cli.h"
#include "cli/surfaces_registration_page.h"
#include "sanitizer/module.sanitizer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

static void show_error(DisplayPort *dp, const char *msg, int row) {
    char buf[128];
    snprintf(buf, sizeof buf, "  [!] %s", msg);
    dp->draw_colored(row, BOX_COL + 2, COLOR_ERROR, 1, buf);
}

void cli_surfaces_registration_execute(RegistrationAggregate *agg,
                                       CompetitionState *state, DisplayPort *dp,
                                       SanitizeAggregate *sn) {
    if (agg == NULL || state == NULL) return;
    char buf[128];

    registration_page_draw(dp, state);

    int count = state->participant_count;
    int row = registration_page_input_row(count);
    int error_row = registration_page_error_row(count);
    char buffer[64];

    while (state->participant_count < MAX_PARTICIPANTS) {
        count = state->participant_count;
        row = registration_page_input_row(count);
        error_row = registration_page_error_row(count);

        snprintf(buf, sizeof buf, "Nama peserta #%d (contoh: \"Budi Santoso\"): ", count + 1);
        dp->draw_colored(row, BOX_COL + 2, COLOR_INFO, 1, buf);
        dp->screen_refresh();

        dp->input_string(row, BOX_COL + 21, buffer, 30);

        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        trim_spaces(buffer);
        len = strlen(buffer);

        if (len == 0) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            if (state->participant_count == 0) {
                if (dp->confirm("Kembali ke menu utama?"))
                    return;
            }
            show_error(dp, "Minimal 5 peserta untuk melanjutkan!", error_row);
            dp->screen_refresh();
            continue;
        }

        if (sn != NULL && agent_sanitize_validate_string(sn, buffer, MAX_NAME_LENGTH) != SANITIZE_OK) {
            show_error(dp, "Input nama tidak valid!", error_row);
            dp->screen_refresh();
            continue;
        }

        ParticipantNameVO name;
        strncpy(name.value, buffer, MAX_NAME_LENGTH);
        name.value[MAX_NAME_LENGTH] = '\0';

        RegistrationError e = agent_registration_add(agg, state, &name);
        if (e == REG_OK) {
            registration_page_draw(dp, state);
            snprintf(buf, sizeof buf, "  [+] %s berhasil terdaftar!", name.value);
            dp->draw_colored(error_row, BOX_COL + 2, COLOR_SUCCESS, 1, buf);
            dp->screen_refresh();
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
            show_error(dp, emsg, error_row);
            dp->screen_refresh();
        }
    }

    registration_page_draw(dp, state);
    int final_count = state->participant_count;
    int final_error_row = registration_page_error_row(final_count);

    snprintf(buf, sizeof buf, "Total peserta: %d", final_count);
    dp->draw_colored(final_error_row, BOX_COL + 2, COLOR_SUCCESS, 1, buf);

    dp->footer("[ENTER] Lanjut  \xe2\x94\x82  [q] Kembali");
    dp->screen_refresh();
    dp->readkey();
}
