/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Domain search: contract, capabilities, agent, root.
   Tanpa I/O: presentasi & input ada di cli/surfaces_search_command.c. */
#ifndef MODULE_SEARCH_H
#define MODULE_SEARCH_H

#include "shared/module.shared.h"

/* ============================================================
   CONTRACT — PROTOCOL
   ============================================================ */
typedef SearchError (*find_participant_fn)(const CompetitionState *state,
                                           const ParticipantNameVO *name,
                                           SearchResultVO *out);
typedef struct {
    find_participant_fn find_participant;
} SearchProtocol;

/* ============================================================
   CONTRACT — AGGREGATE
   ============================================================ */
typedef struct {
    SearchProtocol *protocol;
} SearchAggregate;

/* ============================================================
   CAPABILITIES — pure logic
   ============================================================ */
SearchError capabilities_search_find(const CompetitionState *state,
                                     const ParticipantNameVO *name,
                                     SearchResultVO *out);

/* ============================================================
   AGENT — koordinasi; tanpa I/O
   ============================================================ */
SearchError agent_search_find(SearchAggregate *agg,
                              const CompetitionState *state,
                              const ParticipantNameVO *name,
                              SearchResultVO *out);

/* ============================================================
   ROOT — container builder (wiring only)
   ============================================================ */
SearchAggregate root_search_build(void);

#endif /* MODULE_SEARCH_H */
