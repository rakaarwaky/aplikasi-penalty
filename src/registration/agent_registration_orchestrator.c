#include "registration/module.registration.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int ci_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

RegistrationError agent_registration_run(RegistrationAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return REG_NAME_EMPTY;

    agg->port->display_header();
    printf("Peserta terdaftar: %d\n\n", state->participant_count);

    char buffer[MAX_NAME_LENGTH + 2];
    while (state->participant_count < MAX_PARTICIPANTS) {
        agg->port->display_prompt(state->participant_count + 1);
        RegistrationError re = agg->port->read_name(buffer, sizeof buffer);
        if (re == REG_NAME_EMPTY) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            printf("  Belum mencapai minimal %d peserta. Lanjutkan.\n", MIN_PARTICIPANTS);
            continue;
        }
        if (ci_equal(buffer, "selesai") || ci_equal(buffer, "SELESAI")) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            agg->port->display_error(REG_TOO_FEW);
            continue;
        }
        ParticipantNameVO name;
        strncpy(name.value, buffer, MAX_NAME_LENGTH);
        name.value[MAX_NAME_LENGTH] = '\0';

        RegistrationError ve = agg->protocol->validate_name(state, &name);
        if (ve != REG_OK) { agg->port->display_error(ve); continue; }

        agg->protocol->append_participant(state, &name);
        agg->port->display_success(name.value);
    }

    if (state->participant_count < MIN_PARTICIPANTS) return REG_TOO_FEW;
    printf("Total peserta: %d\n", state->participant_count);
    return REG_OK;
}
