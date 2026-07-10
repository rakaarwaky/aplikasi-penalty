/**
 * @file root_cli_main_entry.c
 * @brief Entry point aplikasi: bootstrap state, rakit aggregate, jalankan menu, cleanup.
 */

#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <signal.h>
#include <stdlib.h>

/* Handler SIGINT/SIGTERM: bebaskan ncurses lalu keluar bersih. */
static void cleanup_handler(int sig) {
    (void)sig;
    tui_end();
    exit(0);
}

int main(void) {
    /* Tangani sinyal agar terminal tidak tinggal di mode ncurses. */
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);

    /* State tunggal aplikasi — dialokasikan di stack (tanpa global). */
    CompetitionState state;
    state.participant_count = 0;
    state.state = STATE_INIT;

    /* Rakit seluruh aggregate lewat masing-masing root container.
       Recap menerima RankingProtocol (pinjaman antar-fitur). */
    RegistrationAggregate reg = root_registration_build();
    ScoringAggregate sc = root_scoring_build();
    RankingAggregate rk = root_ranking_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build(rk.protocol);

    /* Hidupkan mode ncurses, lalu masuk ke menu utama (TUI loop). */
    tui_init();
    cli_surfaces_menu_run(&reg, &sc, &rk, &sr, &rc, &state);
    tui_end();

    /* Pesan penutup di terminal biasa (mode ncurses sudah mati). */
    printf("\nTekan Enter untuk keluar...");
    fflush(stdout);
    getchar();

    return 0;
}
