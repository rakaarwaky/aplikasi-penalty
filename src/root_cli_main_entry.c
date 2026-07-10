#include <stdio.h>

#include "cli/module.cli.h"

int main(void) {
    CompetitionState state;
    state.participant_count = 0;
    state.state = STATE_INIT;

    RegistrationAggregate reg = root_registration_build();
    ScoringAggregate sc = root_scoring_build();
    RankingAggregate rk = root_ranking_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build();

    int running = 1;
    while (running) {
        cli_surfaces_menu_header();
        cli_surfaces_menu_display(state.state);

        int choice;
        int r = scanf("%d", &choice);
        if (r == EOF) break;
        if (r != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            continue;
        }
        { int c; while ((c = getchar()) != '\n' && c != EOF) { } }

        if (choice == MENU_EXIT) { running = 0; continue; }
        cli_surfaces_menu_dispatch(choice, &reg, &sc, &rk, &sr, &rc, &state);
    }
    printf("Terima kasih.\n");
    return 0;
}
