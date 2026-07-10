/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Kontrak (protocol/aggregate) ada di shared/module.shared.h.
   File ini hanya deklarasi implementasi: capabilities, agent, root. */
#ifndef MODULE_RANKING_H
#define MODULE_RANKING_H

#include "shared/module.shared.h"

/* CAPABILITIES — implementasi RankingProtocol (pure, tanpa I/O) */
RankingError capabilities_ranking_compute(const CompetitionState *state,
                                          RankingEntryVO *out, int capacity);

/* AGENT — koordinasi; tanpa I/O */
RankingError agent_ranking_compute(RankingAggregate *agg,
                                   const CompetitionState *state,
                                   RankingEntryVO *out, int capacity);

/* ROOT — container builder (wiring only) */
RankingAggregate root_ranking_build(void);

#endif /* MODULE_RANKING_H */
