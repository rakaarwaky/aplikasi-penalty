#include "cli/module.cli.h"

void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state) {
    agent_registration_run(agg, state);
}
