#include "scoring/module.scoring.h"
#include "registration/module.registration.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* --- Zone validation --- */
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

static void test_zone_boundary_zero(void) {
    ZoneVO z = {0};
    assert(capabilities_scoring_validate_zone(z) == SC_OK);
    printf("  [PASS] test_zone_boundary_zero\n");
}

static void test_zone_boundary_five(void) {
    ZoneVO z = {5};
    assert(capabilities_scoring_validate_zone(z) == SC_OK);
    printf("  [PASS] test_zone_boundary_five\n");
}

static void test_zone_all_valid(void) {
    int valid_zones[] = {0, 1, 2, 3, 4, 5};
    int i;
    for (i = 0; i < 6; i++) {
        ZoneVO z = {valid_zones[i]};
        assert(capabilities_scoring_validate_zone(z) == SC_OK);
    }
    printf("  [PASS] test_zone_all_valid\n");
}

/* --- Record kick --- */
static void test_record_kick(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    ZoneVO z = {3};
    ScoringError e = capabilities_scoring_record_kick(&state, 0, z);
    assert(e == SC_OK);
    assert(state.participants[0].kick_count.value == 1);
    assert(state.participants[0].total_score.value == 3);
    assert(state.participants[0].kicks[0].zone == 3);
    assert(state.participants[0].zone_freq.freq[3] == 1);
    printf("  [PASS] test_record_kick\n");
}

static void test_record_kick_zero_zone(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    ZoneVO z = {0};
    ScoringError e = capabilities_scoring_record_kick(&state, 0, z);
    assert(e == SC_OK);
    assert(state.participants[0].total_score.value == 0);
    assert(state.participants[0].zone_freq.freq[0] == 1);
    printf("  [PASS] test_record_kick_zero_zone\n");
}

static void test_record_kick_five_zone(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    ZoneVO z = {5};
    ScoringError e = capabilities_scoring_record_kick(&state, 0, z);
    assert(e == SC_OK);
    assert(state.participants[0].total_score.value == 5);
    assert(state.participants[0].zone_freq.freq[5] == 1);
    printf("  [PASS] test_record_kick_five_zone\n");
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
    assert(state.participants[0].kick_count.value == TOTAL_KICKS);
    assert(state.participants[0].total_score.value == expected_total);
    assert(state.participants[0].zone_freq.freq[5] == 2);
    assert(state.participants[0].zone_freq.freq[3] == 1);
    assert(state.participants[0].zone_freq.freq[0] == 1);
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

static void test_participant_not_found(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    ZoneVO z = {3};
    assert(capabilities_scoring_record_kick(&state, 99, z) == SC_PARTICIPANT_NOT_FOUND);
    printf("  [PASS] test_participant_not_found\n");
}

static void test_participant_negative_id(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ZoneVO z = {3};
    assert(capabilities_scoring_record_kick(&state, -1, z) == SC_PARTICIPANT_NOT_FOUND);
    printf("  [PASS] test_participant_negative_id\n");
}

/* --- State transitions --- */
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

static void test_state_not_completed_partial(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < MIN_PARTICIPANTS; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "P%d", i);
        capabilities_registration_append(&state, &n);
    }
    /* Only first participant finishes */
    int k;
    for (k = 0; k < TOTAL_KICKS; k++) {
        ZoneVO z = {1};
        capabilities_scoring_record_kick(&state, 0, z);
    }
    assert(state.state == STATE_REGISTERED);
    printf("  [PASS] test_state_not_completed_partial\n");
}

static void test_zone_freq_tracking(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    int zones[] = {0, 0, 0, 5, 5, 5, 3};
    int i;
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {zones[i]};
        capabilities_scoring_record_kick(&state, 0, z);
    }
    assert(state.participants[0].zone_freq.freq[0] == 3);
    assert(state.participants[0].zone_freq.freq[5] == 3);
    assert(state.participants[0].zone_freq.freq[3] == 1);
    assert(state.participants[0].zone_freq.freq[1] == 0);
    assert(state.participants[0].zone_freq.freq[2] == 0);
    assert(state.participants[0].zone_freq.freq[4] == 0);
    printf("  [PASS] test_zone_freq_tracking\n");
}

/* --- Agent layer --- */
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
    assert(state.participants[0].total_score.value == 4);
    printf("  [PASS] test_agent_scoring_record\n");
}

static void test_agent_null_aggregate(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ZoneVO z = {3};
    assert(agent_scoring_record(NULL, &state, 0, z) == SC_NOT_REGISTERED);
    printf("  [PASS] test_agent_null_aggregate\n");
}

static void test_agent_null_state(void) {
    ScoringAggregate agg = root_scoring_build();
    ZoneVO z = {3};
    assert(agent_scoring_record(&agg, NULL, 0, z) == SC_NOT_REGISTERED);
    printf("  [PASS] test_agent_null_state\n");
}

static void test_agent_invalid_zone(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "P1");
    capabilities_registration_append(&state, &n);

    ScoringAggregate agg = root_scoring_build();
    ZoneVO z = {10};
    assert(agent_scoring_record(&agg, &state, 0, z) == SC_INVALID_ZONE);
    printf("  [PASS] test_agent_invalid_zone\n");
}

void run_scoring_tests(void) {
    printf("=== Scoring Tests ===\n");
    test_zone_valid();
    test_zone_invalid_negative();
    test_zone_invalid_too_high();
    test_zone_boundary_zero();
    test_zone_boundary_five();
    test_zone_all_valid();
    test_record_kick();
    test_record_kick_zero_zone();
    test_record_kick_five_zone();
    test_record_multiple_kicks();
    test_already_done();
    test_participant_not_found();
    test_participant_negative_id();
    test_state_completed();
    test_state_not_completed_partial();
    test_zone_freq_tracking();
    test_agent_scoring_record();
    test_agent_null_aggregate();
    test_agent_null_state();
    test_agent_invalid_zone();
    printf("  All scoring tests passed!\n\n");
}
