/* AES_BYPASS: module declaration (barrel) — 1 header per feature folder.
   Domain registration: contract (protocol, aggregate), capabilities, agent, root.
   Tanpa I/O: presentasi & input user ada di cli/surfaces_registration_command.c. */
#ifndef MODULE_REGISTRATION_H
#define MODULE_REGISTRATION_H

#include "shared/module.shared.h"

/* ============================================================
   CONTRACT — PROTOCOL (inbound, business ops; AES402 pakai VO)
   ============================================================ */
typedef RegistrationError (*validate_name_fn)(const CompetitionState *state,
                                              const ParticipantNameVO *name);
typedef RegistrationError (*append_participant_fn)(CompetitionState *state,
                                                   const ParticipantNameVO *name);
typedef struct {
    validate_name_fn validate_name;
    append_participant_fn append_participant;
} RegistrationProtocol;

/* ============================================================
   CONTRACT — AGGREGATE (facade; tanpa port karena tanpa I/O sistem)
   ============================================================ */
typedef struct {
    RegistrationProtocol *protocol;
} RegistrationAggregate;

/* ============================================================
   CAPABILITIES — pure logic (AES403, tanpa I/O)
   ============================================================ */
RegistrationError capabilities_registration_validate_name(const CompetitionState *state,
                                                          const ParticipantNameVO *name);
RegistrationError capabilities_registration_append(CompetitionState *state,
                                                   const ParticipantNameVO *name);

/* ============================================================
   AGENT — koordinasi (validate lalu append); tanpa I/O
   ============================================================ */
RegistrationError agent_registration_add(RegistrationAggregate *agg,
                                         CompetitionState *state,
                                         const ParticipantNameVO *name);

/* ============================================================
   ROOT — container builder (wiring only)
   ============================================================ */
RegistrationAggregate root_registration_build(void);

#endif /* MODULE_REGISTRATION_H */
