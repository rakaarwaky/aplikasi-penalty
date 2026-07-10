/**
 * @file module.scoring.h
 * @brief Kumpulan fungsi fitur input tendangan & skor (cari lewat satu file ini).
 */

#ifndef MODULE_SCORING_H
#define MODULE_SCORING_H

#include "shared/contract_scoring_aggregate.h"

/* Cek zona & catat tendangan (diisi alamatnya saat rakit). */
ScoringError capabilities_scoring_validate_zone(ZoneVO zone);
ScoringError capabilities_scoring_record_kick(CompetitionState *state, int id, ZoneVO zone);

/* Urutkan pencatatan: validasi lalu catat. */
ScoringError agent_scoring_record(ScoringAggregate *agg,
                                  CompetitionState *state,
                                  int id, ZoneVO zone);

/* Siapkan struct scoring. */
ScoringAggregate root_scoring_build(void);

#endif /* MODULE_SCORING_H */
