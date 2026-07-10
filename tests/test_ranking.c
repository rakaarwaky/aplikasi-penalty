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

static void test_tie_breaker(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < 2; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "P%d", i);
        capabilities_registration_append(&state, &name);
    }

    /* P0: 5+5+3+0+0+0+0 = 13, two zone-5 */
    int zones0[] = {5, 5, 3, 0, 0, 0, 0};
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {zones0[i]};
        capabilities_scoring_record_kick(&state, 0, z);
    }

    /* P1: 5+4+4+0+0+0+0 = 13, one zone-5 */
    int zones1[] = {5, 4, 4, 0, 0, 0, 0};
    for (i = 0; i < TOTAL_KICKS; i++) {
        ZoneVO z = {zones1[i]};
        capabilities_scoring_record_kick(&state, 1, z);
    }

    RankingEntryVO entries[2];
    capabilities_ranking_compute(&state, entries, 2);
    /* P0 has more zone-5, should rank higher */
    assert(entries[0].participant_id == 0);
    assert(entries[0].rank == 1);
    assert(entries[1].rank == 2);
    printf("  [PASS] test_tie_breaker\n");
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

    /* Both: 3+3+3+3+3+3+3 = 21, same zone frequencies */
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

static void test_not_ready(void) {
    CompetitionState state = {0};
    state.state = STATE_INIT;
    RankingEntryVO entries[1];
    assert(capabilities_ranking_compute(&state, entries, 1) == RK_NOT_READY);
    printf("  [PASS] test_not_ready\n");
}

void run_ranking_tests(void) {
    printf("=== Ranking Tests ===\n");
    test_basic_ranking();
    test_tie_breaker();
    test_equal_ranking();
    test_not_ready();
    printf("  All ranking tests passed!\n\n");
}
