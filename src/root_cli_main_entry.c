/**
 * @file root_cli_main_entry.c
 * @brief Titik mulai program: siapkan data, jalankan menu, bersihkan layar.
 */

#include "cli/module.cli.h"
#include "sanitizer/module.sanitizer.h"
#include "tui/infrastructure_tui_adapter.h"

#include <signal.h>
#include <stdlib.h>

/* Bila program dihentikan mendadak, matikan moda ncurses dulu. */
static void cleanup_handler(int sig) {
    (void)sig;
    tui_end();
    exit(0);
}

int main(void) {
    /* Tangani tombol interupsi agar terminal kembali normal. */
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);

    /* Satu-satunya data lomba (disimpan di sini, bukan global). */
    CompetitionState state;
    state.participant_count = 0;
    state.state = STATE_INIT;

    /* Siapkan seluruh fitur (pendaftaran, scoring, ranking, dll). */
    RegistrationAggregate reg = root_registration_build();
    ScoringAggregate      sc  = root_scoring_build();
    RankingAggregate      rk  = root_ranking_build();
    SearchAggregate       sr  = root_search_build();
    RecapAggregate        rc  = root_recap_build(rk.protocol);
    SanitizeAggregate     sn  = root_sanitize_build();

    /* Hidupkan layar ncurses lalu tampilkan splash screen. */
    tui_init();

    /* Rakit DisplayPort — surfaces hanya pegang pointer ke ini. */
    DisplayPort dp = root_display_build();

    tui_splash(50);
    flushinp();  /* buang sisa Enter agar tidak bocor ke menu utama */
    cli_surfaces_menu_run(&reg, &sc, &rk, &sr, &rc, &state, &dp, &sn);

    /* D2: Ringkasan juara sebelum keluar — tampilkan di ncurses, tunggu Enter. */
    if (state.state == STATE_COMPLETED && state.participant_count > 0) {
        RankingEntryVO entries[MAX_PARTICIPANTS];
        if (agent_ranking_compute(&rk, &state, entries, MAX_PARTICIPANTS) == RK_OK) {
            dp.cls();
            dp.print_centered_colored(2, "* * * * * * * * * * * * * * * *", COLOR_GOLD, 1);
            dp.print_centered_colored(3, "TERIMA KASIH TELAH BERLOMBA", COLOR_TITLE, 1);
            dp.print_centered_colored(4, "* * * * * * * * * * * * * * * *", COLOR_GOLD, 1);

            const char *winner = state.participants[entries[0].participant_id].name.value;
            dp.print_centered_colored(7, "JUARA UMUM", COLOR_GOLD, 1);

            char line[64];
            snprintf(line, sizeof line, "%s  -  %d poin", winner, entries[0].total_score);
            dp.print_centered_colored(8, line, COLOR_MENU, 1);

            if (state.participant_count >= 2) {
                const char *second = state.participants[entries[1].participant_id].name.value;
                snprintf(line, sizeof line, "Juara 2: %s (%d poin)", second, entries[1].total_score);
                dp.print_centered_colored(10, line, COLOR_MENU, 0);
            }
            if (state.participant_count >= 3) {
                const char *third = state.participants[entries[2].participant_id].name.value;
                snprintf(line, sizeof line, "Juara 3: %s (%d poin)", third, entries[2].total_score);
                dp.print_centered_colored(11, line, COLOR_MENU, 0);
            }

            dp.footer("Tekan ENTER untuk keluar");
            dp.screen_refresh();
            dp.readkey();
        }
    }

    tui_end();

    /* Pesan penutup di terminal biasa. */
    printf("\nTekan Enter untuk keluar...");
    fflush(stdout);
    getchar();

    return 0;
}
