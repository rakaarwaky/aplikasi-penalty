/**
 * @file module.recap.h
 * @brief Kumpulan fungsi fitur rekapitulasi (cari lewat satu file ini).
 */

#ifndef MODULE_RECAP_H
#define MODULE_RECAP_H

#include "shared/contract_recap_aggregate.h"

/* Kumpulkan detail peserta (diisi alamatnya saat rakit). */
RecapError capabilities_recap_prepare_details(const CompetitionState *state,
                                              SearchResultVO *details, int capacity);

/* Susun data recap: peringkat lalu detail. */
RecapError agent_recap_prepare(RecapAggregate *agg,
                               const CompetitionState *state,
                               RankingEntryVO *ranking,
                               SearchResultVO *details,
                               int capacity);

/* Siapkan struct recap (terima peringkat dari fitur ranking). */
RecapAggregate root_recap_build(RankingProtocol *ranking_protocol);

#endif /* MODULE_RECAP_H */
