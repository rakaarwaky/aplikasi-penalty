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

/* Registration → Scoring → Ranking → Search → Recap → Storage */
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
    int p0[] = {5, 5, 5, 5, 5, 5, 5};  /* Alice: 35 */
    int p1[] = {4, 4, 4, 4, 4, 4, 4};  /* Bob: 28 */
    int p2[] = {5, 4, 3, 2, 1, 0, 5};  /* Charlie: 20 */
    int p3[] = {3, 3, 3, 3, 3, 3, 3};  /* Diana: 21 */
    int p4[] = {2, 2, 2, 2, 2, 2, 2};  /* Eve: 14 */
    int *patterns[] = {p0, p1, p2, p3, p4};

    for (int p = 0; p < 5; p++) {
        for (int k = 0; k < 7; k++) {
            ZoneVO z = {patterns[p][k]};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }

    /* 3. Calculate ranking via capabilities */
    RankingEntryVO rankings[MAX_PARTICIPANTS];
    RankingError rke = capabilities_ranking_compute(&state, rankings, MAX_PARTICIPANTS);
    assert(rke == RK_OK);

    /* Verify ranking order: Alice(35) > Bob(28) > Diana(21) > Charlie(20) > Eve(14) */
    assert(rankings[0].total_score == 35);
    assert(rankings[1].total_score == 28);
    assert(rankings[2].total_score == 21);
    assert(rankings[3].total_score == 20);
    assert(rankings[4].total_score == 14);

    /* 4. Search for participant via capabilities */
    ParticipantNameVO bob_name = {{0}};
    strcpy(bob_name.value, "Bob");
    SearchResultVO result;
    SearchError se = capabilities_search_find(&state, &bob_name, &result);
    assert(se == SR_OK);
    assert(result.found == 1);
    assert(result.total_score == 28);

    /* 5. Generate recap via capabilities */
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError re = capabilities_recap_prepare_details(&state, details, MAX_PARTICIPANTS);
    assert(re == RC_OK);
    assert(details[0].total_score == 35);  /* Alice */

    /* 6. Save to file via storage */
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
    RankingError rke = capabilities_ranking_compute(&state, rankings, MAX_PARTICIPANTS);
    assert(rke == RK_OK);

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

/* Storage round-trip with complex state */
static void test_storage_roundtrip(void) {
    CompetitionState state = {0};
    state.state = STATE_COMPLETED;

    for (int i = 0; i < MAX_PARTICIPANTS; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "Player%d", i);
        capabilities_registration_append(&state, &n);
    }

    for (int p = 0; p < state.participant_count; p++) {
        for (int k = 0; k < TOTAL_KICKS; k++) {
            ZoneVO z = {k % 6};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }

    StorageAggregate storage = root_storage_build();
    StorageError ste = agent_storage_save(&storage, "test_roundtrip.dat", &state);
    assert(ste == ST_OK);

    CompetitionState loaded = {0};
    ste = agent_storage_load(&storage, "test_roundtrip.dat", &loaded);
    assert(ste == ST_OK);
    assert(loaded.participant_count == MAX_PARTICIPANTS);
    assert(loaded.state == STATE_COMPLETED);

    for (int i = 0; i < loaded.participant_count; i++) {
        assert(loaded.participants[i].kick_count == TOTAL_KICKS);
    }

    remove("test_roundtrip.dat");
    printf("  [PASS] test_storage_roundtrip\n");
}

void run_integration_tests(void) {
    printf("=== Integration Tests ===\n");
    test_full_pipeline();
    test_all_same_scores();
    test_minimum_participants();
    test_storage_roundtrip();
    printf("All integration tests passed!\n");
}
