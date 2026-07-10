/**
 * @file surfaces_registration_command.c
 * @brief Smart surface: input loop pendaftaran peserta.
 */

#include "cli/module.cli.h"
#include "sanitizer/module.sanitizer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static size_t trim_spaces(char *str) {
    if (str == NULL) return 0;
    char *start = str;
    while (*start == ' ') start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    size_t len = strlen(str);
    while (len > 0 && str[len - 1] == ' ') str[--len] = '\0';
    return len;
}

static void get_box_dims(DisplayPort *dp, int *out_col, int *out_w) {
    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > 64) gw = 64;
    if (gw < 40) gw = 40;
    *out_col = (cols - gw) / 2;
    *out_w = gw;
}

static void clear_row(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++) dp->draw_at(row, col + i, " ");
}

static void show_status(DisplayPort *dp, const char *msg, int color,
                        int row, int box_col, int gw) {
    clear_row(dp, row, box_col + 2, gw - 4);
    char buf[128];
    snprintf(buf, sizeof buf, "  %s", msg);
    dp->draw_colored(row, box_col + 2, color, 1, buf);
}

void cli_surfaces_registration_execute(RegistrationAggregate *agg,
                                       CompetitionState *state, DisplayPort *dp,
                                       SanitizeAggregate *sn) {
    if (agg == NULL || state == NULL) return;
    char buf[128];

    registration_page_draw(dp, state);

    int box_col, gw;
    get_box_dims(dp, &box_col, &gw);
    int count = state->participant_count;
    int row = registration_page_input_row(count);
    int error_row = registration_page_error_row();
    char buffer[64];

    while (state->participant_count < MAX_PARTICIPANTS) {
        count = state->participant_count;
        row = registration_page_input_row(count);
        error_row = registration_page_error_row();

        snprintf(buf, sizeof buf, "Nama #%d: ", count + 1);
        dp->draw_colored(row, box_col + 2, COLOR_INFO, 1, buf);
        dp->screen_refresh();

        dp->input_string(row, box_col + 12, buffer, 30);

        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        trim_spaces(buffer);
        len = strlen(buffer);

        if (len == 0) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            show_status(dp, "[GAGAL] Minimal 5 peserta untuk melanjutkan!",
                        COLOR_ERROR, error_row, box_col, gw);
            dp->screen_refresh();
            continue;
        }

        if (len == 1 && buffer[0] == 'q') break;

        if (sn != NULL && agent_sanitize_validate_string(sn, buffer, MAX_NAME_LENGTH) != SANITIZE_OK) {
            show_status(dp, "[GAGAL] Input nama tidak valid!",
                        COLOR_ERROR, error_row, box_col, gw);
            dp->screen_refresh();
            continue;
        }

        ParticipantNameVO name;
        strncpy(name.value, buffer, MAX_NAME_LENGTH);
        name.value[MAX_NAME_LENGTH] = '\0';

        RegistrationError e = agent_registration_add(agg, state, &name);
        if (e == REG_OK) {
            registration_page_draw(dp, state);
            snprintf(buf, sizeof buf, "[OK] %s berhasil terdaftar!", name.value);
            show_status(dp, buf, COLOR_SUCCESS, error_row, box_col, gw);
            dp->screen_refresh();
        } else {
            const char *emsg = "[GAGAL] Kesalahan pendaftaran!";
            switch (e) {
                case REG_NAME_EMPTY:        emsg = "[GAGAL] Nama tidak boleh kosong!"; break;
                case REG_NAME_TOO_LONG:     emsg = "[GAGAL] Nama maksimal 30 karakter!"; break;
                case REG_NAME_INVALID_CHAR: emsg = "[GAGAL] Nama hanya boleh huruf dan spasi!"; break;
                case REG_NAME_DUPLICATE:    emsg = "[GAGAL] Nama sudah terdaftar!"; break;
                case REG_FULL:              emsg = "[GAGAL] Kuota peserta penuh!"; break;
                default: break;
            }
            show_status(dp, emsg, COLOR_ERROR, error_row, box_col, gw);
            dp->screen_refresh();
        }
    }

    registration_page_draw(dp, state);
    int final_count = state->participant_count;
    int final_error_row = registration_page_error_row();

    snprintf(buf, sizeof buf, "[OK] Total peserta: %d", final_count);
    show_status(dp, buf, COLOR_SUCCESS, final_error_row, box_col, gw);

    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}
