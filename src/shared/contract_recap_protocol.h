#ifndef SHARED_CONTRACT_RECAP_PROTOCOL_H
#define SHARED_CONTRACT_RECAP_PROTOCOL_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_search_result_vo.h"
#include "shared/taxonomy_competition_error.h"

/* Recap menyusun detail per peserta; urutan ranking dipinjam dari RankingProtocol. */
typedef RecapError (*prepare_details_fn)(const CompetitionState *state,
                                         SearchResultVO *details, int capacity);
typedef struct {
    prepare_details_fn prepare_details;
} RecapProtocol;

#endif /* SHARED_CONTRACT_RECAP_PROTOCOL_H */
