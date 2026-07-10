/* Deklarasi modul scoring — capabilities, agent, root. */
#ifndef MODULE_SCORING_H
#define MODULE_SCORING_H

#include "shared/contract_scoring_aggregate.h"

/* CAPABILITIES — implementasi ScoringProtocol (murni, tanpa I/O) */
ScoringError capabilities_scoring_validate_zone(ZoneVO zone);
ScoringError capabilities_scoring_record_kick(CompetitionState *state, int id, ZoneVO zone);

/* AGENT — koordinasi (validasi lalu catat); tanpa I/O */
ScoringError agent_scoring_record(ScoringAggregate *agg,
                                  CompetitionState *state,
                                  int id, ZoneVO zone);

/* ROOT — container builder (wiring only) */
ScoringAggregate root_scoring_build(void);

#endif /* MODULE_SCORING_H */
