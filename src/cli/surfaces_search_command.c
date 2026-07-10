/**
 * @file surfaces_search_command.c
 * @brief Layar cari: baca nama & tampilkan data peserta yang cocok.
 */

#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_WIDTH 58
#define BOX_HEIGHT 16
#define BOX_ROW 3
#define BOX_COL 2

/**
 * Layar pencarian: baca nama, cari di data, tampilkan detail peserta
 * atau pesan "tidak ditemukan". Tolak bila belum ada peserta terdaftar.
 */
void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state,
                                 DisplayPort *dp) {
    char buf[128];
    if (agg == NULL || state == NULL) return;
    if (state->state == STATE_INIT) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Daftar peserta dulu (Menu 1).", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    dp->cls();

    /* Breadcrumb — warna redup */
    dp->print_centered_colored(0, "Menu Utama > Cari Peserta", COLOR_DIM, 0);

    /* Header konsisten dengan UTF-8 escape sequences */
    dp->print_centered_colored(1, UTF_DOUBLE_H_32, COLOR_DIM, 0);
    dp->print_centered_colored(2, "          CARI PESERTA          ", COLOR_TITLE, 1);
    dp->print_centered_colored(3, UTF_DOUBLE_H_32, COLOR_DIM, 0);

    /* Bingkai */
    dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
    dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    /* Instruksi */
    dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_MENU, 0,
                     "Masukkan nama peserta yang dicari:");

    /* Input field label */
    dp->draw_colored(BOX_ROW + 4, BOX_COL + 2, COLOR_INFO, 0, "Nama: ");
    dp->screen_refresh();

    /* Baca nama */
    char buffer[64];
    dp->input_string(BOX_ROW + 4, BOX_COL + 8, buffer, 30);

    /* Buang newline & spasi di awal/akhir */
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
        buffer[--len] = '\0';
    /* Trim spasi depan */
    char *start = buffer;
    while (*start == ' ') start++;
    if (start != buffer) {
        memmove(buffer, start, strlen(start) + 1);
        len = strlen(buffer);
    }
    /* Trim spasi belakang */
    while (len > 0 && buffer[len - 1] == ' ') buffer[--len] = '\0';

    /* Nama kosong */
    if (len == 0) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Nama pencarian kosong!", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    /* Cari peserta */
    ParticipantNameVO name;
    strncpy(name.value, buffer, MAX_NAME_LENGTH);
    name.value[MAX_NAME_LENGTH] = '\0';

    SearchResultVO r;
    SearchError e = agent_search_find(agg, state, &name, &r);

    /* Tampilkan hasil */
    dp->cls();

    if (e == SR_OK) {
        dp->print_centered_colored(1, "HASIL PENCARIAN", COLOR_TITLE, 1);
        dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
        dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

        /* Status ditemukan */
        dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_SUCCESS, 1,
                         "[DITEMUKAN] Peserta ditemukan!");

        dp->separator(BOX_ROW + 3, BOX_COL, BOX_WIDTH);

        /* Detail peserta */
        snprintf(buf, sizeof buf, "Nama        : %s", r.name);
        dp->draw_colored(BOX_ROW + 4, BOX_COL + 4, COLOR_GOLD, 1, buf);

        snprintf(buf, sizeof buf, "Total Skor  : %d poin", r.total_score);
        dp->draw_colored(BOX_ROW + 5, BOX_COL + 4, COLOR_SUCCESS, 1, buf);

        dp->separator(BOX_ROW + 6, BOX_COL, BOX_WIDTH);

        /* Riwayat tendangan */
        dp->draw_colored(BOX_ROW + 7, BOX_COL + 2, COLOR_MENU, 0, "Riwayat Tendangan:");

        int k;
        for (k = 0; k < TOTAL_KICKS; k++) {
            int cx = BOX_COL + 4 + k * 5;
            int cy = BOX_ROW + 8;

            if (r.kicks[k] == -1) {
                dp->draw_colored(cy, cx, COLOR_DIM, 0, " - ");
            } else {
                int zone = r.kicks[k];
                int color = COLOR_DIM;
                if (zone >= 4) color = COLOR_SUCCESS;
                else if (zone >= 2) color = COLOR_WARNING;
                else if (zone == 0) color = COLOR_ERROR;
                snprintf(buf, sizeof buf, "Z%d ", zone);
                dp->draw_colored(cy, cx, color, 1, buf);
            }
        }

        dp->separator(BOX_ROW + 10, BOX_COL, BOX_WIDTH);

        /* Frekuensi zona */
        dp->draw_colored(BOX_ROW + 11, BOX_COL + 2, COLOR_MENU, 0, "Frekuensi Zona:");

        int z;
        for (z = 0; z <= MAX_ZONE; z++) {
            int cx = BOX_COL + 4 + z * 7;
            int cy = BOX_ROW + 12;
            snprintf(buf, sizeof buf, "Z%d:%d", z, r.zone_freq[z]);
            dp->draw_colored(cy, cx, COLOR_MENU, 0, buf);
        }
    } else {
        dp->print_centered_colored(1, "HASIL PENCARIAN", COLOR_TITLE, 1);
        dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
        dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

        dp->draw_colored(BOX_ROW + 4, BOX_COL + 4, COLOR_ERROR, 1, "[TIDAK DITEMUKAN]");

        snprintf(buf, sizeof buf, "Peserta '%s' tidak ditemukan.", buffer);
        dp->draw_colored(BOX_ROW + 6, BOX_COL + 4, COLOR_MENU, 0, buf);

        /* Fuzzy search: hitung similarity threshold ≥ 50% karakter */
        int suggestions = 0;
        int si;
        for (si = 0; si < state->participant_count && suggestions < 3; si++) {
            const char *pname = state->participants[si].name.value;
            /* Hitung jumlah karakter query yang ada di nama */
            int matched = 0;
            int qlen = (int)len;
            size_t pi;
            for (pi = 0; pname[pi] != '\0'; pi++) {
                char lower_p = (char)tolower((unsigned char)pname[pi]);
                size_t bi;
                for (bi = 0; buffer[bi] != '\0'; bi++) {
                    if (tolower((unsigned char)buffer[bi]) == lower_p) {
                        matched++;
                        break;
                    }
                }
            }
            /* Threshold: minimal 50% karakter query cocok di nama */
            if (qlen > 0 && matched * 2 >= qlen) {
                snprintf(buf, sizeof buf, "- %s", pname);
                dp->draw_colored(BOX_ROW + 9 + suggestions, BOX_COL + 6,
                                 COLOR_SUCCESS, 0, buf);
                suggestions++;
            }
        }
        if (suggestions == 0) {
            dp->draw_colored(BOX_ROW + 9, BOX_COL + 6, COLOR_DIM, 0,
                             "(tidak ada saran serupa)");
        }
    }

    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}
