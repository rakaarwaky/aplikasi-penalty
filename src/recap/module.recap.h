/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Kontrak (protocol/aggregate) ada di shared/module.shared.h.
   Recap meminjam RankingProtocol dari shared untuk urutan ranking. */
#ifndef MODULE_RECAP_H
#define MODULE_RECAP_H

#include "shared/module.shared.h"

/* CAPABILITIES — implementasi RecapProtocol: susun detail per peserta (pure) */
RecapError capabilities_recap_prepare_details(const CompetitionState *state,
                                              SearchResultVO *details, int capacity);

/* AGENT — koordinasi: pinjam ranking (kontrak) + detail recap; tanpa I/O */
RecapError agent_recap_prepare(RecapAggregate *agg,
                               const CompetitionState *state,
                               RankingEntryVO *ranking,
                               SearchResultVO *details,
                               int capacity);

/* ROOT — container builder (wiring only) */
RecapAggregate root_recap_build(void);

#endif /* MODULE_RECAP_H */
