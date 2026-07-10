#include "registration/module.registration.h"

/* Koordinasi 2 subsistem capabilities: validasi lalu penambahan. Tanpa I/O. */
RegistrationError agent_registration_add(RegistrationAggregate *agg,
                                         CompetitionState *state,
                                         const ParticipantNameVO *name) {
    if (agg == NULL || state == NULL || name == NULL) return REG_NAME_EMPTY;

    RegistrationError ve = agg->protocol->validate_name(state, name);
    if (ve != REG_OK) return ve;

    return agg->protocol->append_participant(state, name);
}
