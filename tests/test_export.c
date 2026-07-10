/* TESTS — Export Tests */
#include "module.export.h"
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_export_ranking_txt(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;

    /* Register participants */
    const char *names[] = {"Alice", "Bob", "Charlie"};
    for (int i = 0; i < 3; i++) {
        ParticipantNameVO n = {{0}};
        strcpy(n.value, names[i]);
        capabilities_registration_append(&state, &n);
    }

    /* Record kicks */
    int kicks[][7] = {
        {5, 5, 5, 5, 5, 5, 5},
        {4, 4, 4, 4, 4, 4, 4},
        {3, 3, 3, 3, 3, 3, 3}
    };
    for (int p = 0; p < 3; p++) {
        for (int k = 0; k < 7; k++) {
            ZoneVO z = {kicks[p][k]};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }

    /* Calculate ranking */
    RankingEntryVO rankings[MAX_PARTICIPANTS];
    capabilities_ranking_compute(&state, rankings, MAX_PARTICIPANTS);

    /* Export to TXT */
    ExportAggregate agg = root_export_build();
    ExportError err = agent_export_ranking(&agg, "test_export.txt", &state, rankings, 3);
    assert(err == EXP_OK);

    /* Read and verify file */
    FILE *f = fopen("test_export.txt", "r");
    assert(f != NULL);

    char line[256];
    int found_alice = 0, found_bob = 0, found_charlie = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "Alice")) found_alice = 1;
        if (strstr(line, "Bob")) found_bob = 1;
        if (strstr(line, "Charlie")) found_charlie = 1;
    }
    fclose(f);

    assert(found_alice);
    assert(found_bob);
    assert(found_charlie);

    remove("test_export.txt");
    printf("  [PASS] test_export_ranking_txt\n");
}

static void test_export_null_filename(void) {
    ExportAggregate agg = root_export_build();
    RankingEntryVO entries[1] = {{0}};
    ExportError err = agent_export_ranking(&agg, NULL, NULL, entries, 1);
    assert(err == EXP_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_export_null_filename\n");
}

static void test_export_empty_data(void) {
    ExportAggregate agg = root_export_build();
    CompetitionState state = {0};
    RankingEntryVO entries[1] = {{0}};
    ExportError err = agent_export_ranking(&agg, "test.txt", &state, entries, 0);
    assert(err == EXP_ERROR_EMPTY_DATA);
    printf("  [PASS] test_export_empty_data\n");
}

void run_export_tests(void) {
    printf("=== Export Tests ===\n");
    test_export_ranking_txt();
    test_export_null_filename();
    test_export_empty_data();
    printf("All export tests passed!\n");
}
