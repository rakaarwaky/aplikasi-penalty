#include "ranking/module.ranking.h"
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static CompetitionState make_completed_state(int scores[], int n) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < n; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 20, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    int p, k;
    for (p = 0; p < n; p++) {
        for (k = 0; k < TOTAL_KICKS; k++) {
            ZoneVO z = {scores[p]};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }
    return state;
}

/* --- Basic ranking --- */
static void test_basic_ranking(void) {
    int scores[] = {3, 5, 1};
    CompetitionState state = make_completed_state(scores, 3);
    RankingEntryVO entries[3];
    RankingError e = capabilities_ranking_compute(&state, entries, 3);
    assert(e == RK_OK);
    assert(entries[0].total_score == 35);
    assert(entries[0].rank == 1);
    assert(entries[1].total_score == 21);
    assert(entries[2].total_score == 7);
    printf("  [PASS] test_basic_ranking\n");
}

static void test_single_participant(void) {
    int scores[] = {3};
    CompetitionState state = make_completed_state(scores, 1);
    RankingEntryVO entries[1];
    assert(capabilities_ranking_compute(&state, entries, 1) == RK_OK);
    assert(entries[0].rank == 1);
    assert(entries[0].total_score == 21);
    printf("  [PASS] test_single_participant\n");
}

static void test_all_same_score(void) {
    int scores[] = {2, 2, 2};
    CompetitionState state = make_completed_state(scores, 3);
    RankingEntryVO entries[3];
    capabilities_ranking_compute(&state, entries, 3);
    assert(entries[0].rank == 1);
    assert(entries[1].rank == 1);
    assert(entries[2].rank == 1);
    printf("  [PASS] test_all_same_score\n");
}

/* --- Tie-breaker --- */
static void test_tie_breaker_zone5(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < 2; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    int zones0[] = {5, 5, 3, 0, 0, 0, 0};
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {zones0[i]};
        capabilities_scoring_record_kick(&state, 0, z);
    }
    int zones1[] = {5, 4, 4, 0, 0, 0, 0};
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {zones1[i]};
        capabilities_scoring_record_kick(&state, 1, z);
    }

    RankingEntryVO entries[2];
    capabilities_ranking_compute(&state, entries, 2);
    assert(entries[0].participant_id == 0);
    assert(entries[0].rank == 1);
    assert(entries[1].rank == 2);
    printf("  [PASS] test_tie_breaker_zone5\n");
}

static void test_tie_breaker_zone4(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < 2; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    /* Same zone-5 count, different zone-4 */
    int zones0[] = {5, 4, 4, 0, 0, 0, 0};
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {zones0[i]};
        capabilities_scoring_record_kick(&state, 0, z);
    }
    int zones1[] = {5, 4, 3, 1, 0, 0, 0};
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {zones1[i]};
        capabilities_scoring_record_kick(&state, 1, z);
    }

    RankingEntryVO entries[2];
    capabilities_ranking_compute(&state, entries, 2);
    assert(entries[0].participant_id == 0);
    assert(entries[0].rank == 1);
    assert(entries[1].rank == 2);
    printf("  [PASS] test_tie_breaker_zone4\n");
}

static void test_equal_ranking(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < 2; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {3};
        capabilities_scoring_record_kick(&state, 0, z);
        capabilities_scoring_record_kick(&state, 1, z);
    }

    RankingEntryVO entries[2];
    capabilities_ranking_compute(&state, entries, 2);
    assert(entries[0].rank == 1);
    assert(entries[1].rank == 1);
    printf("  [PASS] test_equal_ranking\n");
}

static void test_three_way_tie(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < 3; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {2};
        capabilities_scoring_record_kick(&state, 0, z);
        capabilities_scoring_record_kick(&state, 1, z);
        capabilities_scoring_record_kick(&state, 2, z);
    }

    RankingEntryVO entries[3];
    capabilities_ranking_compute(&state, entries, 3);
    assert(entries[0].rank == 1);
    assert(entries[1].rank == 1);
    assert(entries[2].rank == 1);
    printf("  [PASS] test_three_way_tie\n");
}

/* --- Error cases --- */
static void test_not_ready(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    RankingEntryVO entries[1];
    assert(capabilities_ranking_compute(&state, entries, 1) == RK_NOT_READY);
    printf("  [PASS] test_not_ready\n");
}

static void test_registered_not_ready(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    RankingEntryVO entries[1];
    assert(capabilities_ranking_compute(&state, entries, 1) == RK_NOT_READY);
    printf("  [PASS] test_registered_not_ready\n");
}

static void test_null_state(void) {
    RankingEntryVO entries[1];
    assert(capabilities_ranking_compute(NULL, entries, 1) == RK_NO_PARTICIPANT);
    printf("  [PASS] test_null_state\n");
}

static void test_null_entries(void) {
    CompetitionState state = {0};
    state.state = STATE_COMPLETED;
    assert(capabilities_ranking_compute(&state, NULL, 1) == RK_NO_PARTICIPANT);
    printf("  [PASS] test_null_entries\n");
}

static void test_capacity_too_small(void) {
    int scores[] = {3, 5};
    CompetitionState state = make_completed_state(scores, 2);
    RankingEntryVO entries[1];
    assert(capabilities_ranking_compute(&state, entries, 1) == RK_NO_PARTICIPANT);
    printf("  [PASS] test_capacity_too_small\n");
}

/* --- Agent layer --- */
static void test_agent_ranking(void) {
    int scores[] = {3, 5};
    CompetitionState state = make_completed_state(scores, 2);
    RankingAggregate agg = root_ranking_build();
    RankingEntryVO entries[2];
    RankingError e = agent_ranking_compute(&agg, &state, entries, 2);
    assert(e == RK_OK);
    assert(entries[0].total_score == 35);
    assert(entries[1].total_score == 21);
    printf("  [PASS] test_agent_ranking\n");
}

static void test_agent_null_aggregate(void) {
    CompetitionState state = {0};
    state.state = STATE_COMPLETED;
    RankingEntryVO entries[1];
    assert(agent_ranking_compute(NULL, &state, entries, 1) == RK_NO_PARTICIPANT);
    printf("  [PASS] test_agent_null_aggregate\n");
}

static void test_agent_null_state(void) {
    RankingAggregate agg = root_ranking_build();
    RankingEntryVO entries[1];
    assert(agent_ranking_compute(&agg, NULL, entries, 1) == RK_NO_PARTICIPANT);
    printf("  [PASS] test_agent_null_state\n");
}

static void test_agent_null_entries(void) {
    int scores[] = {3};
    CompetitionState state = make_completed_state(scores, 1);
    RankingAggregate agg = root_ranking_build();
    assert(agent_ranking_compute(&agg, &state, NULL, 1) == RK_NO_PARTICIPANT);
    printf("  [PASS] test_agent_null_entries\n");
}

void run_ranking_tests(void) {
    printf("=== Ranking Tests ===\n");
    test_basic_ranking();
    test_single_participant();
    test_all_same_score();
    test_tie_breaker_zone5();
    test_tie_breaker_zone4();
    test_equal_ranking();
    test_three_way_tie();
    test_not_ready();
    test_registered_not_ready();
    test_null_state();
    test_null_entries();
    test_capacity_too_small();
    test_agent_ranking();
    test_agent_null_aggregate();
    test_agent_null_state();
    test_agent_null_entries();
    printf("  All ranking tests passed!\n\n");
}
