#include "search/module.search.h"
#include "registration/module.registration.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

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

void run_search_tests(void) {
    printf("=== Search Tests ===\n");
    test_search_found();
    test_search_case_insensitive();
    test_search_not_found();
    test_search_empty_query();
    test_search_agent();
    printf("  All search tests passed!\n\n");
}
