/**
 * @file module.cli.h
 * @brief Kumpulan fungsi layar menu & tiap fitur (cari lewat satu file ini).
 */

#ifndef MODULE_CLI_H
#define MODULE_CLI_H

#include "shared/contract_display_port.h"
#include "shared/contract_sanitize_aggregate.h"
#include "shared/taxonomy_display_constant.h"

#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"

DisplayPort root_display_build(void);

/* ── Smart Surfaces (command) ── */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          CompetitionState *state,
                          DisplayPort *dp,
                          SanitizeAggregate *sn);

void cli_surfaces_registration_execute(RegistrationAggregate *agg,
                                       CompetitionState *state, DisplayPort *dp,
                                       SanitizeAggregate *sn);
void cli_surfaces_scoring_execute(ScoringAggregate *agg,
                                  CompetitionState *state, DisplayPort *dp,
                                  SanitizeAggregate *sn);
void cli_surfaces_ranking_execute(RankingAggregate *agg,
                                  CompetitionState *state, DisplayPort *dp);
void cli_surfaces_search_execute(SearchAggregate *agg,
                                 CompetitionState *state, DisplayPort *dp);
void cli_surfaces_recap_execute(RecapAggregate *agg,
                                CompetitionState *state, DisplayPort *dp);

/* ── Dumb Surfaces (page) ── */
void menu_page_draw(DisplayPort *dp, int selected, CompetitionStateKind state,
                    int participant_count);
void menu_page_draw_help(DisplayPort *dp);

void registration_page_draw(DisplayPort *dp, CompetitionState *state);
int  registration_page_input_row(int participant_count);
int  registration_page_error_row(int participant_count);

void scoring_page_draw(DisplayPort *dp, ParticipantEntity *part,
                       const char *msg, int msg_is_error);

void ranking_page_draw(DisplayPort *dp, RankingEntryVO *entries,
                       CompetitionState *state, int max_score);

void search_page_draw_input(DisplayPort *dp);
void search_page_draw_found(DisplayPort *dp, SearchResultVO *r);
void search_page_draw_not_found(DisplayPort *dp, const char *query,
                                CompetitionState *state);

void recap_page_draw(DisplayPort *dp, RankingEntryVO *ranking,
                     SearchResultVO *details, CompetitionState *state,
                     int total_score, int avg_score, int highest_score);

#endif /* MODULE_CLI_H */
