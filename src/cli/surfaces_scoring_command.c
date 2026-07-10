#include "cli/module.cli.h"

void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state) {
    agent_scoring_run(agg, state);
}
