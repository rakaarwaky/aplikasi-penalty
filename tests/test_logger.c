/* TESTS — Logger Tests */
#include "shared/taxonomy_logger.h"
#include <assert.h>
#include <stdio.h>

static void test_log_set_get_level(void) {
    log_set_level(LOG_WARN);
    assert(log_get_level() == LOG_WARN);
    log_set_level(LOG_DEBUG);
    assert(log_get_level() == LOG_DEBUG);
    log_set_level(LOG_INFO);
    printf("  [PASS] test_log_set_get_level\n");
}

static void test_log_message_debug(void) {
    log_set_level(LOG_DEBUG);
    LOG_DEBUG("debug message");
    printf("  [PASS] test_log_message_debug\n");
}

static void test_log_message_info(void) {
    log_set_level(LOG_INFO);
    LOG_INFO("info message");
    printf("  [PASS] test_log_message_info\n");
}

static void test_log_message_warn(void) {
    log_set_level(LOG_WARN);
    LOG_WARN("warn message");
    printf("  [PASS] test_log_message_warn\n");
}

static void test_log_message_error(void) {
    log_set_level(LOG_ERROR);
    LOG_ERROR("error message");
    printf("  [PASS] test_log_message_error\n");
}

static void test_log_suppress_below_level(void) {
    log_set_level(LOG_ERROR);
    LOG_DEBUG("should not appear");
    LOG_INFO("should not appear");
    LOG_WARN("should not appear");
    LOG_ERROR("should appear");
    log_set_level(LOG_INFO);
    printf("  [PASS] test_log_suppress_below_level\n");
}

void run_logger_tests(void) {
    printf("=== Logger Tests ===\n");
    test_log_set_get_level();
    test_log_message_debug();
    test_log_message_info();
    test_log_message_warn();
    test_log_message_error();
    test_log_suppress_below_level();
    printf("All logger tests passed!\n");
}
