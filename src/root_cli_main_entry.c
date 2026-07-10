#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"

#include <signal.h>
#include <stdlib.h>

static void cleanup_handler(int sig) {
    (void)sig;
    tui_end();
    exit(0);
}

int main(void) {
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);

    CompetitionState state;
    state.participant_count = 0;
    state.state = STATE_INIT;

    RegistrationAggregate reg = root_registration_build();
    ScoringAggregate sc = root_scoring_build();
    RankingAggregate rk = root_ranking_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build(rk.protocol);

    tui_init();
    cli_surfaces_menu_run(&reg, &sc, &rk, &sr, &rc, &state);
    tui_end();

    printf("\nTekan Enter untuk keluar...");
    fflush(stdout);
    getchar();

    return 0;
}
