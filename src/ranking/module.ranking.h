/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Domain ranking: contract, capabilities, agent, root.
   Tanpa I/O: presentasi ada di cli/surfaces_ranking_command.c. */
#ifndef MODULE_RANKING_H
#define MODULE_RANKING_H

#include "shared/module.shared.h"

/* ============================================================
   CONTRACT — PROTOCOL
   ============================================================ */
typedef RankingError (*compute_ranking_fn)(const CompetitionState *state,
                                           RankingEntryVO *out, int capacity);
typedef struct {
    compute_ranking_fn compute_ranking;
} RankingProtocol;

/* ============================================================
   CONTRACT — AGGREGATE
   ============================================================ */
typedef struct {
    RankingProtocol *protocol;
} RankingAggregate;

/* ============================================================
   CAPABILITIES — pure logic
   ============================================================ */
RankingError capabilities_ranking_compute(const CompetitionState *state,
                                          RankingEntryVO *out, int capacity);

/* ============================================================
   AGENT — koordinasi; tanpa I/O
   ============================================================ */
RankingError agent_ranking_compute(RankingAggregate *agg,
                                   const CompetitionState *state,
                                   RankingEntryVO *out, int capacity);

/* ============================================================
   ROOT — container builder (wiring only)
   ============================================================ */
RankingAggregate root_ranking_build(void);

#endif /* MODULE_RANKING_H */
