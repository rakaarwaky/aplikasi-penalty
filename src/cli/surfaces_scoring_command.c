/**
 * @file surfaces_scoring_command.c
 * @brief Layar input tendangan: baca zona tiap peserta & catat skornya.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_WIDTH 58
#define BOX_HEIGHT 14
#define BOX_ROW 3
#define BOX_COL 2

static ScoringError read_zone(ZoneVO *out, char *raw_out, size_t raw_size) {
    char buf[32];
    echo();
    curs_set(1);
    memset(buf, 0, sizeof buf);
    getnstr(buf, 10);
    curs_set(0);
    noecho();

    /* Simpan input mentah untuk pesan error */
    if (raw_out != NULL && raw_size > 0) {
        strncpy(raw_out, buf, raw_size - 1);
        raw_out[raw_size - 1] = '\0';
    }

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

static void draw_scoring_screen(ParticipantEntity *part, const char *msg, int msg_is_error) {
    /* A3: Hanya clear bila bukan pesan error, agar tidak flicker saat validasi zona. */
    if (!msg_is_error) tui_clear();

    /* Breadcrumb */
    tui_print_centered_colored(0, "Menu Utama > Input Tendangan", COLOR_DIM, 0);
    tui_print_centered_colored(1, "INPUT TENDANGAN DAN SKOR", COLOR_TITLE, 1);
    tui_box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
    tui_separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    attron(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
    mvprintw(BOX_ROW + 2, BOX_COL + 2, "Peserta: %s", part->name.value);
    attroff(COLOR_PAIR(COLOR_GOLD) | A_BOLD);

    int kick_pct = (part->kick_count * 100) / TOTAL_KICKS;
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(BOX_ROW + 3, BOX_COL + 2, "Tendangan %d/%d: ", part->kick_count, TOTAL_KICKS);
    attroff(COLOR_PAIR(COLOR_MENU));
    tui_progress_bar(BOX_ROW + 3, BOX_COL + 18, 20, kick_pct, COLOR_SUCCESS);

    attron(COLOR_PAIR(COLOR_WARNING) | A_BOLD);
    mvprintw(BOX_ROW + 4, BOX_COL + 2, "Skor sementara: %d poin", part->total_score);
    attroff(COLOR_PAIR(COLOR_WARNING) | A_BOLD);

    tui_separator(BOX_ROW + 5, BOX_COL, BOX_WIDTH);

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(BOX_ROW + 6, BOX_COL + 2, "Riwayat tendangan:");
    attroff(COLOR_PAIR(COLOR_MENU));

    int k;
    for (k = 0; k < TOTAL_KICKS; k++) {
        int cx = BOX_COL + 4 + k * 4;
        int cy = BOX_ROW + 7;

        if (k < part->kick_count) {
            int zone = part->kicks[k];
            int color = COLOR_DIM;
            if (zone >= 4) color = COLOR_SUCCESS;
            else if (zone >= 2) color = COLOR_WARNING;
            else if (zone == 0) color = COLOR_ERROR;

            attron(COLOR_PAIR(color) | A_BOLD);
            mvprintw(cy, cx, "Z%d ", zone);
            attroff(COLOR_PAIR(color) | A_BOLD);
        } else if (k == part->kick_count) {
            attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
            mvprintw(cy, cx, " -> ");
            attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
        } else {
            attron(COLOR_PAIR(COLOR_DIM));
            mvprintw(cy, cx, " . ");
            attroff(COLOR_PAIR(COLOR_DIM));
        }
    }

    tui_separator(BOX_ROW + 9, BOX_COL, BOX_WIDTH);

    attron(COLOR_PAIR(COLOR_DIM));
    mvprintw(BOX_ROW + 10, BOX_COL + 2, "Zona: 0=Miss 1=Mudah 2=Sedang 3=Sulit 4=SkSlt 5=Top");
    attroff(COLOR_PAIR(COLOR_DIM));

    attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
    mvprintw(BOX_ROW + 11, BOX_COL + 2, "Masukkan zona (0-%d, contoh: 5): ", MAX_ZONE);
    attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);

    if (msg != NULL && msg[0] != '\0') {
        int msg_color = msg_is_error ? COLOR_ERROR : COLOR_SUCCESS;
        attron(COLOR_PAIR(msg_color) | A_BOLD);
        mvprintw(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, "%s", msg);
        attroff(COLOR_PAIR(msg_color) | A_BOLD);
    }

    refresh();
}

