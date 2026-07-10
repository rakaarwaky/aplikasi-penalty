#include <stdio.h>

extern void run_registration_tests(void);
extern void run_scoring_tests(void);
extern void run_ranking_tests(void);
extern void run_search_tests(void);
extern void run_recap_tests(void);
extern void run_infrastructure_tests(void);
extern void run_sanitizer_tests(void);
extern void run_integration_tests(void);
extern void run_export_tests(void);
extern void run_logger_tests(void);

int main(void) {
    printf("========================================\n");
    printf("  UNIT TESTS — C-AES Penalty Shootout\n");
    printf("========================================\n\n");

    run_registration_tests();
    run_scoring_tests();
    run_ranking_tests();
    run_search_tests();
    run_recap_tests();
    run_infrastructure_tests();
    run_sanitizer_tests();
    run_integration_tests();
    run_export_tests();
    run_logger_tests();

    printf("========================================\n");
    printf("  ALL TESTS PASSED!\n");
    printf("========================================\n");

    return 0;
}
