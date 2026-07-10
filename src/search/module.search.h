/* Deklarasi modul pencarian — capabilities, agent, root. */
#ifndef MODULE_SEARCH_H
#define MODULE_SEARCH_H

#include "shared/contract_search_aggregate.h"

/* CAPABILITIES — implementasi SearchProtocol (murni, tanpa I/O) */
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
