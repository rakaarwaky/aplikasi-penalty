/* AES_BYPASS: module declaration (barrel) — 1 header untuk folder cli.
   Menyatukan deklarasi seluruh surface command. Surface HANYA delegasi ke agent
   (AES406): tanpa logic/I/O langsung. */
#ifndef MODULE_CLI_H
#define MODULE_CLI_H

#include "shared/module.shared.h"
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"

/* Menu surfaces — header, display, dispatch */
void cli_surfaces_menu_header(void);
void cli_surfaces_menu_display(CompetitionStateKind state);
void cli_surfaces_menu_dispatch(int choice, RegistrationAggregate *reg,
                                ScoringAggregate *sc, RankingAggregate *rk,
                                SearchAggregate *sr, RecapAggregate *rc,
                                CompetitionState *state);

/* Feature surfaces */
void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state);
void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state);
void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state);
void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state);
void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state);

#endif /* MODULE_CLI_H */
