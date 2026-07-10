/**
 * @file surfaces_scoring_command.c
 * @brief Surface: layar input tendangan & skor (I/O ncurses + delegasi ke agent).
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ──────────────────────────────────────────────
 * Baca satu zona dari pengguna (string -> int).
 * Validasi karakter digit/- di sini (pramasuk),
 * lalu kembalikan via out. SC_INVALID_ZONE bila
 * input bukan angka sah.
 * ────────────────────────────────────────────── */
static ScoringError read_zone(ZoneVO *out) {
    char buf[32];
    echo();
    curs_set(1);
    memset(buf, 0, sizeof buf);
    getnstr(buf, 10);
    curs_set(0);
    noecho();

    /* Hanya digit atau '-' yang diizinkan sebagai awal angka. */
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

/**
 * Layar scoring: untuk tiap peserta, loop TOTAL_KICKS tendangan.
 * Setiap tendangan dibaca (read_zone), divalidasi rentang, lalu
 * didelegasikan ke agent_scoring_record(). Guard state: blokir bila
 * STATE_INIT (belum daftar) atau STATE_COMPLETED (sudah selesai).
 */
void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;

    /* Blokir bila peserta belum daftar. */
    if (state->state == STATE_INIT) {
        tui_clear();
        attron(COLOR_PAIR(COLOR_ERROR));
        tui_print_centered(10, "[GAGAL] Daftar peserta dulu (Menu 1).");
        attroff(COLOR_PAIR(COLOR_ERROR));
        refresh();
        tui_getch();
        return;
    }

    /* Blokir bila kompetisi sudah selesai. */
    if (state->state == STATE_COMPLETED) {
        tui_clear();
        attron(COLOR_PAIR(COLOR_MENU));
        tui_print_centered(10, "[INFO] Semua peserta sudah selesai melakukan tendangan.");
        attroff(COLOR_PAIR(COLOR_MENU));
        refresh();
        tui_getch();
        return;
    }

    /* Iterasi tiap peserta. */
    int p;
    for (p = 0; p < state->participant_count; p++) {
        ParticipantEntity *part = &state->participants[p];
        while (part->kick_count < TOTAL_KICKS) {
            tui_clear();

            /* Judul + bingkai + info peserta & riwayat tendangan. */
            attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
            tui_print_centered(1, "INPUT TENDANGAN DAN SKOR");
            attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

            tui_box(3, 2, 56, 12);

            attron(COLOR_PAIR(COLOR_MENU));
            mvprintw(4, 4, "Peserta: %s", part->name.value);
            mvprintw(5, 4, "Tendangan %d/%d  |  Skor sementara: %d",
                     part->kick_count + 1, TOTAL_KICKS, part->total_score);
            attroff(COLOR_PAIR(COLOR_MENU));

            /* Riwayat zona yang sudah dicatat. */
            int k;
            attron(COLOR_PAIR(COLOR_MENU));
            mvprintw(7, 4, "Riwayat: ");
            for (k = 0; k < part->kick_count; k++) {
                printw("Z%d ", part->kicks[k]);
            }
            attroff(COLOR_PAIR(COLOR_MENU));

            /* Prompt zona. */
            attron(COLOR_PAIR(COLOR_MENU));
            mvprintw(9, 4, "Masukkan zona (0-%d): ", MAX_ZONE);
            attroff(COLOR_PAIR(COLOR_MENU));
            refresh();

            /* Baca & validasi rentang zona. */
            ZoneVO z;
            if (read_zone(&z) != SC_OK || z.value < MIN_ZONE || z.value > MAX_ZONE) {
                tui_clear();
                attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
                tui_print_centered(1, "INPUT TENDANGAN DAN SKOR");
                attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
                tui_box(3, 2, 56, 12);
                attron(COLOR_PAIR(COLOR_ERROR));
                mvprintw(9, 4, "[GAGAL] Zona harus %d-%d.        ", MIN_ZONE, MAX_ZONE);
                attroff(COLOR_PAIR(COLOR_ERROR));
                refresh();
                tui_getch();
                continue;
            }

            /* Delegasikan pencatatan ke agent. */
            ScoringError e = agent_scoring_record(agg, state, p, z);
            tui_clear();
            attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
            tui_print_centered(1, "INPUT TENDANGAN DAN SKOR");
            attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
            tui_box(3, 2, 56, 12);
            if (e == SC_OK) {
                attron(COLOR_PAIR(COLOR_SUCCESS));
                mvprintw(9, 4, "Zona %d -> %d poin               ", z.value, z.value);
                attroff(COLOR_PAIR(COLOR_SUCCESS));
            } else if (e == SC_INVALID_ZONE) {
                attron(COLOR_PAIR(COLOR_ERROR));
                mvprintw(9, 4, "[GAGAL] Zona harus %d-%d.        ", MIN_ZONE, MAX_ZONE);
                attroff(COLOR_PAIR(COLOR_ERROR));
            } else {
                attron(COLOR_PAIR(COLOR_ERROR));
                mvprintw(9, 4, "[GAGAL] Kesalahan pencatatan.    ");
                attroff(COLOR_PAIR(COLOR_ERROR));
            }
            refresh();
            tui_getch();
        }

        /* Pesan selesai per peserta. */
        tui_clear();
        attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
        mvprintw(5, 4, "[SELESAI] %s — Total: %d poin",
                 part->name.value, part->total_score);
        attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
        refresh();
        tui_getch();
    }

    /* Penutup: bila semua selesai, state sudah COMPLETED. */
    tui_clear();
    if (state->state == STATE_COMPLETED) {
        attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
        tui_print_centered(5, "Semua tendangan selesai!");
        attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
    }
    attron(COLOR_PAIR(COLOR_MENU));
    tui_print_centered(7, "Tekan Enter untuk kembali...");
    attroff(COLOR_PAIR(COLOR_MENU));
    refresh();
    tui_getch();
}
