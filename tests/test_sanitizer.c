/* TESTS — Sanitizer Tests */
#include "module.sanitizer.h"
#include <assert.h>
#include <stdio.h>

static void test_validate_string_ok(void) {
    assert(sanitizer_validate_string("Hello", 10) == SANITIZE_OK);
    printf("  [PASS] test_validate_string_ok\n");
}

static void test_validate_string_null(void) {
    assert(sanitizer_validate_string(NULL, 10) == SANITIZE_ERROR_NULL_INPUT);
    printf("  [PASS] test_validate_string_null\n");
}

static void test_validate_string_too_long(void) {
    assert(sanitizer_validate_string("VeryLongString", 5) == SANITIZE_ERROR_TOO_LONG);
    printf("  [PASS] test_validate_string_too_long\n");
}

static void test_validate_string_invalid_chars(void) {
    assert(sanitizer_validate_string("Hello\x01", 10) == SANITIZE_ERROR_INVALID_CHARS);
    printf("  [PASS] test_validate_string_invalid_chars\n");
}

static void test_validate_int_ok(void) {
    assert(sanitizer_validate_int("5", 0, 10) == SANITIZE_OK);
    printf("  [PASS] test_validate_int_ok\n");
}

static void test_validate_int_null(void) {
    assert(sanitizer_validate_int(NULL, 0, 10) == SANITIZE_ERROR_NULL_INPUT);
    printf("  [PASS] test_validate_int_null\n");
}

static void test_validate_int_invalid(void) {
    assert(sanitizer_validate_int("abc", 0, 10) == SANITIZE_ERROR_INVALID_CHARS);
    printf("  [PASS] test_validate_int_invalid\n");
}

static void test_validate_int_out_of_range(void) {
    assert(sanitizer_validate_int("15", 0, 10) == SANITIZE_ERROR_OUT_OF_RANGE);
    printf("  [PASS] test_validate_int_out_of_range\n");
}

/* Batas bawah PERSIS (min=0, input "0") harus sah.
   Tanpa test ini, mutasi "val < min" -> "val <= min" lolos (survivor). */
static void test_validate_int_boundary_min(void) {
    assert(sanitizer_validate_int("0", 0, 10) == SANITIZE_OK);
    assert(sanitizer_validate_int("-1", 0, 10) == SANITIZE_ERROR_OUT_OF_RANGE);
    printf("  [PASS] test_validate_int_boundary_min\n");
}

void run_sanitizer_tests(void) {
    printf("=== Sanitizer Tests ===\n");
    test_validate_string_ok();
    test_validate_string_null();
    test_validate_string_too_long();
    test_validate_string_invalid_chars();
    test_validate_int_ok();
    test_validate_int_null();
    test_validate_int_invalid();
    test_validate_int_out_of_range();
    test_validate_int_boundary_min();
    printf("All sanitizer tests passed!\n");
}
