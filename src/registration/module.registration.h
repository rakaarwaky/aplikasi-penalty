/* AES_BYPASS: module declaration file — 1 header per feature folder.
   Berisi semua deklarasi publik untuk fitur registration:
   contract (protocol, port, aggregate), capabilities, infrastructure, agent, root. */
#ifndef MODULE_REGISTRATION_H
#define MODULE_REGISTRATION_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_errors.h"

/* ============================================================
   CONTRACT — PROTOCOL (inbound, business operations)
   AES402: pakai ParticipantNameVO, bukan char* mentah.
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
   CONTRACT — PORT (outbound I/O, AES406: I/O hanya via port)
   ============================================================ */
typedef struct {
    void (*display_header)(void);
    void (*display_prompt)(int number);
    RegistrationError (*read_name)(char *buffer, int size);
    void (*display_success)(const char *name);
    void (*display_error)(RegistrationError e);
    void (*display_wait)(void);
    void (*clear_buffer)(void);
} RegistrationPort;

/* ============================================================
   CONTRACT — AGGREGATE (pengikat protocol + port)
   ============================================================ */
typedef struct {
    RegistrationProtocol *protocol;
    RegistrationPort *port;
} RegistrationAggregate;

/* ============================================================
   CAPABILITIES — function declarations
   ============================================================ */
RegistrationError capabilities_registration_validate_name(const CompetitionState *state,
                                                          const ParticipantNameVO *name);
RegistrationError capabilities_registration_append(CompetitionState *state,
                                                   const ParticipantNameVO *name);

/* ============================================================
   INFRASTRUCTURE — port factory
   ============================================================ */
RegistrationPort *create_registration_port(void);

/* ============================================================
   AGENT — orchestrator
   ============================================================ */
RegistrationError agent_registration_run(RegistrationAggregate *agg, CompetitionState *state);

/* ============================================================
   ROOT — container builder (wiring only, no logic)
   ============================================================ */
RegistrationAggregate root_registration_build(void);

#endif /* MODULE_REGISTRATION_H */
