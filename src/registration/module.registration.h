/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Kontrak (protocol/aggregate) ada di shared/module.shared.h.
   File ini hanya deklarasi implementasi: capabilities, agent, root. */
#ifndef MODULE_REGISTRATION_H
#define MODULE_REGISTRATION_H

#include "shared/module.shared.h"

/* CAPABILITIES — implementasi RegistrationProtocol (pure, tanpa I/O) */
RegistrationError capabilities_registration_validate_name(const CompetitionState *state,
                                                          const ParticipantNameVO *name);
RegistrationError capabilities_registration_append(CompetitionState *state,
                                                   const ParticipantNameVO *name);

/* AGENT — koordinasi (validate lalu append); tanpa I/O */
RegistrationError agent_registration_add(RegistrationAggregate *agg,
                                         CompetitionState *state,
                                         const ParticipantNameVO *name);

/* ROOT — container builder (wiring only) */
RegistrationAggregate root_registration_build(void);

#endif /* MODULE_REGISTRATION_H */
