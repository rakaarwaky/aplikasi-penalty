/**
 * @file module.ranking.h
 * @brief Kumpulan fungsi fitur peringkat (cari lewat satu file ini).
 */

#ifndef MODULE_RANKING_H
#define MODULE_RANKING_H

#include "shared/contract_ranking_aggregate.h"

/* Hitung peringkat peserta (diisi alamatnya saat rakit). */
RankingError capabilities_ranking_compute(const CompetitionState *state,
                                          RankingEntryVO *out, int capacity);

/* Hitung peringkat. */
RankingError agent_ranking_compute(RankingAggregate *agg,
                                   const CompetitionState *state,
                                   RankingEntryVO *out, int capacity);

/* Siapkan struct ranking. */
RankingAggregate root_ranking_build(void);

#endif /* MODULE_RANKING_H */
