#ifndef SHARED_CONTRACT_RECAP_AGGREGATE_H
#define SHARED_CONTRACT_RECAP_AGGREGATE_H

#include "shared/contract_recap_protocol.h"
#include "shared/contract_ranking_protocol.h"

typedef struct {
    RecapProtocol *protocol;    /* milik recap */
    RankingProtocol *ranking;   /* dipinjam dari fitur ranking via kontrak */
} RecapAggregate;

#endif /* SHARED_CONTRACT_RECAP_AGGREGATE_H */
