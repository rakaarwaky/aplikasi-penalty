/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Menyatukan deklarasi publik ranking: contract, capabilities, infrastructure,
   agent, root. */
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
   CONTRACT — PORT
   ============================================================ */
typedef struct {
    void (*display_header)(void);
    void (*display_entry)(const CompetitionState *state, const RankingEntryVO *e);
    void (*display_not_ready)(void);
    void (*display_wait)(void);
} RankingPort;

/* ============================================================
   CONTRACT — AGGREGATE
   ============================================================ */
typedef struct {
    RankingProtocol *protocol;
    RankingPort *port;
} RankingAggregate;

/* ============================================================
   CAPABILITIES
   ============================================================ */
RankingError capabilities_ranking_compute(const CompetitionState *state,
                                          RankingEntryVO *out, int capacity);

/* ============================================================
   INFRASTRUCTURE — port factory
   ============================================================ */
RankingPort *create_ranking_port(void);

/* ============================================================
   AGENT — orchestrator
   ============================================================ */
RankingError agent_ranking_run(RankingAggregate *agg, CompetitionState *state);

/* ============================================================
   ROOT — container builder (wiring only)
   ============================================================ */
RankingAggregate root_ranking_build(void);

#endif /* MODULE_RANKING_H */
