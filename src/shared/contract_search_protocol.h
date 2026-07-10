/**
 * @file contract_search_protocol.h
 * @brief Daftar fungsi yang harus ada untuk cari peserta.
 */

#ifndef SHARED_CONTRACT_SEARCH_PROTOCOL_H
#define SHARED_CONTRACT_SEARCH_PROTOCOL_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_search_result_vo.h"
#include "shared/taxonomy_competition_error.h"

/* Tipe fungsi: cari peserta dari namanya. */
typedef SearchError (*find_participant_fn)(const CompetitionState *state,
                                           const ParticipantNameVO *name,
                                           SearchResultVO *out);

/** Kumpulan fungsi pencarian — diisi oleh root container. */
typedef struct {
    find_participant_fn find_participant;  /**< Cari peserta. */
} SearchProtocol;

#endif /* SHARED_CONTRACT_SEARCH_PROTOCOL_H */
