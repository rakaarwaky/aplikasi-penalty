#include "scoring/module.scoring.h"
#include "registration/module.registration.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void test_zone_valid(void) {
    ZoneVO z = {3};
    assert(capabilities_scoring_validate_zone(z) == SC_OK);
    printf("  [PASS] test_zone_valid\n");
}

static void test_zone_invalid_negative(void) {
    ZoneVO z = {-1};
    assert(capabilities_scoring_validate_zone(z) == SC_INVALID_ZONE);
    printf("  [PASS] test_zone_invalid_negative\n");
}

static void test_zone_invalid_too_high(void) {
    ZoneVO z = {6};
    assert(capabilities_scoring_validate_zone(z) == SC_INVALID_ZONE);
    printf("  [PASS] test_zone_invalid_too_high\n");
}

static void test_zone_boundary(void) {
    ZoneVO z0 = {0};
    ZoneVO z5 = {5};
    assert(capabilities_scoring_validate_zone(z0) == SC_OK);
    assert(capabilities_scoring_validate_zone(z5) == SC_OK);
    printf("  [PASS] test_zone_boundary\n");
}

static void test_record_kick(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    ZoneVO z = {3};
    ScoringError e = capabilities_scoring_record_kick(&state, 0, z);
    assert(e == SC_OK);
    assert(state.participants[0].kick_count == 1);
    assert(state.participants[0].total_score == 3);
    assert(state.participants[0].kicks[0] == 3);
    assert(state.participants[0].zone_freq[3] == 1);
    printf("  [PASS] test_record_kick\n");
}

static void test_record_multiple_kicks(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    int zones[] = {5, 3, 0, 2, 4, 1, 5};
    int expected_total = 5 + 3 + 0 + 2 + 4 + 1 + 5;
    int i;
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {zones[i]};
        capabilities_scoring_record_kick(&state, 0, z);
    }
    assert(state.participants[0].kick_count == TOTAL_KICKS);
    assert(state.participants[0].total_score == expected_total);
    assert(state.participants[0].zone_freq[5] == 2);
    assert(state.participants[0].zone_freq[3] == 1);
    assert(state.participants[0].zone_freq[0] == 1);
    printf("  [PASS] test_record_multiple_kicks\n");
}

static void test_already_done(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    int i;
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {1};
        capabilities_scoring_record_kick(&state, 0, z);
    }
    ZoneVO extra = {2};
    assert(capabilities_scoring_record_kick(&state, 0, extra) == SC_ALREADY_DONE);
    printf("  [PASS] test_already_done\n");
}

static void test_state_completed(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < MIN_PARTICIPANTS; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "P%d", i);
        capabilities_registration_append(&state, &n);
    }
    int p, k;
    for (p = 0; p < state.participant_count; p++) {
        for (k = 0; k < TOTAL_KICKS; k++) {
            ZoneVO z = {1};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }
    assert(state.state == STATE_COMPLETED);
    printf("  [PASS] test_state_completed\n");
}

static void test_agent_scoring_record(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    ScoringAggregate agg = root_scoring_build();
    ZoneVO z = {4};
    ScoringError e = agent_scoring_record(&agg, &state, 0, z);
    assert(e == SC_OK);
    assert(state.participants[0].total_score == 4);
    printf("  [PASS] test_agent_scoring_record\n");
}

void run_scoring_tests(void) {
    printf("=== Scoring Tests ===\n");
    test_zone_valid();
    test_zone_invalid_negative();
    test_zone_invalid_too_high();
    test_zone_boundary();
    test_record_kick();
    test_record_multiple_kicks();
    test_already_done();
    test_state_completed();
    test_agent_scoring_record();
    printf("  All scoring tests passed!\n\n");
}
