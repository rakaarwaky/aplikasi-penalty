#include "cli/module.cli.h"

void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state) {
    agent_recap_run(agg, state);
}
