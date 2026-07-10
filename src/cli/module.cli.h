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
#include "storage/module.storage.h"
#include "export/module.export.h"

DisplayPort root_display_build(void);

/* ── Smart Surfaces (command) ── */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          StorageAggregate *st, ExportAggregate *ex,
                          CompetitionState *state,
                          DisplayPort *dp, SanitizeAggregate *sn);

void cli_surfaces_registration_execute(RegistrationAggregate *agg,
                                       CompetitionState *state, DisplayPort *dp,
                                       SanitizeAggregate *sn);
void cli_surfaces_scoring_execute(ScoringAggregate *agg,
                                  CompetitionState *state, DisplayPort *dp,
                                  SanitizeAggregate *sn);
void cli_surfaces_ranking_execute(RankingAggregate *agg,
                                  ExportAggregate *ex,
                                  CompetitionState *state, DisplayPort *dp);
void cli_surfaces_search_execute(SearchAggregate *agg,
                                 ExportAggregate *ex,
                                 CompetitionState *state, DisplayPort *dp);
void cli_surfaces_recap_execute(RecapAggregate *agg,
                                ExportAggregate *ex,
                                CompetitionState *state, DisplayPort *dp);
void cli_surfaces_storage_execute(StorageAggregate *agg,
                                  CompetitionState *state, DisplayPort *dp);
void cli_surfaces_menu_draw(DisplayPort *dp, int selected, CompetitionState *state);

/* ── Dumb Surfaces (page) ── */
void registration_page_draw(DisplayPort *dp, CompetitionState *state);
int  registration_page_input_row(int participant_count);
int  registration_page_error_row(void);

void scoring_page_draw(DisplayPort *dp, ParticipantEntity *part,
                       const char *msg, int msg_is_error);

void ranking_page_draw(DisplayPort *dp, RankingEntryVO *entries,
                       CompetitionState *state, int max_score,
                       ExportAggregate *ex);

void search_page_draw_input(DisplayPort *dp);
void search_page_draw_found(DisplayPort *dp, SearchResultVO *r,
                            ExportAggregate *ex);
void search_page_draw_not_found(DisplayPort *dp, const char *query,
                                CompetitionState *state);

void recap_page_draw(DisplayPort *dp, RankingEntryVO *ranking,
                     SearchResultVO *details, CompetitionState *state,
                     int total_score, int avg_score, int highest_score,
                     ExportAggregate *ex);

void storage_page_draw(DisplayPort *dp, int selected, int file_exists,
                       const char *msg, int msg_is_error);

#endif /* MODULE_CLI_H */
