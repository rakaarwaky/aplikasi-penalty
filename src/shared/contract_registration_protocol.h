#ifndef SHARED_CONTRACT_REGISTRATION_PROTOCOL_H
#define SHARED_CONTRACT_REGISTRATION_PROTOCOL_H

#include "shared/taxonomy_competition_state_vo.h"
#include "shared/taxonomy_participant_vo.h"
#include "shared/taxonomy_competition_error.h"

/* Inbound protocol registration (diimplementasikan capabilities). AES402: pakai VO. */
typedef RegistrationError (*validate_name_fn)(const CompetitionState *state,
                                              const ParticipantNameVO *name);
typedef RegistrationError (*append_participant_fn)(CompetitionState *state,
                                                   const ParticipantNameVO *name);
typedef struct {
    validate_name_fn validate_name;
    append_participant_fn append_participant;
} RegistrationProtocol;

#endif /* SHARED_CONTRACT_REGISTRATION_PROTOCOL_H */
