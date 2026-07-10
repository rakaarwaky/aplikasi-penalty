/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Kontrak (protocol/aggregate) ada di shared/module.shared.h.
   File ini hanya deklarasi implementasi: capabilities, agent, root. */
#ifndef MODULE_SEARCH_H
#define MODULE_SEARCH_H

#include "shared/module.shared.h"

/* CAPABILITIES — implementasi SearchProtocol (pure, tanpa I/O) */
SearchError capabilities_search_find(const CompetitionState *state,
                                     const ParticipantNameVO *name,
                                     SearchResultVO *out);

/* AGENT — koordinasi; tanpa I/O */
SearchError agent_search_find(SearchAggregate *agg,
                              const CompetitionState *state,
                              const ParticipantNameVO *name,
                              SearchResultVO *out);

/* ROOT — container builder (wiring only) */
SearchAggregate root_search_build(void);

#endif /* MODULE_SEARCH_H */
