/* TESTS — Coverage Gaps (tutup seluruh branch ke 100%) */
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

/* ── Storage orchestrator: null-guard & protocol->save==NULL ── */
static void test_storage_orchestrator_null(void) {
    CompetitionState state = {0};
    assert(agent_storage_save(NULL, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);
    StorageAggregate agg = { .protocol = NULL };
    assert(agent_storage_save(&agg, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_load(NULL, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_load(&agg, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);

    /* protocol ada tapi fungsi save/load NULL */
    StorageProtocol proto = {0};
    StorageAggregate agg2 = { .protocol = &proto };
    assert(agent_storage_save(&agg2, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_load(&agg2, "x.dat", &state) == ST_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_storage_orchestrator_null\n");
}

/* ── Storage adapter: param NULL & I/O error (CORRUPT) ── */
static void test_storage_adapter_edges(void) {
    CompetitionState state = {0};
    state.state = STATE_COMPLETED;
    StorageAggregate agg = root_storage_build();
    assert(agent_storage_save(&agg, NULL, &state) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_save(&agg, "x.dat", NULL) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_load(&agg, NULL, &state) == ST_ERROR_FILE_NOT_FOUND);
    assert(agent_storage_load(&agg, "x.dat", NULL) == ST_ERROR_FILE_NOT_FOUND);

    /* write gagal: /dev/full selalu penuh -> fwrite != 1 -> CORRUPT */
    assert(agent_storage_save(&agg, "/dev/full", &state) == ST_ERROR_CORRUPT);

    /* read gagal: file terlalu kecil -> fread != 1 -> CORRUPT */
    FILE *f = fopen("tiny.dat", "wb");
    assert(f != NULL);
    fputc('x', f);
    fclose(f);
    CompetitionState loaded = {0};
    assert(agent_storage_load(&agg, "tiny.dat", &loaded) == ST_ERROR_CORRUPT);
    remove("tiny.dat");
    printf("  [PASS] test_storage_adapter_edges\n");
}

/* ── Export orchestrator: null-guard & protocol->export_ranking==NULL ── */
static void test_export_orchestrator_null(void) {
    CompetitionState state = {0};
    RankingEntryVO entries[1] = {{0}};
    assert(agent_export_ranking(NULL, "x.txt", &state, entries, 1)
           == EXP_ERROR_FILE_NOT_FOUND);
    ExportAggregate agg = { .protocol = NULL };
    assert(agent_export_ranking(&agg, "x.txt", &state, entries, 1)
           == EXP_ERROR_FILE_NOT_FOUND);

    ExportProtocol proto = {0};
    ExportAggregate agg2 = { .protocol = &proto };
    assert(agent_export_ranking(&agg2, "x.txt", &state, entries, 1)
           == EXP_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_export_orchestrator_null\n");
}

/* ── Export adapter: param NULL, count<=0, fopen gagal ── */
static void test_export_adapter_edges(void) {
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
    /* fopen gagal: direktori tidak ada -> PERMISSION */
    assert(agent_export_ranking(&agg, "/no/such/dir/x.txt", &state, entries, 1)
           == EXP_ERROR_PERMISSION);
    printf("  [PASS] test_export_adapter_edges\n");
}

/* ── Sanitizer: edge int (negatif & kosong) ── */
static void test_sanitizer_int_edge(void) {
    assert(sanitizer_validate_int("-5", 0, 10) == SANITIZE_ERROR_TOO_LONG);
    assert(sanitizer_validate_int("", 0, 10) == SANITIZE_ERROR_INVALID_CHARS);
    printf("  [PASS] test_sanitizer_int_edge\n");
}

/* ── Sanitizer orchestrator + container (0% -> 100%) ── */
static void test_sanitizer_orchestrator(void) {
    /* container: build menghasilkan agg dengan protocol terisi */
    SanitizeAggregate ok = root_sanitize_build();
    assert(ok.protocol != NULL);
    assert(agent_sanitize_validate_string(&ok, "Alice", 20) == SANITIZE_OK);
    assert(agent_sanitize_validate_int(&ok, "5", 0, 10) == SANITIZE_OK);

    /* null-guard: agg/protocol NULL */
    assert(agent_sanitize_validate_string(NULL, "x", 10) == SANITIZE_ERROR_NULL_INPUT);
    SanitizeAggregate agg = { .protocol = NULL };
    assert(agent_sanitize_validate_string(&agg, "x", 10) == SANITIZE_ERROR_NULL_INPUT);
    assert(agent_sanitize_validate_int(NULL, "5", 0, 10) == SANITIZE_ERROR_NULL_INPUT);
    SanitizeProtocol proto = {0};
    SanitizeAggregate agg2 = { .protocol = &proto };
    assert(agent_sanitize_validate_string(&agg2, "x", 10) == SANITIZE_ERROR_NULL_INPUT);
    assert(agent_sanitize_validate_int(&agg2, "5", 0, 10) == SANITIZE_ERROR_NULL_INPUT);

    /* input tidak valid lewat agg valid -> cabang LOG_ERROR("tidak valid") */
    assert(agent_sanitize_validate_string(&ok, "bad\01", 20) == SANITIZE_ERROR_INVALID_CHARS);
    assert(agent_sanitize_validate_int(&ok, "abc", 0, 10) == SANITIZE_ERROR_INVALID_CHARS);
    printf("  [PASS] test_sanitizer_orchestrator\n");
}

/* ── Recap orchestrator: compute_ranking gagal -> RC_NOT_READY ── */
static void test_recap_orchestrator_compute_fails(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;  /* belum COMPLETED -> compute RK_NOT_READY */
    RecapAggregate agg = root_recap_build(root_ranking_build().protocol);
    RankingEntryVO ranking[1];
    SearchResultVO details[1];
    assert(agent_recap_prepare(&agg, &state, ranking, details, 1) == RC_NOT_READY);
    printf("  [PASS] test_recap_orchestrator_compute_fails\n");
}

void run_coverage_gap_tests(void) {
    printf("=== Coverage Gap Tests ===\n");
    test_storage_orchestrator_null();
    test_storage_adapter_edges();
    test_export_orchestrator_null();
    test_export_adapter_edges();
    test_sanitizer_int_edge();
    test_sanitizer_orchestrator();
    test_recap_orchestrator_compute_fails();
    printf("  All coverage gap tests passed!\n\n");
}
