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

static ScoringError read_zone(DisplayPort *dp, ZoneVO *out, char *raw_out, size_t raw_size) {
    char buf[32];
    dp->input_string(-1, -1, buf, 10);

    if (raw_out != NULL && raw_size > 0)
        snprintf(raw_out, raw_size, "%s", buf);

    int i = 0;
    while (buf[i] != '\0' && buf[i] != '\n') {
        if (!isdigit((unsigned char)buf[i]) && buf[i] != '-') return SC_INVALID_ZONE;
        i++;
    }
    int z;
    if (sscanf(buf, "%d", &z) != 1) return SC_INVALID_ZONE;
    out->value = z;
    return SC_OK;
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
        dp->print_centered_colored(10, "[GAGAL] Daftar peserta dulu (Menu 1).", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    if (state->state == STATE_COMPLETED) {
        dp->cls();
        dp->print_centered_colored(10, "[INFO] Semua peserta sudah selesai melakukan tendangan.",
                                   COLOR_SUCCESS, 1);
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
            read_zone(dp, &z, raw, sizeof raw);

            if (raw[0] == '\0' && part->kick_count.value == 0 && p == 0) {
                if (!dp->confirm("Kembali ke menu? Progres yang belum tersimpan akan hilang."))
                    continue;
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
        dp->print_centered_colored(4, "[SELESAI]", COLOR_SUCCESS, 1);
        dp->box(6, box_col, gw, 6);

        snprintf(buf, sizeof buf, "Peserta: %s", part->name.value);
        dp->draw_colored(8, box_col + 4, COLOR_GOLD, 1, buf);

        snprintf(buf, sizeof buf, "Total Skor: %d poin", part->total_score.value);
        dp->draw_colored(9, box_col + 4, COLOR_SUCCESS, 1, buf);

        dp->footer("[ENTER] Lanjut");
        dp->screen_refresh();
        dp->readkey();
    }

    dp->cls();
    if (state->state == STATE_COMPLETED) {
        dp->print_centered_colored(4, UTF_DOUBLE_H_16, COLOR_DIM, 0);
        dp->print_centered_colored(5, "SEMUA TENDANGAN SELESAI!", COLOR_SUCCESS, 1);
        dp->print_centered_colored(6, UTF_DOUBLE_H_16, COLOR_DIM, 0);
    }
    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}
