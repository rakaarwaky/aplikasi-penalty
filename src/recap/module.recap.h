/* Deklarasi modul rekapitulasi — capabilities, agent, root.
   Recap meminjam RankingProtocol dari shared untuk urutan ranking. */
#ifndef MODULE_RECAP_H
#define MODULE_RECAP_H

#include "shared/contract_recap_aggregate.h"

/* CAPABILITIES — implementasi RecapProtocol: susun detail per peserta (murni) */
RecapError capabilities_recap_prepare_details(const CompetitionState *state,
                                              SearchResultVO *details, int capacity);

/* AGENT — koordinasi: pinjam ranking (kontrak) + detail recap; tanpa I/O */
RecapError agent_recap_prepare(RecapAggregate *agg,
                               const CompetitionState *state,
                               RankingEntryVO *ranking,
                               SearchResultVO *details,
                               int capacity);

/* ROOT — container builder (wiring only) — menerima RankingProtocol yang sudah di-build */
RecapAggregate root_recap_build(RankingProtocol *ranking_protocol);

#endif /* MODULE_RECAP_H */
