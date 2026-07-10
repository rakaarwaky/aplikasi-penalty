/**
 * @file module.search.h
 * @brief Kumpulan fungsi fitur cari peserta (cari lewat satu file ini).
 */

#ifndef MODULE_SEARCH_H
#define MODULE_SEARCH_H

#include "shared/contract_search_aggregate.h"

/* Cari peserta dari nama (diisi alamatnya saat rakit). */
SearchError capabilities_search_find(const CompetitionState *state,
                                     const ParticipantNameVO *name,
                                     SearchResultVO *out);

/* Cari peserta. */
SearchError agent_search_find(SearchAggregate *agg,
                              const CompetitionState *state,
                              const ParticipantNameVO *name,
                              SearchResultVO *out);

/* Siapkan struct pencarian. */
SearchAggregate root_search_build(void);

#endif /* MODULE_SEARCH_H */
