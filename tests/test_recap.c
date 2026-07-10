#include "recap/module.recap.h"
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void test_recap_not_ready(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    RankingEntryVO ranking[1];
    SearchResultVO details[1];
    assert(capabilities_recap_prepare(&state, ranking, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_not_ready\n");
}

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

    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = capabilities_recap_prepare(&state, ranking, details, MAX_PARTICIPANTS);
    assert(e == RC_OK);
    /* Highest score should be first */
    assert(ranking[0].total_score >= ranking[1].total_score);
    printf("  [PASS] test_recap_ready\n");
}

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

    RecapAggregate agg = root_recap_build();
    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agent_recap_prepare(&agg, &state, ranking, details, MAX_PARTICIPANTS);
    assert(e == RC_OK);
    printf("  [PASS] test_recap_agent\n");
}

void run_recap_tests(void) {
    printf("=== Recap Tests ===\n");
    test_recap_not_ready();
    test_recap_ready();
    test_recap_agent();
    printf("  All recap tests passed!\n\n");
}
