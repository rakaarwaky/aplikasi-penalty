/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Domain scoring: contract, capabilities, agent, root.
   Tanpa I/O: presentasi & input ada di cli/surfaces_scoring_command.c. */
#ifndef MODULE_SCORING_H
#define MODULE_SCORING_H

#include "shared/module.shared.h"

/* ============================================================
   CONTRACT — PROTOCOL (AES402 pakai ZoneVO)
   ============================================================ */
typedef ScoringError (*validate_zone_fn)(ZoneVO zone);
typedef ScoringError (*record_kick_fn)(CompetitionState *state, int id, ZoneVO zone);
typedef struct {
    validate_zone_fn validate_zone;
    record_kick_fn record_kick;
} ScoringProtocol;

/* ============================================================
   CONTRACT — AGGREGATE
   ============================================================ */
typedef struct {
    ScoringProtocol *protocol;
} ScoringAggregate;

/* ============================================================
   CAPABILITIES — pure logic
   ============================================================ */
ScoringError capabilities_scoring_validate_zone(ZoneVO zone);
ScoringError capabilities_scoring_record_kick(CompetitionState *state, int id, ZoneVO zone);

/* ============================================================
   AGENT — koordinasi (validate lalu record); tanpa I/O
   ============================================================ */
ScoringError agent_scoring_record(ScoringAggregate *agg,
                                  CompetitionState *state,
                                  int id, ZoneVO zone);

/* ============================================================
   ROOT — container builder (wiring only)
   ============================================================ */
ScoringAggregate root_scoring_build(void);

#endif /* MODULE_SCORING_H */
