/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Domain recap: contract, capabilities, agent, root.
   Tanpa I/O: presentasi ada di cli/surfaces_recap_command.c. */
#ifndef MODULE_RECAP_H
#define MODULE_RECAP_H

#include "shared/module.shared.h"

/* ============================================================
   CONTRACT — PROTOCOL (recap menyusun ranking + detail tendangan)
   ============================================================ */
typedef RecapError (*prepare_recap_fn)(const CompetitionState *state,
                                       RankingEntryVO *ranking,
                                       SearchResultVO *details,
                                       int capacity);
typedef struct {
    prepare_recap_fn prepare_recap;
} RecapProtocol;

/* ============================================================
   CONTRACT — AGGREGATE
   ============================================================ */
typedef struct {
    RecapProtocol *protocol;
} RecapAggregate;

/* ============================================================
   CAPABILITIES — pure logic
   ============================================================ */
RecapError capabilities_recap_prepare(const CompetitionState *state,
                                      RankingEntryVO *ranking,
                                      SearchResultVO *details,
                                      int capacity);

/* ============================================================
   AGENT — koordinasi; tanpa I/O
   ============================================================ */
RecapError agent_recap_prepare(RecapAggregate *agg,
                               const CompetitionState *state,
                               RankingEntryVO *ranking,
                               SearchResultVO *details,
                               int capacity);

/* ============================================================
   ROOT — container builder (wiring only)
   ============================================================ */
RecapAggregate root_recap_build(void);

#endif /* MODULE_RECAP_H */
