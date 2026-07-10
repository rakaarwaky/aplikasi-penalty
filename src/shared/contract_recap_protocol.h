/**
 * @file contract_recap_protocol.h
 * @brief Daftar fungsi yang harus ada untuk rekapitulasi.
 */

#ifndef SHARED_CONTRACT_RECAP_PROTOCOL_H
#define SHARED_CONTRACT_RECAP_PROTOCOL_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_search_result_vo.h"
#include "shared/taxonomy_competition_error.h"

/* Tipe fungsi: kumpulkan data tiap peserta untuk ditampilkan. */
typedef RecapError (*prepare_details_fn)(const CompetitionState *state,
                                         SearchResultVO *details, int capacity);

/** Kumpulan fungsi recap — diisi oleh root container. */
typedef struct {
    prepare_details_fn prepare_details;  /**< Susun detail peserta. */
} RecapProtocol;

#endif /* SHARED_CONTRACT_RECAP_PROTOCOL_H */