void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;

    if (state->state == STATE_INIT) {
        tui_clear();
        tui_print_centered_colored(10, "[GAGAL] Daftar peserta dulu (Menu 1).", COLOR_ERROR, 1);
        refresh();
        tui_getch();
        return;
    }

    if (state->state == STATE_COMPLETED) {
        tui_clear();
        tui_print_centered_colored(10, "[INFO] Semua peserta sudah selesai melakukan tendangan.", COLOR_SUCCESS, 1);
        refresh();
        tui_getch();
        return;
    }

    int p;
    for (p = 0; p < state->participant_count; p++) {
        ParticipantEntity *part = &state->participants[p];
        while (part->kick_count < TOTAL_KICKS) {
            draw_scoring_screen(part, NULL, 0);

            ZoneVO z;
            char raw[32] = "";
            if (read_zone(&z, raw, sizeof raw) != SC_OK || z.value < MIN_ZONE || z.value > MAX_ZONE) {
                /* Tampilkan error di baris feedback saja, tanpa clear layar. */
                char err_msg[96];
                snprintf(err_msg, sizeof err_msg,
                         "[GAGAL] Zona harus %d-%d. Input: '%s'.", MIN_ZONE, MAX_ZONE, raw);
                attron(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
                mvprintw(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, "%s", err_msg);
                attroff(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
                refresh();
                tui_getch();
                continue;
            }

            ScoringError e = agent_scoring_record(agg, state, p, z);
            if (e == SC_OK) {
                char ok_msg[64];
                snprintf(ok_msg, sizeof ok_msg, "[OK] Zona %d -> %d poin", z.value, z.value);
                draw_scoring_screen(part, ok_msg, 0);
            } else if (e == SC_INVALID_ZONE) {
                char err_msg[96];
                snprintf(err_msg, sizeof err_msg, "[GAGAL] Zona harus %d-%d. Anda memasukkan '%s'.", MIN_ZONE, MAX_ZONE, raw_input);
                draw_scoring_screen(part, err_msg, 1);
            } else {
                draw_scoring_screen(part, "[GAGAL] Kesalahan pencatatan!", 1);
            }
            refresh();
            tui_getch();
        }

        tui_clear();
        tui_print_centered_colored(4, "[SELESAI]", COLOR_SUCCESS, 1);

        tui_box(6, BOX_COL, BOX_WIDTH, 6);

        attron(COLOR_PAIR(COLOR_GOLD) | A_BOLD);
        mvprintw(8, BOX_COL + 4, "Peserta: %s", part->name.value);
        attroff(COLOR_PAIR(COLOR_GOLD) | A_BOLD);

        attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
        mvprintw(9, BOX_COL + 4, "Total Skor: %d poin", part->total_score);
        attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);

        tui_footer("[ENTER] Lanjut");
        refresh();
        tui_getch();
    }

    tui_clear();
    if (state->state == STATE_COMPLETED) {
        tui_print_centered_colored(4, "* * * * * * * * * * * * * * * *", COLOR_GOLD, 1);
        tui_print_centered_colored(5, "SEMUA TENDANGAN SELESAI!", COLOR_SUCCESS, 1);
        tui_print_centered_colored(6, "* * * * * * * * * * * * * * * *", COLOR_GOLD, 1);
    }
    tui_footer("[ENTER] Kembali ke menu");
    refresh();
    tui_getch();
}
