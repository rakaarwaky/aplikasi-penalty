/**
 * @file module.cli.h
 * @brief Kumpulan fungsi layar menu & tiap fitur (cari lewat satu file ini).
 */

#ifndef MODULE_CLI_H
#define MODULE_CLI_H

/* Fungsi tiap fitur (supaya layar tahu prototipenya). */
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"
#include "storage/module.storage.h"

/* Loop menu utama. */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          StorageAggregate *st,
                          CompetitionState *state);

/* Layar tiap fitur. */
void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state);
void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state);
void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state);
void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state);
void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state);

#endif /* MODULE_CLI_H */
