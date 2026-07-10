/* TESTS — Surfaces (CLI/TUI) headless via fake ncurses.
 * Mengendalikan input lewat antrian fake_ncurses, memanggil surfaces
 * sungguhan, lalu assert perubahan state. Menutup coverage layer cli/tui. */
#include "shared/contract_display_port.h"
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"
#include "sanitizer/module.sanitizer.h"
#include "cli/module.cli.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* API fake ncurses (diimplementasikan tests/fake_ncurses.c) */
extern void fake_tui_push_key(int k);
extern void fake_tui_push_str(const char *s);
extern void fake_tui_reset_input(void);

/* Build DisplayPort asli (tapi ncurses di-stub -> aman headless). */
static DisplayPort make_dp(void) { return tui_display_port_create(); }

/* Isi state dengan n peserta terdaftar (siap untuk scoring). */
static CompetitionState seeded_state(int n) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < n; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 20, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    return state;
}

static void test_registration_surface(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    RegistrationAggregate agg = root_registration_build();
    SanitizeAggregate sn = root_sanitize_build();
    DisplayPort dp = make_dp();

    fake_tui_reset_input();
    fake_tui_push_str("Budi");
    fake_tui_push_str("Sari");
    fake_tui_push_str("Tono");
    fake_tui_push_str("Wati");
    fake_tui_push_str("Joko");
    fake_tui_push_str("");        /* kosong -> selesai (sudah >= MIN) */
    fake_tui_push_key('\n');      /* readkey() terakhir */

    cli_surfaces_registration_execute(&agg, &state, &dp, &sn);
    assert(state.participant_count == 5);
    assert(state.state == STATE_REGISTERED);
    printf("  [PASS] test_registration_surface\n");
}

static void test_registration_error_paths(void) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    RegistrationAggregate agg = root_registration_build();
    SanitizeAggregate sn = root_sanitize_build();
    DisplayPort dp = make_dp();

    fake_tui_reset_input();
    fake_tui_push_str("Budi123");   /* invalid char -> error, lanjut */
    fake_tui_push_str("Budi");
    fake_tui_push_str("Sari");
    fake_tui_push_str("Tono");
    fake_tui_push_str("Wati");
    fake_tui_push_str("Joko");
    fake_tui_push_str("");
    fake_tui_push_key('\n');

    cli_surfaces_registration_execute(&agg, &state, &dp, &sn);
    /* "Budi123" ditolak, sisanya 5 masuk -> tetap 5 */
    assert(state.participant_count == 5);
    printf("  [PASS] test_registration_error_paths\n");
}

static void test_scoring_surface(void) {
    CompetitionState state = seeded_state(5);
    ScoringAggregate agg = root_scoring_build();
    SanitizeAggregate sn = root_sanitize_build();
    DisplayPort dp = make_dp();

    fake_tui_reset_input();
    int p, k;
    for (p = 0; p < 5; p++) {
        for (k = 0; k < TOTAL_KICKS; k++) {
            fake_tui_push_str("5");
        }
    }
    fake_tui_push_key('\n');  /* readkey penutup (jika ada) */

    cli_surfaces_scoring_execute(&agg, &state, &dp, &sn);
    assert(state.state == STATE_COMPLETED);
    assert(state.participants[0].total_score.value == 5 * TOTAL_KICKS);
    printf("  [PASS] test_scoring_surface\n");
}

static void test_scoring_invalid_zone(void) {
    CompetitionState state = seeded_state(2);
    ScoringAggregate agg = root_scoring_build();
    SanitizeAggregate sn = root_sanitize_build();
    DisplayPort dp = make_dp();

    fake_tui_reset_input();
    /* peserta 0: zona invalid lalu valid */
    fake_tui_push_str("99");   /* invalid -> error */
    fake_tui_push_str("3");
    for (k = 1; k < TOTAL_KICKS; k++) fake_tui_push_str("3");
    /* peserta 1: semua valid */
    for (k = 0; k < TOTAL_KICKS; k++) fake_tui_push_str("2");
    fake_tui_push_key('\n');

    cli_surfaces_scoring_execute(&agg, &state, &dp, &sn);
    assert(state.state == STATE_COMPLETED);
    assert(state.participants[0].total_score.value == 3 * TOTAL_KICKS);
    printf("  [PASS] test_scoring_invalid_zone\n");
}

static void test_ranking_search_recap_surfaces(void) {
    CompetitionState state = seeded_state(5);
    /* beri skor manual agar ranking/recap punya data */
    int p, k;
    for (p = 0; p < 5; p++)
        for (k = 0; k < TOTAL_KICKS; k++)
            capabilities_scoring_record_kick(&state, p, (ZoneVO){p % 6});

    RankingAggregate rk = root_ranking_build(root_ranking_build().protocol);
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build(root_ranking_build().protocol);
    DisplayPort dp = make_dp();

    fake_tui_reset_input();
    fake_tui_push_key('q');
    cli_surfaces_ranking_execute(&rk, &state, &dp);

    fake_tui_reset_input();
    fake_tui_push_str("P0");
    fake_tui_push_key('\n');
    cli_surfaces_search_execute(&sr, &state, &dp);

    fake_tui_reset_input();
    fake_tui_push_key('q');
    cli_surfaces_recap_execute(&rc, &state, &dp);

    printf("  [PASS] test_ranking_search_recap_surfaces\n");
}

void run_surface_tests(void) {
    printf("=== Surface (CLI/TUI) Tests ===\n");
    test_registration_surface();
    test_registration_error_paths();
    test_scoring_surface();
    test_scoring_invalid_zone();
    test_ranking_search_recap_surfaces();
    printf("  All surface tests passed!\n\n");
}
