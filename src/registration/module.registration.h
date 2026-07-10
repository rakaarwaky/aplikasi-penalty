/* Deklarasi modul pendaftaran — capabilities, agent, root. */
#ifndef MODULE_REGISTRATION_H
#define MODULE_REGISTRATION_H

#include "shared/contract_registration_aggregate.h"

/* CAPABILITIES — implementasi RegistrationProtocol (murni, tanpa I/O) */
RegistrationError capabilities_registration_validate_name(const CompetitionState *state,
                                                          const ParticipantNameVO *name);
RegistrationError capabilities_registration_append(CompetitionState *state,
                                                   const ParticipantNameVO *name);

/* AGENT — koordinasi (validasi lalu tambah); tanpa I/O */
RegistrationError agent_registration_add(RegistrationAggregate *agg,
                                         CompetitionState *state,
                                         const ParticipantNameVO *name);

/* ROOT — container builder (wiring only) */
RegistrationAggregate root_registration_build(void);

#endif /* MODULE_REGISTRATION_H */
