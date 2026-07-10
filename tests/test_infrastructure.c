/* TESTS — Infrastructure Tests */
#include "infrastructure/infrastructure_storage.h"
#include "infrastructure/infrastructure_export.h"
#include "infrastructure/infrastructure_logger.h"
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* --- Storage Tests --- */
static void test_storage_init(void) {
    Storage storage;
    StorageError err = storage_init(&storage, "test.dat");
    assert(err == ST_OK);
    assert(strcmp(storage.filename, "test.dat") == 0);
    printf("  [PASS] test_storage_init\n");
}

static void test_storage_init_null(void) {
    StorageError err = storage_init(NULL, "test.dat");
    assert(err == ST_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_storage_init_null\n");
}

static void test_storage_save_load(void) {
    Storage storage;
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    
    /* Register a participant */
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "TestPlayer");
    capabilities_registration_append(&state, &name);
    
    /* Save */
    StorageError err = storage_init(&storage, "test_save.dat");
    assert(err == ST_OK);
    
    err = storage_save(&storage, &state);
    assert(err == ST_OK);
    
    /* Load */
    CompetitionState loaded = {0};
    err = storage_load(&storage, &loaded);
    assert(err == ST_OK);
    
    /* Verify */
    assert(loaded.participant_count == 1);
    assert(loaded.state == STATE_REGISTERED);
    assert(strcmp(loaded.participants[0].name.value, "TestPlayer") == 0);
    
    /* Cleanup */
    remove("test_save.dat");
    printf("  [PASS] test_storage_save_load\n");
}

static void test_storage_load_not_found(void) {
    Storage storage;
    CompetitionState state = {0};
    
    StorageError err = storage_init(&storage, "nonexistent.dat");
    assert(err == ST_OK);
    
    err = storage_load(&storage, &state);
    assert(err == ST_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_storage_load_not_found\n");
}

/* --- Export Tests --- */
static void test_export_ranking_csv(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    
    StorageError err = export_ranking(&state, "test_ranking.csv", EXPORT_CSV);
    assert(err == ST_OK);
    
    /* Verify file exists */
    FILE *file = fopen("test_ranking.csv", "r");
    assert(file != NULL);
    fclose(file);
    
    remove("test_ranking.csv");
    printf("  [PASS] test_export_ranking_csv\n");
}

static void test_export_recap_csv(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    
    StorageError err = export_recap(&state, "test_recap.csv", EXPORT_CSV);
    assert(err == ST_OK);
    
    /* Verify file exists */
    FILE *file = fopen("test_recap.csv", "r");
    assert(file != NULL);
    fclose(file);
    
    remove("test_recap.csv");
    printf("  [PASS] test_export_recap_csv\n");
}

static void test_export_null_state(void) {
    StorageError err = export_ranking(NULL, "test.csv", EXPORT_CSV);
    assert(err == ST_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_export_null_state\n");
}

void run_infrastructure_tests(void) {
    printf("=== Infrastructure Tests ===\n");
    
    /* Storage tests */
    test_storage_init();
    test_storage_init_null();
    test_storage_save_load();
    test_storage_load_not_found();
    
    /* Export tests */
    test_export_ranking_csv();
    test_export_recap_csv();
    test_export_null_state();
    
    printf("All infrastructure tests passed!\n");
}
