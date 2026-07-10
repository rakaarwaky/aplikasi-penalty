#include "cli/module.cli.h"

void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state) {
    agent_ranking_run(agg, state);
}
