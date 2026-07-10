#include "cli/module.cli.h"

void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state) {
    agent_search_run(agg, state);
}
