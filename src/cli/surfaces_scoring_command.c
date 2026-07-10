/**
 * @file surfaces_scoring_command.c
 * @brief Layar input tendangan: baca zona tiap peserta & catat skornya.
 */

#include "cli/module.cli.h"
#include "sanitizer/module.sanitizer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_WIDTH 58
#define BOX_HEIGHT 14
#define BOX_ROW 3
#define BOX_COL 2

/* Baca zona dari input pengguna; kembalikan SC_OK atau SC_INVALID_ZONE. */
static ScoringError read_zone(DisplayPort *dp, ZoneVO *out, char *raw_out, size_t raw_size) {
    char buf[32];
    /* Posisi -1 berarti gunakan kursor saat ini */
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

static void draw_scoring_screen(DisplayPort *dp, ParticipantEntity *part,
                                const char *msg, int msg_is_error) {
    char buf[128];
    /* A3: Hanya clear bila bukan pesan error, agar tidak flicker saat validasi zona. */
    if (!msg_is_error) dp->cls();

    /* Breadcrumb */
    dp->print_centered_colored(0, "Menu Utama > Input Tendangan", COLOR_DIM, 0);
    dp->print_centered_colored(1, "INPUT TENDANGAN DAN SKOR", COLOR_TITLE, 1);
    dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
    dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    snprintf(buf, sizeof buf, "Peserta: %s", part->name.value);
    dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_GOLD, 1, buf);

    int kick_pct = (part->kick_count.value * 100) / TOTAL_KICKS;
    snprintf(buf, sizeof buf, "Tendangan %d/%d: ", part->kick_count.value, TOTAL_KICKS);
    dp->draw_colored(BOX_ROW + 3, BOX_COL + 2, COLOR_MENU, 0, buf);
    dp->progress_bar(BOX_ROW + 3, BOX_COL + 18, 20, kick_pct, COLOR_SUCCESS);

    snprintf(buf, sizeof buf, "Skor sementara: %d poin", part->total_score.value);
    dp->draw_colored(BOX_ROW + 4, BOX_COL + 2, COLOR_WARNING, 1, buf);

    dp->separator(BOX_ROW + 5, BOX_COL, BOX_WIDTH);
    dp->draw_colored(BOX_ROW + 6, BOX_COL + 2, COLOR_MENU, 0, "Riwayat tendangan:");

    int k;
    for (k = 0; k < TOTAL_KICKS; k++) {
        int cx = BOX_COL + 4 + k * 4;
        int cy = BOX_ROW + 7;

        if (k < part->kick_count.value) {
            int zone = part->kicks[k].zone;
            int color = COLOR_DIM;
            if (zone >= 4) color = COLOR_SUCCESS;
            else if (zone >= 2) color = COLOR_WARNING;
            else if (zone == 0) color = COLOR_ERROR;
            snprintf(buf, sizeof buf, "Z%d ", zone);
            dp->draw_colored(cy, cx, color, 1, buf);
        } else if (k == part->kick_count.value) {
            dp->draw_colored(cy, cx, COLOR_INFO, 1, " -> ");
        } else {
            dp->draw_colored(cy, cx, COLOR_DIM, 0, " . ");
        }
    }

    dp->separator(BOX_ROW + 9, BOX_COL, BOX_WIDTH);
    dp->draw_colored(BOX_ROW + 10, BOX_COL + 2, COLOR_DIM, 0,
                     "Zona: 0=Miss 1=Mudah 2=Sedang 3=Sulit 4=SgtSulit 5=Top");

    snprintf(buf, sizeof buf, "Masukkan zona (0-%d, contoh: 5): ", MAX_ZONE);
    dp->draw_colored(BOX_ROW + 11, BOX_COL + 2, COLOR_INFO, 1, buf);

    if (msg != NULL && msg[0] != '\0') {
        int msg_color = msg_is_error ? COLOR_ERROR : COLOR_SUCCESS;
        dp->draw_colored(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, msg_color, 1, msg);
    }

    dp->screen_refresh();
}

void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state,
                                  DisplayPort *dp, SanitizeAggregate *sn) {
    char buf[128];
    if (agg == NULL || state == NULL) return;

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
            draw_scoring_screen(dp, part, NULL, 0);

            ZoneVO z;
            char raw[32] = "";
            read_zone(dp, &z, raw, sizeof raw);

            /* ESC = batal kembali ke menu */
            if (raw[0] == '\0' && part->kick_count.value == 0 && p == 0) {
                /* Kosong di awal = user mau kembali */
                if (!dp->confirm("Kembali ke menu? Progres yang belum tersimpan akan hilang."))
                    continue;
                cancelled = 1;
                break;
            }

            /* Validasi mentah via sanitizer agent sebelum domain agent */
            if (sn != NULL && raw[0] != '\0' &&
                agent_sanitize_validate_int(sn, raw, MIN_ZONE, MAX_ZONE) != SANITIZE_OK) {
                snprintf(buf, sizeof buf,
                         "[GAGAL] Input zona tidak valid: '%s'.", raw);
                dp->draw_colored(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, COLOR_ERROR, 1, buf);
                dp->screen_refresh();
                dp->readkey();
                continue;
            }

            if (z.value < MIN_ZONE || z.value > MAX_ZONE) {
                snprintf(buf, sizeof buf,
                         "[GAGAL] Zona harus %d-%d. Input: '%s'.", MIN_ZONE, MAX_ZONE, raw);
                dp->draw_colored(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, COLOR_ERROR, 1, buf);
                dp->screen_refresh();
                dp->readkey();
                continue;
            }

            ScoringError e = agent_scoring_record(agg, state, p, z);
            if (e == SC_OK) {
                snprintf(buf, sizeof buf, "[OK] Zona %d -> %d poin", z.value, z.value);
                draw_scoring_screen(dp, part, buf, 0);
            } else if (e == SC_INVALID_ZONE) {
                snprintf(buf, sizeof buf, "[GAGAL] Zona harus %d-%d. Anda memasukkan '%s'.",
                         MIN_ZONE, MAX_ZONE, raw);
                draw_scoring_screen(dp, part, buf, 1);
            } else {
                draw_scoring_screen(dp, part, "[GAGAL] Kesalahan pencatatan!", 1);
            }
            dp->screen_refresh();
            dp->readkey();
        }

        if (cancelled) break;

        dp->cls();
        dp->print_centered_colored(4, "[SELESAI]", COLOR_SUCCESS, 1);
        dp->box(6, BOX_COL, BOX_WIDTH, 6);

        snprintf(buf, sizeof buf, "Peserta: %s", part->name.value);
        dp->draw_colored(8, BOX_COL + 4, COLOR_GOLD, 1, buf);

        snprintf(buf, sizeof buf, "Total Skor: %d poin", part->total_score.value);
        dp->draw_colored(9, BOX_COL + 4, COLOR_SUCCESS, 1, buf);

        dp->footer("[ENTER] Lanjut");
        dp->screen_refresh();
        dp->readkey();
    }

    dp->cls();
    if (state->state == STATE_COMPLETED) {
        dp->print_centered_colored(4, "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90", COLOR_DIM, 0);
        dp->print_centered_colored(5, "SEMUA TENDANGAN SELESAI!", COLOR_SUCCESS, 1);
        dp->print_centered_colored(6, "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90", COLOR_DIM, 0);
    }
    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}
