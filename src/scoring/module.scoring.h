/* AES_BYPASS: module declaration file — 1 header per feature folder.
   Berisi semua deklarasi publik untuk fitur scoring:
   contract (protocol, port, aggregate), capabilities, infrastructure, agent, root. */
#ifndef MODULE_SCORING_H
#define MODULE_SCORING_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_zone_vo.h"
#include "shared/taxonomy_errors.h"

/* ============================================================
   CONTRACT — PROTOCOL
   AES402: pakai ZoneVO, bukan int zone.
   ============================================================ */
typedef ScoringError (*validate_zone_fn)(ZoneVO zone);
typedef ScoringError (*record_kick_fn)(CompetitionState *state, int id, ZoneVO zone);
typedef struct {
    validate_zone_fn validate_zone;
    record_kick_fn   record_kick;
} ScoringProtocol;

/* ============================================================
   CONTRACT — PORT (AES406: I/O terminal hanya via port)
   ============================================================ */
typedef struct {
    void (*display_header)(void);
    void (*display_prompt)(const ParticipantEntity *p, int kick_no);
    ScoringError (*read_zone)(ZoneVO *out);
    void (*display_result)(int zone, int points);
    void (*display_error)(ScoringError e);
    void (*display_wait)(void);
    void (*clear_buffer)(void);
} ScoringPort;

/* ============================================================
   CONTRACT — AGGREGATE
   ============================================================ */
typedef struct {
    ScoringProtocol *protocol;
    ScoringPort     *port;
} ScoringAggregate;

/* ============================================================
   CAPABILITIES
   ============================================================ */
ScoringError capabilities_scoring_validate_zone(ZoneVO zone);
ScoringError capabilities_scoring_record_kick(CompetitionState *state, int id, ZoneVO zone);

/* ============================================================
   INFRASTRUCTURE — port factory
   ============================================================ */
ScoringPort *create_scoring_port(void);

/* ============================================================
   AGENT — orchestrator
   ============================================================ */
ScoringError agent_scoring_run(ScoringAggregate *agg, CompetitionState *state);

/* ============================================================
   ROOT — container builder
   ============================================================ */
ScoringAggregate root_scoring_build(void);

#endif /* MODULE_SCORING_H */
