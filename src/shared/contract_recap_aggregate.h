/**
 * @file contract_recap_aggregate.h
 * @brief Wadah recap: gabungkan fungsi detail & peringkat.
 *
 * Struct ini memuat dua penghubung: fungsi detail milik recap
 * sendiri, dan fungsi peringkat milik fitur ranking (dipinjam
 * agar recap tak perlu menghitung peringkat sendiri).
 */

#ifndef SHARED_CONTRACT_RECAP_AGGREGATE_H
#define SHARED_CONTRACT_RECAP_AGGREGATE_H

#include "shared/contract_recap_protocol.h"
#include "shared/contract_ranking_protocol.h"

/** Penghubung recap: detail peserta + peringkat (dipinjam). */
typedef struct {
    RecapProtocol *protocol;    /**< Fungsi detail recap. */
    RankingProtocol *ranking;   /**< Fungsi peringkat dari fitur ranking. */
} RecapAggregate;

#endif /* SHARED_CONTRACT_RECAP_AGGREGATE_H */
