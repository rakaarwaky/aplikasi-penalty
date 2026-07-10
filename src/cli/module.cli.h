/* Deklarasi modul CLI — menyatukan seluruh surface command.
   Surface HANYA delegasi ke agent (AES406): tanpa logika/I/O langsung. */
#ifndef MODULE_CLI_H
#define MODULE_CLI_H

/* Deklarasi modul tiap fitur: capabilities, agent, root.
   Termasuk shared contract (aggregate) sehingga tipe aggregate & fungsi
   agent terlihat oleh seluruh surface. */
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"

/* Surface menu utama — loop TUI */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          CompetitionState *state);

/* Surface per fitur */
void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state);
void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state);
void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state);
void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state);
void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state);

#endif /* MODULE_CLI_H */
