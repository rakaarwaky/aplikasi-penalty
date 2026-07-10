#ifndef SHARED_CONTRACT_RECAP_PROTOCOL_H
#define SHARED_CONTRACT_RECAP_PROTOCOL_H

/* Inbound protocol untuk rekapitulasi hasil lomba. */

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_search_result_vo.h"
#include "shared/taxonomy_competition_error.h"

/**
 * Susun detail seluruh peserta untuk rekapitulasi.
 * @param state    Pointer ke state kompetisi (read-only).
 * @param details  Array SearchResultVO untuk menampung hasil.
 * @param capacity Kapasitas maksimum array details.
 * @return         RecapError — RC_OK jika berhasil.
 */
typedef RecapError (*prepare_details_fn)(const CompetitionState *state,
                                         SearchResultVO *details, int capacity);

/** Protocol recap. */
typedef struct {
    prepare_details_fn prepare_details;  /**< Susun detail seluruh peserta. */
} RecapProtocol;

#endif /* SHARED_CONTRACT_RECAP_PROTOCOL_H */
