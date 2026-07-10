/**
 * @file surfaces_registration_command.c
 * @brief Layar pendaftaran: baca nama peserta & simpan ke data lomba.
 */

#include "cli/module.cli.h"
#include "sanitizer/module.sanitizer.h"

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

static void show_error(DisplayPort *dp, const char *msg, int row) {
    char buf[128];
    snprintf(buf, sizeof buf, "  [!] %s", msg);
    dp->draw_colored(row, BOX_COL + 2, COLOR_ERROR, 1, buf);
}

static void draw_registration_screen(DisplayPort *dp, CompetitionState *state) {
    char buf[128];
    dp->cls();

    int count = state->participant_count;
    int box_height = count + 13;
    int max_h = dp->get_lines() - 4;
    if (box_height > max_h) box_height = max_h;

    /* Breadcrumb — warna redup agar tidak mendominasi */
    dp->print_centered_colored(0, "Menu Utama > Pendaftaran Peserta", COLOR_DIM, 0);

    /* Header konsisten dengan Unicode solid */
    dp->print_centered_colored(1, "══════════════════════════════════", COLOR_DIM, 0);
    dp->print_centered_colored(2, "       PENDAFTARAN PESERTA        ", COLOR_TITLE, 1);
    dp->print_centered_colored(3, "══════════════════════════════════", COLOR_DIM, 0);

    dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, box_height);

    /* Progress bar — clean, tanpa label berlebih */
    int pct = (count * 100) / MAX_PARTICIPANTS;
    snprintf(buf, sizeof buf, "Kuota: %d/%d Terisi", count, MAX_PARTICIPANTS);
    dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_INFO, 0, buf);
    dp->progress_bar(BOX_ROW + 3, BOX_COL + 2, 30, pct, COLOR_SUCCESS);

    /* Garis pemisah tipis (Gestalt: Continuity) */
    dp->separator(BOX_ROW + 4, BOX_COL, BOX_WIDTH);

    /* Instruksi minimalis — kurangi Beban Kognitif */
    dp->draw_colored(BOX_ROW + 5, BOX_COL + 2, COLOR_DIM, 0,
                     "Ketik nama, Enter. Kosongkan untuk selesai.");

    dp->separator(BOX_ROW + 6, BOX_COL, BOX_WIDTH);

    /* Daftar Peserta */
    dp->draw_colored(BOX_ROW + 7, BOX_COL + 2, COLOR_MENU, 1, "Peserta Terdaftar:");

    int i;
    for (i = 0; i < count; i++) {
        snprintf(buf, sizeof buf, "%2d. %s", i + 1, state->participants[i].name.value);
        dp->draw_colored(BOX_ROW + 8 + i, BOX_COL + 4, COLOR_SUCCESS, 0, buf);
    }

    dp->separator(BOX_ROW + 9 + count, BOX_COL, BOX_WIDTH);
    dp->screen_refresh();
}

void cli_surfaces_registration_execute(RegistrationAggregate *agg,
                                       CompetitionState *state, DisplayPort *dp,
                                       SanitizeAggregate *sn) {
    if (agg == NULL || state == NULL) return;
    char buf[128];

    draw_registration_screen(dp, state);

    int count = state->participant_count;
    int row = BOX_ROW + 10 + count;
    int error_row = BOX_ROW + count + 13 - 2;
    char buffer[64];

    while (state->participant_count < MAX_PARTICIPANTS) {
        count = state->participant_count;
        row = BOX_ROW + 10 + count;
        error_row = BOX_ROW + count + 13 - 2;

        snprintf(buf, sizeof buf, "Nama peserta #%d (contoh: \"Budi Santoso\"): ", count + 1);
        dp->draw_colored(row, BOX_COL + 2, COLOR_INFO, 1, buf);
        dp->screen_refresh();

        /* Input baca melalui DisplayPort */
        dp->input_string(row, BOX_COL + 21, buffer, 30);

        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        trim_spaces(buffer);
        len = strlen(buffer);

        if (len == 0) {
            /* Kosong = selesai bila sudah cukup, atau kembali bila belum ada peserta */
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            if (state->participant_count == 0) {
                if (dp->confirm("Kembali ke menu utama?"))
                    return;
            }
            show_error(dp, "Minimal 5 peserta untuk melanjutkan!", error_row);
            dp->screen_refresh();
            continue;
        }

        /* Validasi mentah via sanitizer agent sebelum domain agent */
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
            draw_registration_screen(dp, state);
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

    draw_registration_screen(dp, state);
    int final_count = state->participant_count;
    int final_error_row = BOX_ROW + final_count + 13 - 2;

    snprintf(buf, sizeof buf, "Total peserta: %d", final_count);
    dp->draw_colored(final_error_row, BOX_COL + 2, COLOR_SUCCESS, 1, buf);

    dp->footer("[ENTER] Lanjut  \xe2\x94\x82  [q] Kembali");
    dp->screen_refresh();
    dp->readkey();
}
