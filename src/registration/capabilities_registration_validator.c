#include "registration/module.registration.h"

#include <string.h>
#include "shared/taxonomy_string_utility.h"

RegistrationError capabilities_registration_validate_name(const CompetitionState *state,
                                                          const ParticipantNameVO *name) {
    if (name == NULL || name->value[0] == '\0') return REG_NAME_EMPTY;

    size_t len = strlen(name->value);
    if (len > (size_t)MAX_NAME_LENGTH) return REG_NAME_TOO_LONG;

    int has_letter = 0;
    for (size_t i = 0; i < len; i++) {
        char c = name->value[i];
        if (isalpha((unsigned char)c)) has_letter = 1;
        else if (c != ' ') return REG_NAME_INVALID_CHAR;
    }
    if (!has_letter) return REG_NAME_EMPTY;

    if (state != NULL) {
        for (int i = 0; i < state->participant_count; i++) {
            if (ci_equal(state->participants[i].name.value, name->value))
                return REG_NAME_DUPLICATE;
        }
    }
    return REG_OK;
}
