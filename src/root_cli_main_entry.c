/**
 * @file root_cli_main_entry.c
 * @brief Titik mulai program: siapkan data, jalankan menu, bersihkan layar.
 */

#include "cli/module.cli.h"
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
    ScoringAggregate sc = root_scoring_build();
    RankingAggregate rk = root_ranking_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build(rk.protocol);

    /* Hidupkan layar ncurses lalu tampilkan splash screen. */
    tui_init();
    tui_splash(50);
    flushinp();  /* buang sisa Enter agar tidak bocor ke menu utama */
    cli_surfaces_menu_run(&reg, &sc, &rk, &sr, &rc, &state);
    tui_end();

    /* Pesan penutup di terminal biasa. */
    printf("\nTekan Enter untuk keluar...");
    fflush(stdout);
    getchar();

    return 0;
}
