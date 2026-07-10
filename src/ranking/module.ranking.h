/* Deklarasi modul ranking — capabilities, agent, root. */
#ifndef MODULE_RANKING_H
#define MODULE_RANKING_H

#include "shared/contract_ranking_aggregate.h"

/* CAPABILITIES — implementasi RankingProtocol (murni, tanpa I/O) */
RankingError capabilities_ranking_compute(const CompetitionState *state,
                                          RankingEntryVO *out, int capacity);

/* AGENT — koordinasi; tanpa I/O */
RankingError agent_ranking_compute(RankingAggregate *agg,
                                   const CompetitionState *state,
                                   RankingEntryVO *out, int capacity);

/* ROOT — container builder (wiring only) */
RankingAggregate root_ranking_build(void);

#endif /* MODULE_RANKING_H */
