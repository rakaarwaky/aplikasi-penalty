/* TESTS — Storage Tests */
#include "storage/module.storage.h"
#include "registration/module.registration.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_storage_save_load(void) {
    StorageProtocol storage = storage_capabilities_create();
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    
    ParticipantNameVO name = {{0}};
    strcpy(name.value, "TestPlayer");
    capabilities_registration_append(&state, &name);
    
    StorageError err = storage.save("test.dat", &state);
    assert(err == ST_OK);
    
    CompetitionState loaded = {0};
    err = storage.load("test.dat", &loaded);
    assert(err == ST_OK);
    
    assert(loaded.participant_count == 1);
    assert(strcmp(loaded.participants[0].name.value, "TestPlayer") == 0);
    
    remove("test.dat");
    printf("  [PASS] test_storage_save_load\n");
}

static void test_storage_load_not_found(void) {
    StorageProtocol storage = storage_capabilities_create();
    CompetitionState state = {0};
    
    StorageError err = storage.load("nonexistent.dat", &state);
    assert(err == ST_ERROR_FILE_NOT_FOUND);
    printf("  [PASS] test_storage_load_not_found\n");
}

void run_infrastructure_tests(void) {
    printf("=== Storage Tests ===\n");
    test_storage_save_load();
    test_storage_load_not_found();
    printf("All storage tests passed!\n");
}
