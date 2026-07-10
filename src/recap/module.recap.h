/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Menyatukan deklarasi publik recap: contract, capabilities, infrastructure,
   agent, root. */
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
   CONTRACT — PORT
   ============================================================ */
typedef struct {
    void (*display_header)(void);
    void (*display_row)(const CompetitionState *state,
                        const RankingEntryVO *e,
                        const SearchResultVO *d);
    void (*display_not_ready)(void);
    void (*display_wait)(void);
} RecapPort;

/* ============================================================
   CONTRACT — AGGREGATE
   ============================================================ */
typedef struct {
    RecapProtocol *protocol;
    RecapPort *port;
} RecapAggregate;

/* ============================================================
   CAPABILITIES
   ============================================================ */
RecapError capabilities_recap_prepare(const CompetitionState *state,
                                      RankingEntryVO *ranking,
                                      SearchResultVO *details,
                                      int capacity);

/* ============================================================
   INFRASTRUCTURE — port factory
   ============================================================ */
RecapPort *create_recap_port(void);

/* ============================================================
   AGENT — orchestrator
   ============================================================ */
RecapError agent_recap_run(RecapAggregate *agg, CompetitionState *state);

/* ============================================================
   ROOT — container builder (wiring only)
   ============================================================ */
RecapAggregate root_recap_build(void);

#endif /* MODULE_RECAP_H */
