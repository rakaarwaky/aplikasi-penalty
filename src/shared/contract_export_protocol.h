/* EXPORT — Contract (Protocol) */
#ifndef SHARED_CONTRACT_EXPORT_PROTOCOL_H
#define SHARED_CONTRACT_EXPORT_PROTOCOL_H

#include "shared/taxonomy_export_error.h"
#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_rank_vo.h"
#include <stddef.h>

typedef struct {
    ExportError (*export_ranking)(const char *filename,
                                 const CompetitionState *state,
                                 const RankingEntryVO *entries,
                                 int count);
} ExportProtocol;

#endif /* SHARED_CONTRACT_EXPORT_PROTOCOL_H */
