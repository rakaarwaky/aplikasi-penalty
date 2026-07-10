/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Menyatukan deklarasi publik search: contract, capabilities, infrastructure,
   agent, root. */
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
   CONTRACT — PORT
   ============================================================ */
typedef struct {
    void (*display_header)(void);
    void (*display_prompt)(void);
    SearchError (*read_name)(char *buffer, int size);
    void (*display_result)(const SearchResultVO *r);
    void (*display_not_found)(const char *name);
    void (*display_wait)(void);
} SearchPort;

/* ============================================================
   CONTRACT — AGGREGATE
   ============================================================ */
typedef struct {
    SearchProtocol *protocol;
    SearchPort *port;
} SearchAggregate;

/* ============================================================
   CAPABILITIES
   ============================================================ */
SearchError capabilities_search_find(const CompetitionState *state,
                                     const ParticipantNameVO *name,
                                     SearchResultVO *out);

/* ============================================================
   INFRASTRUCTURE — port factory
   ============================================================ */
SearchPort *create_search_port(void);

/* ============================================================
   AGENT — orchestrator
   ============================================================ */
SearchError agent_search_run(SearchAggregate *agg, CompetitionState *state);

/* ============================================================
   ROOT — container builder (wiring only)
   ============================================================ */
SearchAggregate root_search_build(void);

#endif /* MODULE_SEARCH_H */
