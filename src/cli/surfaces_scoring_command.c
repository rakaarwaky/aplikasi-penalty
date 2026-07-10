/**
 * @file surfaces_scoring_command.c
 * @brief Smart surface: input loop tendangan & pencatatan skor.
 */

#include "cli/module.cli.h"
#include "sanitizer/module.sanitizer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_HEIGHT 14
#define BOX_ROW    4

static ScoringError read_zone(DisplayPort *dp, int row, int col,
                              ZoneVO *out, char *raw_out, size_t raw_size) {
    char buf[32];
    dp->input_string(row, col, buf, 10);

    /* Buang spasi/tab di awal & akhir agar " 5" atau "5 " tidak dibilang invalid. */
    char *p = buf;
    while (*p == ' ' || *p == '\t') p++;
    if (p != buf) memmove(buf, p, strlen(p) + 1);
    size_t L = strlen(buf);
    while (L > 0 && (buf[L - 1] == ' ' || buf[L - 1] == '\t' ||
                     buf[L - 1] == '\n' || buf[L - 1] == '\r'))
        buf[--L] = '\0';

    /* Salin versi yang sudah dibersihkan untuk dipakai pesan error/sanitizer. */
    if (raw_out != NULL && raw_size > 0)
        snprintf(raw_out, raw_size, "%s", buf);

    int i = 0;
    while (buf[i] != '\0') {
        if (!isdigit((unsigned char)buf[i]) && buf[i] != '-') return SC_INVALID_ZONE;
        i++;
    }
    int z;
    if (sscanf(buf, "%d", &z) != 1) return SC_INVALID_ZONE;
    out->value = z;
    return SC_OK;
}

static void draw_double_line(DisplayPort *dp, int row, int col, int width) {
    int i;
    for (i = 0; i < width; i++)
        dp->draw_at(row, col + i, "\xe2\x95\x90");
}

void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state,
                                  DisplayPort *dp, SanitizeAggregate *sn) {
    char buf[128];
    if (agg == NULL || state == NULL) return;

    int cols = dp->get_cols();
    int gw = cols - 4;
    if (gw > 64) gw = 64;
    if (gw < 40) gw = 40;
    int box_col = (cols - gw) / 2;

    if (state->state == STATE_INIT) {
        dp->cls();
        draw_double_line(dp, 0, 2, cols - 4);
        dp->print_centered_colored(1, "  INPUT TENDANGAN  ", COLOR_TITLE, 1);
        draw_double_line(dp, 2, 2, cols - 4);
        dp->box(4, box_col, gw, 6);
        dp->print_centered_colored(6, "[GAGAL] Daftar peserta dulu (Menu 1).", COLOR_ERROR, 1);
        dp->footer("[ENTER] Kembali ke menu");
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    if (state->state == STATE_COMPLETED) {
        dp->cls();
        draw_double_line(dp, 0, 2, cols - 4);
        dp->print_centered_colored(1, "  INPUT TENDANGAN  ", COLOR_TITLE, 1);
        draw_double_line(dp, 2, 2, cols - 4);
        dp->box(4, box_col, gw, 6);
        dp->print_centered_colored(6, "[INFO] Semua peserta sudah selesai melakukan tendangan.",
                                   COLOR_SUCCESS, 1);
        dp->footer("[ENTER] Kembali ke menu");
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    int p;
    int cancelled = 0;
    for (p = 0; p < state->participant_count && !cancelled; p++) {
        ParticipantEntity *part = &state->participants[p];
        while (part->kick_count.value < TOTAL_KICKS) {
            scoring_page_draw(dp, part, NULL, 0);

            ZoneVO z = { -1 };
            char raw[32] = "";
            int input_row = BOX_ROW + 11;
            char label[64];
            snprintf(label, sizeof label, "Masukkan zona (0-%d, contoh: 5): ", MAX_ZONE);
            int input_col = box_col + 2 + (int)strlen(label);
            read_zone(dp, input_row, input_col, &z, raw, sizeof raw);

            if (raw[0] == '\0') {
                if (dp->confirm("Kembali ke menu? Progres yang belum tersimpan akan hilang."))
                    cancelled = 1;
                break;
            }

            if (sn != NULL && raw[0] != '\0' &&
                agent_sanitize_validate_int(sn, raw, MIN_ZONE, MAX_ZONE) != SANITIZE_OK) {
                snprintf(buf, sizeof buf,
                         "[GAGAL] Input zona tidak valid: '%s'.", raw);
                dp->draw_colored(4 + BOX_HEIGHT - 2, box_col + 2, COLOR_ERROR, 1, buf);
                dp->screen_refresh();
                dp->readkey();
                continue;
            }

            if (z.value < MIN_ZONE || z.value > MAX_ZONE) {
                snprintf(buf, sizeof buf,
                         "[GAGAL] Zona harus %d-%d. Input: '%s'.", MIN_ZONE, MAX_ZONE, raw);
                dp->draw_colored(4 + BOX_HEIGHT - 2, box_col + 2, COLOR_ERROR, 1, buf);
                dp->screen_refresh();
                dp->readkey();
                continue;
            }

            ScoringError e = agent_scoring_record(agg, state, p, z);
            if (e == SC_OK) {
                snprintf(buf, sizeof buf, "[OK] Zona %d -> %d poin", z.value, z.value);
                scoring_page_draw(dp, part, buf, 0);
            } else if (e == SC_INVALID_ZONE) {
                snprintf(buf, sizeof buf, "[GAGAL] Zona harus %d-%d. Anda memasukkan '%s'.",
                         MIN_ZONE, MAX_ZONE, raw);
                scoring_page_draw(dp, part, buf, 1);
            } else {
                scoring_page_draw(dp, part, "[GAGAL] Kesalahan pencatatan!", 1);
            }
            dp->screen_refresh();
            dp->readkey();
        }

        if (cancelled) break;

        dp->cls();
        draw_double_line(dp, 0, 2, cols - 4);
        dp->print_centered_colored(1, "  INPUT TENDANGAN  ", COLOR_TITLE, 1);
        draw_double_line(dp, 2, 2, cols - 4);
        dp->box(4, box_col, gw, 6);
        dp->separator(5, box_col, gw);

        snprintf(buf, sizeof buf, "Peserta: %s", part->name.value);
        dp->draw_colored(6, box_col + 4, COLOR_GOLD, 1, buf);

        snprintf(buf, sizeof buf, "Total Skor: %d poin", part->total_score.value);
        dp->draw_colored(7, box_col + 4, COLOR_SUCCESS, 1, buf);

        dp->print_centered_colored(9, "[SELESAI]", COLOR_SUCCESS, 1);

        dp->footer("[ENTER] Lanjut");
        dp->screen_refresh();
        dp->readkey();
    }

    dp->cls();
    draw_double_line(dp, 0, 2, cols - 4);
    dp->print_centered_colored(1, "  INPUT TENDANGAN  ", COLOR_TITLE, 1);
    draw_double_line(dp, 2, 2, cols - 4);
    if (state->state == STATE_COMPLETED) {
        dp->box(4, box_col, gw, 6);
        dp->print_centered_colored(6, "SEMUA TENDANGAN SELESAI!", COLOR_SUCCESS, 1);
    }
    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}
