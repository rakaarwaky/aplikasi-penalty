#include "recap/module.recap.h"
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* --- Not ready cases --- */
static void test_recap_not_ready_init(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    SearchResultVO details[1];
    assert(capabilities_recap_prepare_details(&state, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_not_ready_init\n");
}

static void test_recap_not_ready_registered(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    SearchResultVO details[1];
    assert(capabilities_recap_prepare_details(&state, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_not_ready_registered\n");
}

static void test_recap_null_state(void) {
    SearchResultVO details[1];
    assert(capabilities_recap_prepare_details(NULL, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_null_state\n");
}

static void test_recap_null_details(void) {
    CompetitionState state = {0};
    state.state = STATE_COMPLETED;
    assert(capabilities_recap_prepare_details(&state, NULL, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_null_details\n");
}

static void test_recap_capacity_too_small(void) {
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
    SearchResultVO details[1];
    assert(capabilities_recap_prepare_details(&state, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_capacity_too_small\n");
}

/* --- Ready cases --- */
static void test_recap_ready(void) {
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
            ZoneVO z = {p + 1};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }

    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = capabilities_recap_prepare_details(&state, details, MAX_PARTICIPANTS);
    assert(e == RC_OK);
    assert(details[0].found == 1);
    printf("  [PASS] test_recap_ready\n");
}

static void test_recap_details_correct(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < MIN_PARTICIPANTS; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "Player%d", i);
        capabilities_registration_append(&state, &n);
    }
    int p, k;
    for (p = 0; p < state.participant_count; p++) {
        for (k = 0; k < TOTAL_KICKS; k++) {
            ZoneVO z = {p};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }

    SearchResultVO details[MAX_PARTICIPANTS];
    capabilities_recap_prepare_details(&state, details, MAX_PARTICIPANTS);

    for (i = 0; i < state.participant_count; i++) {
        assert(details[i].found == 1);
        assert(details[i].total_score == i * TOTAL_KICKS);
        assert(details[i].zone_freq[i] == TOTAL_KICKS);
        char expected_name[20];
        snprintf(expected_name, 20, "Player%d", i);
        assert(strcmp(details[i].name, expected_name) == 0);
    }
    printf("  [PASS] test_recap_details_correct\n");
}

static void test_recap_single_participant(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Solo");
    capabilities_registration_append(&state, &n);

    int k;
    for (k = 0; k < TOTAL_KICKS; k++) {
        ZoneVO z = {4};
        capabilities_scoring_record_kick(&state, 0, z);
    }

    SearchResultVO details[1];
    RecapError e = capabilities_recap_prepare_details(&state, details, 1);
    assert(e == RC_OK);
    assert(details[0].total_score == 28);
    assert(strcmp(details[0].name, "Solo") == 0);
    printf("  [PASS] test_recap_single_participant\n");
}

/* --- Agent layer --- */
static void test_recap_agent(void) {
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
            ZoneVO z = {2};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }

    RecapAggregate agg = root_recap_build(root_ranking_build().protocol);
    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agent_recap_prepare(&agg, &state, ranking, details, MAX_PARTICIPANTS);
    assert(e == RC_OK);
    /* Verify ranking is sorted */
    for (i = 1; i < state.participant_count; i++) {
        assert(ranking[i - 1].total_score >= ranking[i].total_score);
    }
    printf("  [PASS] test_recap_agent\n");
}

static void test_recap_agent_null_aggregate(void) {
    CompetitionState state = {0};
    state.state = STATE_COMPLETED;
    RankingEntryVO ranking[1];
    SearchResultVO details[1];
    assert(agent_recap_prepare(NULL, &state, ranking, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_agent_null_aggregate\n");
}

static void test_recap_agent_null_state(void) {
    RecapAggregate agg = root_recap_build(root_ranking_build().protocol);
    RankingEntryVO ranking[1];
    SearchResultVO details[1];
    assert(agent_recap_prepare(&agg, NULL, ranking, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_agent_null_state\n");
}

static void test_recap_agent_null_ranking(void) {
    CompetitionState state = {0};
    state.state = STATE_COMPLETED;
    RecapAggregate agg = root_recap_build(root_ranking_build().protocol);
    SearchResultVO details[1];
    assert(agent_recap_prepare(&agg, &state, NULL, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_agent_null_ranking\n");
}

static void test_recap_agent_null_details(void) {
    CompetitionState state = {0};
    state.state = STATE_COMPLETED;
    RecapAggregate agg = root_recap_build(root_ranking_build().protocol);
    RankingEntryVO ranking[1];
    assert(agent_recap_prepare(&agg, &state, ranking, NULL, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_agent_null_details\n");
}

void run_recap_tests(void) {
    printf("=== Recap Tests ===\n");
    test_recap_not_ready_init();
    test_recap_not_ready_registered();
    test_recap_null_state();
    test_recap_null_details();
    test_recap_capacity_too_small();
    test_recap_ready();
    test_recap_details_correct();
    test_recap_single_participant();
    test_recap_agent();
    test_recap_agent_null_aggregate();
    test_recap_agent_null_state();
    test_recap_agent_null_ranking();
    test_recap_agent_null_details();
    printf("  All recap tests passed!\n\n");
}
