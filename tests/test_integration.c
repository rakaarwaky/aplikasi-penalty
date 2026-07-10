/* TESTS — Integration Tests (Full Pipeline) */
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"
#include "storage/module.storage.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Registration → Scoring → Ranking → Search → Recap */
static void test_full_pipeline(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;

    /* 1. Register 5 participants */
    const char *names[] = {"Alice", "Bob", "Charlie", "Diana", "Eve"};
    for (int i = 0; i < 5; i++) {
        ParticipantNameVO n = {{0}};
        strcpy(n.value, names[i]);
        capabilities_registration_append(&state, &n);
    }
    assert(state.participant_count == 5);

    /* 2. Record kicks with different patterns */
    int patterns[5][7] = {
        {5, 5, 5, 5, 5, 5, 5},  /* Alice: all zone 5 = 35 */
        {4, 4, 4, 4, 4, 4, 4},  /* Bob: all zone 4 = 28 */
        {5, 4, 3, 2, 1, 0, 5},  /* Charlie: mixed = 20 */
        {3, 3, 3, 3, 3, 3, 3},  /* Diana: all zone 3 = 21 */
        {2, 2, 2, 2, 2, 2, 2}   /* Eve: all zone 2 = 14 */
    };
    for (int p = 0; p < 5; p++) {
        for (int k = 0; k < 7; k++) {
            ZoneVO z = {patterns[p][k]};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }

    /* 3. Calculate ranking */
    RankingEntryVO rankings[MAX_PARTICIPANTS];
    RankingProtocol rp = ranking_capabilities_create();
    rp.calculate(&state, rankings, MAX_PARTICIPANTS);

    /* Verify ranking order: Alice(35) > Bob(28) > Diana(21) > Charlie(20) > Eve(14) */
    assert(rankings[0].total_score == 35);
    assert(rankings[1].total_score == 28);
    assert(rankings[2].total_score == 21);
    assert(rankings[3].total_score == 20);
    assert(rankings[4].total_score == 14);

    /* 4. Search for participant */
    SearchProtocol sp = search_capabilities_create();
    SearchResultVO result;
    SearchError se = sp.find(&state, "Bob", &result);
    assert(se == SR_OK);
    assert(result.found == 1);
    assert(result.total_score == 28);

    /* 5. Generate recap */
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapProtocol rp2 = recap_capabilities_create();
    RecapError re = rp2.prepare_details(&state, details, MAX_PARTICIPANTS);
    assert(re == RC_OK);
    assert(details[0].total_score == 35);  /* Alice */

    /* 6. Save to file */
    StorageAggregate storage = root_storage_build();
    StorageError ste = agent_storage_save(&storage, "test_pipeline.dat", &state);
    assert(ste == ST_OK);

    /* 7. Load and verify */
    CompetitionState loaded = {0};
    ste = agent_storage_load(&storage, "test_pipeline.dat", &loaded);
    assert(ste == ST_OK);
    assert(loaded.participant_count == 5);
    assert(strcmp(loaded.participants[0].name.value, "Alice") == 0);

    remove("test_pipeline.dat");
    printf("  [PASS] test_full_pipeline\n");
}

/* Edge: all same scores */
static void test_all_same_scores(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;

    for (int i = 0; i < MIN_PARTICIPANTS; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "P%d", i);
        capabilities_registration_append(&state, &n);
    }

    for (int p = 0; p < state.participant_count; p++) {
        for (int k = 0; k < TOTAL_KICKS; k++) {
            ZoneVO z = {3};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }

    RankingEntryVO rankings[MAX_PARTICIPANTS];
    RankingProtocol rp = ranking_capabilities_create();
    rp.calculate(&state, rankings, MAX_PARTICIPANTS);

    /* All should have same score */
    for (int i = 0; i < state.participant_count; i++) {
        assert(rankings[i].total_score == 21);
    }
    printf("  [PASS] test_all_same_scores\n");
}

/* Edge: minimum participants */
static void test_minimum_participants(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;

    for (int i = 0; i < MIN_PARTICIPANTS; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "Player%d", i);
        capabilities_registration_append(&state, &n);
    }
    assert(state.participant_count == MIN_PARTICIPANTS);
    assert(state.state == STATE_REGISTERED);

    for (int p = 0; p < state.participant_count; p++) {
        for (int k = 0; k < TOTAL_KICKS; k++) {
            ZoneVO z = {p % 6};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }

    assert(state.state == STATE_COMPLETED);
    printf("  [PASS] test_minimum_participants\n");
}

void run_integration_tests(void) {
    printf("=== Integration Tests ===\n");
    test_full_pipeline();
    test_all_same_scores();
    test_minimum_participants();
    printf("All integration tests passed!\n");
}
