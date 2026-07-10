/* TESTS — Coverage Gaps (tutup branch error/null-guard ke 100%) */
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"
#include "storage/module.storage.h"
#include "export/module.export.h"
#include "sanitizer/module.sanitizer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Storage orchestrator: agg/protocol NULL -> ST_ERROR_FILE_NOT_FOUND */
static void test_storage_orchestrator_null(void) {
    CompetitionState state = {0};
    assert(agent_storage_save(NULL, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);
    StorageAggregate agg = { .protocol = NULL };
    assert(agent_storage_save(&agg, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_load(NULL, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_load(&agg, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_storage_orchestrator_null\n");
}

/* Storage adapter: filename/state NULL -> error (lewat orchestrator valid) */
static void test_storage_adapter_null_params(void) {
    CompetitionState state = {0};
    state.state = STATE_COMPLETED;
    StorageAggregate agg = root_storage_build();
    assert(agent_storage_save(&agg, NULL, &state) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_save(&agg, "x.dat", NULL) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_load(&agg, NULL, &state) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_load(&agg, "x.dat", NULL) == ST_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_storage_adapter_null_params\n");
}

/* Export orchestrator: agg/protocol NULL -> EXP_ERROR_FILE_NOT_FOUND */
static void test_export_orchestrator_null(void) {
    CompetitionState state = {0};
    RankingEntryVO entries[1] = {{0}};
    assert(agent_export_ranking(NULL, "x.txt", &state, entries, 1)
           == EXP_ERROR_FILE_NOT_FOUND);
    ExportAggregate agg = { .protocol = NULL };
    assert(agent_export_ranking(&agg, "x.txt", &state, entries, 1)
           == EXP_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_export_orchestrator_null\n");
}

/* Export adapter: filename/state/entries NULL & count<=0 -> error */
static void test_export_adapter_null_and_empty(void) {
    CompetitionState state = {0};
    RankingEntryVO entries[1] = {{0}};
    ExportAggregate agg = root_export_build();
    assert(agent_export_ranking(&agg, NULL, &state, entries, 1)
           == EXP_ERROR_FILE_NOT_FOUND);
    assert(agent_export_ranking(&agg, "x.txt", NULL, entries, 1)
           == EXP_ERROR_FILE_NOT_FOUND);
    assert(agent_export_ranking(&agg, "x.txt", &state, NULL, 1)
           == EXP_ERROR_FILE_NOT_FOUND);
    assert(agent_export_ranking(&agg, "x.txt", &state, entries, 0)
           == EXP_ERROR_EMPTY_DATA);
    printf("  [PASS] test_export_adapter_null_and_empty\n");
}

/* Sanitizer: nilai negatif (val<min) & string kosong (endptr==input) */
static void test_sanitizer_int_edge(void) {
    assert(sanitizer_validate_int("-5", 0, 10) == SANITIZE_ERROR_TOO_LONG);
    assert(sanitizer_validate_int("", 0, 10) == SANITIZE_ERROR_INVALID_CHARS);
    printf("  [PASS] test_sanitizer_int_edge\n");
}

/* Recap orchestrator: compute_ranking gagal (state belum COMPLETED) -> RC_NOT_READY */
static void test_recap_orchestrator_compute_fails(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;  /* belum COMPLETED -> compute return RK_NOT_READY */
    RecapAggregate agg = root_recap_build(root_ranking_build().protocol);
    RankingEntryVO ranking[1];
    SearchResultVO details[1];
    assert(agent_recap_prepare(&agg, &state, ranking, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_orchestrator_compute_fails\n");
}

void run_coverage_gap_tests(void) {
    printf("=== Coverage Gap Tests ===\n");
    test_storage_orchestrator_null();
    test_storage_adapter_null_params();
    test_export_orchestrator_null();
    test_export_adapter_null_and_empty();
    test_sanitizer_int_edge();
    test_recap_orchestrator_compute_fails();
    printf("  All coverage gap tests passed!\n\n");
}
