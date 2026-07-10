#include "search/module.search.h"
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* --- Found cases --- */
static void test_search_found(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Budi");
    capabilities_registration_append(&state, &n);

    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Budi");
    SearchResultVO result;
    SearchError e = capabilities_search_find(&state, &query, &result);
    assert(e == SR_OK);
    assert(result.found == 1);
    assert(strcmp(result.name, "Budi") == 0);
    printf("  [PASS] test_search_found\n");
}

static void test_search_case_insensitive(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Ali");
    capabilities_registration_append(&state, &n);

    ParticipantNameVO query = {{0}};
    strcpy(query.value, "ali");
    SearchResultVO result;
    SearchError e = capabilities_search_find(&state, &query, &result);
    assert(e == SR_OK);
    assert(result.found == 1);
    printf("  [PASS] test_search_case_insensitive\n");
}

static void test_search_case_mixed(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Raka");
    capabilities_registration_append(&state, &n);

    ParticipantNameVO query = {{0}};
    strcpy(query.value, "rAKA");
    SearchResultVO result;
    SearchError e = capabilities_search_find(&state, &query, &result);
    assert(e == SR_OK);
    assert(result.found == 1);
    printf("  [PASS] test_search_case_mixed\n");
}

static void test_search_first_participant(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < 3; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "P%d", i);
        capabilities_registration_append(&state, &n);
    }

    ParticipantNameVO query = {{0}};
    strcpy(query.value, "P0");
    SearchResultVO result;
    assert(capabilities_search_find(&state, &query, &result) == SR_OK);
    assert(result.participant_id == 0);
    printf("  [PASS] test_search_first_participant\n");
}

static void test_search_last_participant(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < 3; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "P%d", i);
        capabilities_registration_append(&state, &n);
    }

    ParticipantNameVO query = {{0}};
    strcpy(query.value, "P2");
    SearchResultVO result;
    assert(capabilities_search_find(&state, &query, &result) == SR_OK);
    assert(result.participant_id == 2);
    printf("  [PASS] test_search_last_participant\n");
}

static void test_search_result_details(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Test");
    capabilities_registration_append(&state, &n);

    /* Record some kicks */
    int zones[] = {5, 3, 0, 2, 4, 1, 5};
    int i;
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {zones[i]};
        capabilities_scoring_record_kick(&state, 0, z);
    }

    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Test");
    SearchResultVO result;
    capabilities_search_find(&state, &query, &result);
    assert(result.total_score == 20);
    for (i = 0; i < TOTAL_KICKS; i++) {
        assert(result.kicks[i] == zones[i]);
    }
    assert(result.zone_freq[5] == 2);
    assert(result.zone_freq[3] == 1);
    printf("  [PASS] test_search_result_details\n");
}

/* --- Not found cases --- */
static void test_search_not_found(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Budi");
    capabilities_registration_append(&state, &n);

    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Raka");
    SearchResultVO result;
    SearchError e = capabilities_search_find(&state, &query, &result);
    assert(e == SR_NOT_FOUND);
    assert(result.found == 0);
    printf("  [PASS] test_search_not_found\n");
}

static void test_search_empty_query(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO query = {{0}};
    SearchResultVO result;
    assert(capabilities_search_find(&state, &query, &result) == SR_EMPTY_QUERY);
    printf("  [PASS] test_search_empty_query\n");
}

static void test_search_partial_name(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Budi Santoso");
    capabilities_registration_append(&state, &n);

    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Budi");
    SearchResultVO result;
    /* Partial match should NOT find (exact match required) */
    assert(capabilities_search_find(&state, &query, &result) == SR_NOT_FOUND);
    printf("  [PASS] test_search_partial_name\n");
}

static void test_search_similar_name(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Budi");
    capabilities_registration_append(&state, &n);

    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Budiman");
    SearchResultVO result;
    assert(capabilities_search_find(&state, &query, &result) == SR_NOT_FOUND);
    printf("  [PASS] test_search_similar_name\n");
}

/* --- Null/edge cases --- */
static void test_search_null_state(void) {
    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Test");
    SearchResultVO result;
    assert(capabilities_search_find(NULL, &query, &result) == SR_NOT_FOUND);
    printf("  [PASS] test_search_null_state\n");
}

static void test_search_null_name(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    SearchResultVO result;
    assert(capabilities_search_find(&state, NULL, &result) == SR_NOT_FOUND);
    printf("  [PASS] test_search_null_name\n");
}

static void test_search_null_result(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Test");
    assert(capabilities_search_find(&state, &query, NULL) == SR_NOT_FOUND);
    printf("  [PASS] test_search_null_result\n");
}

/* --- Agent layer --- */
static void test_search_agent(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Dina");
    capabilities_registration_append(&state, &n);

    SearchAggregate agg = root_search_build();
    ParticipantNameVO query = {{0}};
    strcpy(query.value, "dina");
    SearchResultVO result;
    SearchError e = agent_search_find(&agg, &state, &query, &result);
    assert(e == SR_OK);
    assert(result.found == 1);
    printf("  [PASS] test_search_agent\n");
}

static void test_search_agent_not_found(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO n = {{0}};
    strcpy(n.value, "Dina");
    capabilities_registration_append(&state, &n);

    SearchAggregate agg = root_search_build();
    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Raka");
    SearchResultVO result;
    SearchError e = agent_search_find(&agg, &state, &query, &result);
    assert(e == SR_NOT_FOUND);
    printf("  [PASS] test_search_agent_not_found\n");
}

static void test_search_agent_null_aggregate(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Test");
    SearchResultVO result;
    assert(agent_search_find(NULL, &state, &query, &result) == SR_NOT_FOUND);
    printf("  [PASS] test_search_agent_null_aggregate\n");
}

static void test_search_agent_null_state(void) {
    SearchAggregate agg = root_search_build();
    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Test");
    SearchResultVO result;
    assert(agent_search_find(&agg, NULL, &query, &result) == SR_NOT_FOUND);
    printf("  [PASS] test_search_agent_null_state\n");
}

static void test_search_agent_null_query(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    SearchAggregate agg = root_search_build();
    SearchResultVO result;
    assert(agent_search_find(&agg, &state, NULL, &result) == SR_NOT_FOUND);
    printf("  [PASS] test_search_agent_null_query\n");
}

static void test_search_agent_null_result(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    ParticipantNameVO query = {{0}};
    strcpy(query.value, "Test");
    SearchAggregate agg = root_search_build();
    assert(agent_search_find(&agg, &state, &query, NULL) == SR_NOT_FOUND);
    printf("  [PASS] test_search_agent_null_result\n");
}

void run_search_tests(void) {
    printf("=== Search Tests ===\n");
    test_search_found();
    test_search_case_insensitive();
    test_search_case_mixed();
    test_search_first_participant();
    test_search_last_participant();
    test_search_result_details();
    test_search_not_found();
    test_search_empty_query();
    test_search_partial_name();
    test_search_similar_name();
    test_search_null_state();
    test_search_null_name();
    test_search_null_result();
    test_search_agent();
    test_search_agent_not_found();
    test_search_agent_null_aggregate();
    test_search_agent_null_state();
    test_search_agent_null_query();
    test_search_agent_null_result();
    printf("  All search tests passed!\n\n");
}
