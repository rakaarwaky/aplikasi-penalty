#include "registration/module.registration.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void test_empty_name(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    assert(capabilities_registration_validate_name(&state, &name) == REG_NAME_EMPTY);
    printf("  [PASS] test_empty_name\n");
}

static void test_name_too_long(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name;
    memset(name.value, 'A', 31);
    name.value[31] = '\0';
    assert(capabilities_registration_validate_name(&state, &name) == REG_NAME_TOO_LONG);
    printf("  [PASS] test_name_too_long\n");
}

static void test_invalid_chars(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Budi123");
    assert(capabilities_registration_validate_name(&state, &name) == REG_NAME_INVALID_CHAR);
    printf("  [PASS] test_invalid_chars\n");
}

static void test_valid_name(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Budi Santoso");
    assert(capabilities_registration_validate_name(&state, &name) == REG_OK);
    printf("  [PASS] test_valid_name\n");
}

static void test_duplicate_name(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO n1 = {{0}};
    strcpy(n1.value, "Ali");
    capabilities_registration_append(&state, &n1);

    ParticipantNameVO n2 = {{0}};
    strcpy(n2.value, "ali");
    assert(capabilities_registration_validate_name(&state, &n2) == REG_NAME_DUPLICATE);
    printf("  [PASS] test_duplicate_name\n");
}

static void test_append_participant(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Raka");

    RegistrationError e = capabilities_registration_append(&state, &name);
    assert(e == REG_OK);
    assert(state.participant_count == 1);
    assert(strcmp(state.participants[0].name.value, "Raka") == 0);
    assert(state.participants[0].total_score == 0);
    assert(state.participants[0].kick_count == 0);
    printf("  [PASS] test_append_participant\n");
}

static void test_state_transition(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    char names[5][20] = {"A", "B", "C", "D", "E"};
    int i;
    for (i = 0; i < 5; i++) {
        ParticipantNameVO name = {{0}};
        strcpy(name.value, names[i]);
        capabilities_registration_append(&state, &name);
    }
    assert(state.state == STATE_REGISTERED);
    printf("  [PASS] test_state_transition\n");
}

static void test_full_capacity(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    int i;
    for (i = 0; i < MAX_PARTICIPANTS; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 20, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    ParticipantNameVO extra = {{0}};
    strcpy(extra.value, "Extra");
    assert(capabilities_registration_append(&state, &extra) == REG_FULL);
    printf("  [PASS] test_full_capacity\n");
}

static void test_agent_registration_add(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    RegistrationAggregate agg = root_registration_build();

    ParticipantNameVO name = {{0}};
    strcpy(name.value, "TestUser");
    RegistrationError e = agent_registration_add(&agg, &state, &name);
    assert(e == REG_OK);
    assert(state.participant_count == 1);

    /* Duplicate should fail */
    RegistrationError e2 = agent_registration_add(&agg, &state, &name);
    assert(e2 == REG_NAME_DUPLICATE);
    printf("  [PASS] test_agent_registration_add\n");
}

void run_registration_tests(void) {
    printf("=== Registration Tests ===\n");
    test_empty_name();
    test_name_too_long();
    test_invalid_chars();
    test_valid_name();
    test_duplicate_name();
    test_append_participant();
    test_state_transition();
    test_full_capacity();
    test_agent_registration_add();
    printf("  All registration tests passed!\n\n");
}
