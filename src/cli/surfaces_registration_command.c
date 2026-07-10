/**
 * @file surfaces_registration_command.c
 * @brief Surface: layar pendaftaran peserta (I/O ncurses + delegasi ke agent).
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ──────────────────────────────────────────────
 * Pangkas spasi di awal & akhir string (in-place).
 * Mengembalikan panjang string setelah dipangkas.
 * ────────────────────────────────────────────── */
static size_t trim_spaces(char *str) {
    if (str == NULL) return 0;
    char *start = str;
    while (*start == ' ') start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    size_t len = strlen(str);
    while (len > 0 && str[len - 1] == ' ') str[--len] = '\0';
    return len;
}

/* ──────────────────────────────────────────────
 * Petakan RegistrationError -> pesan ramah pengguna,
 * lalu tampilkan dengan warna ERROR. Dipakai utk
 * semua kegagalan pendaftaran.
 * ────────────────────────────────────────────── */
static void show_error(RegistrationError e) {
    const char *msg;
    switch (e) {
        case REG_NAME_EMPTY:        msg = "Nama tidak boleh kosong."; break;
        case REG_NAME_TOO_LONG:     msg = "Nama melebihi 30 karakter."; break;
        case REG_NAME_INVALID_CHAR: msg = "Nama hanya huruf dan spasi."; break;
        case REG_NAME_DUPLICATE:    msg = "Nama sudah terdaftar."; break;
        case REG_FULL:              msg = "Kuota penuh."; break;
        case REG_TOO_FEW:           msg = "Minimal 5 peserta."; break;
        default:                    msg = "Kesalahan pendaftaran.";
    }
    attron(COLOR_PAIR(COLOR_ERROR));
    mvprintw(20, 4, "[GAGAL] %s          ", msg);
    attroff(COLOR_PAIR(COLOR_ERROR));
}

/**
 * Layar pendaftaran: loop baca nama dari pengguna, delegasikan
 * ke agent_registration_add(), tampilkan hasil/error. Kosongkan nama
 * & Enter = selesai (bila sudah >= MIN_PARTICIPANTS).
 */
void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;

    tui_clear();

    /* Judul + bingkai + info kuota & daftar terdaftar. */
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    tui_print_centered(1, "PENDAFTARAN PESERTA");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    tui_box(3, 2, 56, 16);

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(4, 4, "Kuota: 5 - 7 peserta.");
    mvprintw(5, 4, "Kosongkan nama & tekan Enter untuk selesai.");
    mvprintw(6, 4, "Peserta terdaftar: %d", state->participant_count);
    attroff(COLOR_PAIR(COLOR_MENU));

    /* Tampilkan daftar peserta yang sudah terdaftar. */
    int i;
    for (i = 0; i < state->participant_count; i++) {
        attron(COLOR_PAIR(COLOR_SUCCESS));
        mvprintw(8 + i, 4, "%d. %s", i + 1, state->participants[i].name.value);
        attroff(COLOR_PAIR(COLOR_SUCCESS));
    }

    refresh();

    /* Baris input berikutnya (di bawah daftar). */
    int row = 8 + state->participant_count + 1;
    char buffer[64];
    while (state->participant_count < MAX_PARTICIPANTS) {
        attron(COLOR_PAIR(COLOR_MENU));
        mvprintw(row, 4, "Nama peserta #%d: ", state->participant_count + 1);
        attroff(COLOR_PAIR(COLOR_MENU));
        refresh();

        /* Aktifkan echo & cursor untuk input teks. */
        echo();
        curs_set(1);
        memset(buffer, 0, sizeof buffer);
        mvgetnstr(row, 22, buffer, 30);
        curs_set(0);
        noecho();

        /* Buang newline/CR sisa getnstr. */
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';

        trim_spaces(buffer);
        len = strlen(buffer);

        /* Input kosong = selesai (bila quota minimal tercapai). */
        if (len == 0) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            show_error(REG_TOO_FEW);
            refresh();
            continue;
        }

        /* Salin ke VO lalu delegasikan ke agent (validasi+append). */
        ParticipantNameVO name;
        strncpy(name.value, buffer, MAX_NAME_LENGTH);
        name.value[MAX_NAME_LENGTH] = '\0';

        RegistrationError e = agent_registration_add(agg, state, &name);
        if (e == REG_OK) {
            attron(COLOR_PAIR(COLOR_SUCCESS));
            mvprintw(row, 4, "[OK] %s terdaftar.              ", name.value);
            attroff(COLOR_PAIR(COLOR_SUCCESS));
            row++;
        } else {
            show_error(e);
        }
        refresh();
    }

    /* Penutup layar pendaftaran. */
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(22, 4, "Total peserta: %d. Tekan Enter untuk kembali.", state->participant_count);
    attroff(COLOR_PAIR(COLOR_MENU));
    refresh();
    tui_getch();
}
