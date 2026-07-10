#include "cli/module.cli.h"
#include "infrastructure_tui_adapter.h"

int main(void) {
    CompetitionState state;
    state.participant_count = 0;
    state.state = STATE_INIT;

    RegistrationAggregate reg = root_registration_build();
    ScoringAggregate sc = root_scoring_build();
    RankingAggregate rk = root_ranking_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build();

    tui_init();
    cli_surfaces_menu_run(&reg, &sc, &rk, &sr, &rc, &state);
    tui_end();

    return 0;
}
