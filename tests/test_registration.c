#include "registration/module.registration.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* --- Basic validation --- */
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
    ParticipantNameVO name = {{0}};
    memset(name.value, 'A', MAX_NAME_LENGTH + 1);
    assert(capabilities_registration_validate_name(&state, &name) == REG_NAME_TOO_LONG);
    printf("  [PASS] test_name_too_long\n");
}

static void test_invalid_chars_digit(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Budi123");
    assert(capabilities_registration_validate_name(&state, &name) == REG_NAME_INVALID_CHAR);
    printf("  [PASS] test_invalid_chars_digit\n");
}

static void test_invalid_chars_symbol(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Budi@#$");
    assert(capabilities_registration_validate_name(&state, &name) == REG_NAME_INVALID_CHAR);
    printf("  [PASS] test_invalid_chars_symbol\n");
}

static void test_spaces_only(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "   ");
    assert(capabilities_registration_validate_name(&state, &name) == REG_NAME_EMPTY);
    printf("  [PASS] test_spaces_only\n");
}

static void test_valid_name(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Budi Santoso");
    assert(capabilities_registration_validate_name(&state, &name) == REG_OK);
    printf("  [PASS] test_valid_name\n");
}

static void test_valid_name_single_char(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "A");
    assert(capabilities_registration_validate_name(&state, &name) == REG_OK);
    printf("  [PASS] test_valid_name_single_char\n");
}

static void test_valid_name_exactly_30(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    memset(name.value, 'A', 30);
    name.value[30] = '\0';
    assert(capabilities_registration_validate_name(&state, &name) == REG_OK);
    printf("  [PASS] test_valid_name_exactly_30\n");
}

static void test_valid_name_with_spaces(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Budi Santoso Putra");
    assert(capabilities_registration_validate_name(&state, &name) == REG_OK);
    printf("  [PASS] test_valid_name_with_spaces\n");
}

/* --- Duplicate detection --- */
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

static void test_duplicate_exact_case(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO n1 = {{0}};
    strcpy(n1.value, "Raka");
    capabilities_registration_append(&state, &n1);

    ParticipantNameVO n2 = {{0}};
    strcpy(n2.value, "Raka");
    assert(capabilities_registration_validate_name(&state, &n2) == REG_NAME_DUPLICATE);
    printf("  [PASS] test_duplicate_exact_case\n");
}

/* --- Append logic --- */
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

static void test_append_initializes_fields(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "TestInit");
    capabilities_registration_append(&state, &name);

    ParticipantEntity *p = &state.participants[0];
    assert(p->id == 0);
    assert(p->total_score == 0);
    assert(p->kick_count == 0);
    int z;
    for (z = 0; z <= MAX_ZONE; z++) assert(p->zone_freq[z] == 0);
    int k;
    for (k = 0; k < TOTAL_KICKS; k++) assert(p->kicks[k] == -1);
    printf("  [PASS] test_append_initializes_fields\n");
}

static void test_append_multiple(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    int i;
    for (i = 0; i < 3; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 20, "Player%d", i);
        capabilities_registration_append(&state, &name);
    }
    assert(state.participant_count == 3);
    assert(strcmp(state.participants[2].name.value, "Player2") == 0);
    assert(state.participants[2].id == 2);
    printf("  [PASS] test_append_multiple\n");
}

/* --- State machine --- */
static void test_state_transition(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    int i;
    for (i = 0; i < 5; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    assert(state.state == STATE_REGISTERED);
    printf("  [PASS] test_state_transition\n");
}

static void test_state_stays_registered(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    int i;
    for (i = 0; i < 6; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    assert(state.state == STATE_REGISTERED);
    printf("  [PASS] test_state_stays_registered\n");
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

/* --- Agent layer --- */
static void test_agent_registration_add(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    RegistrationAggregate agg = root_registration_build();

    ParticipantNameVO name = {{0}};
    strcpy(name.value, "TestUser");
    RegistrationError e = agent_registration_add(&agg, &state, &name);
    assert(e == REG_OK);
    assert(state.participant_count == 1);

    RegistrationError e2 = agent_registration_add(&agg, &state, &name);
    assert(e2 == REG_NAME_DUPLICATE);
    printf("  [PASS] test_agent_registration_add\n");
}

static void test_agent_null_aggregate(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Test");
    assert(agent_registration_add(NULL, &state, &name) == REG_NAME_EMPTY);
    printf("  [PASS] test_agent_null_aggregate\n");
}

static void test_agent_null_state(void) {
    RegistrationAggregate agg = root_registration_build();
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Test");
    assert(agent_registration_add(&agg, NULL, &name) == REG_NAME_EMPTY);
    printf("  [PASS] test_agent_null_state\n");
}

static void test_agent_null_name(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    RegistrationAggregate agg = root_registration_build();
    assert(agent_registration_add(&agg, &state, NULL) == REG_NAME_EMPTY);
    printf("  [PASS] test_agent_null_name\n");
}

static void test_validate_null_state(void) {
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "Test");
    assert(capabilities_registration_validate_name(NULL, &name) == REG_OK);
    printf("  [PASS] test_validate_null_state\n");
}

static void test_append_null_pointers(void) {
    assert(capabilities_registration_append(NULL, NULL) == REG_NAME_EMPTY);
    printf("  [PASS] test_append_null_pointers\n");
}

void run_registration_tests(void) {
    printf("=== Registration Tests ===\n");
    test_empty_name();
    test_name_too_long();
    test_invalid_chars_digit();
    test_invalid_chars_symbol();
    test_spaces_only();
    test_valid_name();
    test_valid_name_single_char();
    test_valid_name_exactly_30();
    test_valid_name_with_spaces();
    test_duplicate_name();
    test_duplicate_exact_case();
    test_append_participant();
    test_append_initializes_fields();
    test_append_multiple();
    test_state_transition();
    test_state_stays_registered();
    test_full_capacity();
    test_agent_registration_add();
    test_agent_null_aggregate();
    test_agent_null_state();
    test_agent_null_name();
    test_validate_null_state();
    test_append_null_pointers();
    printf("  All registration tests passed!\n\n");
}
