#ifndef SHARED_CONTRACT_SEARCH_PROTOCOL_H
#define SHARED_CONTRACT_SEARCH_PROTOCOL_H

/* Inbound protocol untuk pencarian peserta. */

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_search_result_vo.h"
#include "shared/taxonomy_competition_error.h"

/**
 * Cari peserta berdasarkan nama.
 * @param state  Pointer ke state kompetisi (read-only).
 * @param name   Pointer ke VO nama yang dicari.
 * @param out    Pointer ke SearchResultVO untuk menampung hasil.
 * @return       SearchError — SR_OK jika ditemukan.
 */
typedef SearchError (*find_participant_fn)(const CompetitionState *state,
                                           const ParticipantNameVO *name,
                                           SearchResultVO *out);

/** Protocol search. */
typedef struct {
    find_participant_fn find_participant;  /**< Cari peserta berdasarkan nama. */
} SearchProtocol;

#endif /* SHARED_CONTRACT_SEARCH_PROTOCOL_H */
