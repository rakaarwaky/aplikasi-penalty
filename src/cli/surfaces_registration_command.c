/**
 * @file surfaces_registration_command.c
 * @brief Layar pendaftaran: baca nama peserta & simpan ke data lomba.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_WIDTH 56
#define BOX_HEIGHT 18
#define BOX_ROW 3
#define BOX_COL 2

/* Buang spasi di awal & akhir teks (ubah di tempat). */
static size_t trim_spaces(char *str) {
    if (str == NULL) return 0;
    char *start = str;
    while (*start == ' ') start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    size_t len = strlen(str);
    while (len > 0 && str[len - 1] == ' ') str[--len] = '\0';
    return len;
}

/* Tampilkan pesan error pendaftaran yang sesuai dengan kode error. */
static void show_error(RegistrationError e, int row) {
    const char *msg;
    switch (e) {
        case REG_NAME_EMPTY:        msg = "Nama tidak boleh kosong!"; break;
        case REG_NAME_TOO_LONG:     msg = "Nama maksimal 30 karakter!"; break;
        case REG_NAME_INVALID_CHAR: msg = "Nama hanya boleh huruf dan spasi!"; break;
        case REG_NAME_DUPLICATE:    msg = "Nama sudah terdaftar!"; break;
        case REG_FULL:              msg = "Kuota peserta penuh!"; break;
        case REG_TOO_FEW:           msg = "Minimal 5 peserta untuk melanjutkan!"; break;
        default:                    msg = "Kesalahan pendaftaran!";
    }
    attron(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
    mvprintw(row, BOX_COL + 2, "  [!] %s", msg);
    attroff(COLOR_PAIR(COLOR_ERROR) | A_BOLD);
}

/**
 * Layar pendaftaran: ulang baca nama, simpan bila sah, tampilkan
 * hasil/error. Kosongkan nama & Enter = selesai (bila >= 5 peserta).
 */
void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;

    tui_clear();

    /* Judul */
    tui_print_centered_colored(1, "PENDAFTARAN PESERTA", COLOR_TITLE, 1);

    /* Bingkai */
    tui_box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);

    /* Separator header */
    tui_separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    /* Info kuota dengan progress bar */
    int pct = (state->participant_count * 100) / MAX_PARTICIPANTS;
    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(BOX_ROW + 2, BOX_COL + 2, "Kuota peserta: %d / %d", state->participant_count, MAX_PARTICIPANTS);
    attroff(COLOR_PAIR(COLOR_MENU));

    tui_progress_bar(BOX_ROW + 3, BOX_COL + 2, 30, pct, COLOR_SUCCESS);

    /* Instruksi */
    attron(COLOR_PAIR(COLOR_DIM));
    mvprintw(BOX_ROW + 5, BOX_COL + 2, "Ketik nama lalu Enter. Kosongkan untuk selesai.");
    attroff(COLOR_PAIR(COLOR_DIM));

    /* Separator */
    tui_separator(BOX_ROW + 6, BOX_COL, BOX_WIDTH);

    /* Tampilkan peserta yang sudah terdaftar */
    attron(COLOR_PAIR(COLOR_WARNING) | A_BOLD);
    mvprintw(BOX_ROW + 7, BOX_COL + 2, "Peserta terdaftar:");
    attroff(COLOR_PAIR(COLOR_WARNING) | A_BOLD);

    int i;
    for (i = 0; i < state->participant_count; i++) {
        attron(COLOR_PAIR(COLOR_SUCCESS));
        mvprintw(BOX_ROW + 8 + i, BOX_COL + 4, "%d. %s", i + 1, state->participants[i].name.value);
        attroff(COLOR_PAIR(COLOR_SUCCESS));
    }

    /* Separator sebelum input */
    tui_separator(BOX_ROW + 9 + state->participant_count, BOX_COL, BOX_WIDTH);

    refresh();

    /* Baris input berikutnya */
    int row = BOX_ROW + 10 + state->participant_count;
    char buffer[64];
    int error_row = BOX_ROW + BOX_HEIGHT - 2;

    while (state->participant_count < MAX_PARTICIPANTS) {
        attron(COLOR_PAIR(COLOR_INFO) | A_BOLD);
        mvprintw(row, BOX_COL + 2, "Nama peserta #%d: ", state->participant_count + 1);
        attroff(COLOR_PAIR(COLOR_INFO) | A_BOLD);
        refresh();

        /* Nyalakan echo & kursor untuk mengetik */
        echo();
        curs_set(1);
        memset(buffer, 0, sizeof buffer);
        mvgetnstr(row, BOX_COL + 21, buffer, 30);
        curs_set(0);
        noecho();

        /* Buang newline sisa */
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';

        trim_spaces(buffer);
        len = strlen(buffer);

        /* Kosong = selesai (bila sudah cukup peserta) */
        if (len == 0) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            show_error(REG_TOO_FEW, error_row);
            refresh();
            continue;
        }

        /* Salin ke struct nama lalu simpan */
        ParticipantNameVO name;
        strncpy(name.value, buffer, MAX_NAME_LENGTH);
        name.value[MAX_NAME_LENGTH] = '\0';

        RegistrationError e = agent_registration_add(agg, state, &name);
        if (e == REG_OK) {
            /* Bersihkan error sebelumnya */
            mvprintw(error_row, BOX_COL + 2, "                                              ");

            attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
            mvprintw(error_row, BOX_COL + 2, "  [+] %s berhasil terdaftar!", name.value);
            attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);

            /* Update progress bar */
            pct = (state->participant_count * 100) / MAX_PARTICIPANTS;
            tui_progress_bar(BOX_ROW + 3, BOX_COL + 2, 30, pct, COLOR_SUCCESS);

            row++;
        } else {
            show_error(e, error_row);
        }
        refresh();
    }

    /* Penutup */
    tui_separator(BOX_ROW + BOX_HEIGHT - 3, BOX_COL, BOX_WIDTH);
    attron(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);
    mvprintw(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, "Total peserta: %d", state->participant_count);
    attroff(COLOR_PAIR(COLOR_SUCCESS) | A_BOLD);

    tui_footer("Tekan ENTER untuk kembali ke menu utama");
    refresh();
    tui_getch();
}
