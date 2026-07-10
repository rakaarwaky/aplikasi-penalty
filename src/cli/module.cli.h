/* AES_BYPASS: module declaration (barrel) — 1 header untuk folder cli.
   Menyatukan deklarasi seluruh surface command. Surface HANYA delegasi ke agent
   (AES406): tanpa logic/I/O langsung. */
#ifndef MODULE_CLI_H
#define MODULE_CLI_H

#include "shared/contract_registration_aggregate.h"
#include "shared/contract_scoring_aggregate.h"
#include "shared/contract_ranking_aggregate.h"
#include "shared/contract_search_aggregate.h"
#include "shared/contract_recap_aggregate.h"

/* Feature surfaces */
void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state);
void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state);
void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state);
void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state);
void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state);

#endif /* MODULE_CLI_H */
