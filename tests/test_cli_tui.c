/* TESTS — CLI & TUI coverage via fake DisplayPort (headless, no terminal).
 *
 * Semua surfaces dipanggil lewat DisplayPort *dp yang diisi oleh
 * tui_display_port_create() (stub ncurses). Input disimulasikan lewat
 * antrian di fake_ncurses.c (fake_tui_push_str / fake_tui_push_key).
 */
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"
#include "sanitizer/module.sanitizer.h"
#include "cli/module.cli.h"
#include "tui/infrastructure_tui_adapter.h"   /* root_display_build */

/* API fake ncurses (tests/fake_ncurses.c) */
extern void fake_tui_push_key(int k);
extern void fake_tui_push_str(const char *s);
extern void fake_tui_reset_input(void);

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* TUI_KEY_* mungkin ada di taxonomy_display_constant.h; fallback kalau tidak. */
#ifndef TUI_KEY_ENTER
#define TUI_KEY_ENTER 10
#define TUI_KEY_UP    65
#define TUI_KEY_DOWN  66
#define TUI_KEY_ESC   27
#define TUI_KEY_RESIZE 410
#endif

/* Mainkan pendaftaran + scoring secara langsung (state -> COMPLETED). */
static void play_to_completed(DisplayPort *dp, CompetitionState *state,
                              RegistrationAggregate *reg, ScoringAggregate *sc,
                              SanitizeAggregate *sn) {
    /* Registration: 5 nama valid lalu 1 kosong (selesai) + 1 readkey footer. */
    const char *names[] = {"Budi", "Sari", "Tono", "Wati", "Joko"};
    int i;
    for (i = 0; i < 5; i++) fake_tui_push_str(names[i]);
    fake_tui_push_str("");   /* kosong -> break */
    fake_tui_push_key(TUI_KEY_ENTER);  /* footer lanjut */
    cli_surfaces_registration_execute(reg, state, dp, sn);
    assert(state->participant_count == 5);
    assert(state->state == STATE_REGISTERED);

    /* Scoring: 5 peserta x 7 kick. Tiap kick: 1 zona + 1 readkey;
       tiap peserta: +1 readkey akhir. Total 35 zona + 40 key. */
    for (i = 0; i < 35; i++) fake_tui_push_str("5");
    for (i = 0; i < 40; i++) fake_tui_push_key(TUI_KEY_ENTER);
    cli_surfaces_scoring_execute(sc, state, dp, sn);
    assert(state->state == STATE_COMPLETED);
    assert(state->participants[0].total_score.value == 35);
}

static void test_full_game_via_surfaces(void) {
    CompetitionState state = {0};
    RegistrationAggregate reg = root_registration_build();
    ScoringAggregate sc = root_scoring_build();
    RankingAggregate rk = root_ranking_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build();
    SanitizeAggregate sn = root_sanitize_build();
    DisplayPort dp = root_display_build();

    fake_tui_reset_input();
    play_to_completed(&dp, &state, &reg, &sc, &sn);

    /* Ranking (state COMPLETED) -> 1 readkey. */
    fake_tui_reset_input();
    fake_tui_push_key(TUI_KEY_ENTER);
    cli_surfaces_ranking_execute(&rk, &state, &dp);

    /* Search "Budi" -> 1 string + 1 readkey. */
    fake_tui_reset_input();
    fake_tui_push_str("Budi");
    fake_tui_push_key(TUI_KEY_ENTER);
    cli_surfaces_search_execute(&sr, &state, &dp);

    /* Recap (state COMPLETED) -> 1 readkey. */
    fake_tui_reset_input();
    fake_tui_push_key(TUI_KEY_ENTER);
    cli_surfaces_recap_execute(&rc, &state, &dp);

    printf("  [PASS] test_full_game_via_surfaces\n");
}

/* Menu: dispatch '1' (registration dari state INIT) lalu keluar. */
static void test_menu_dispatch_and_exit(void) {
    CompetitionState state = {0};
    RegistrationAggregate reg = root_registration_build();
    ScoringAggregate sc = root_scoring_build();
    RankingAggregate rk = root_ranking_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build();
    SanitizeAggregate sn = root_sanitize_build();
    DisplayPort dp = root_display_build();

    fake_tui_reset_input();
    fake_tui_push_key('1');           /* shortcut -> registration */
    fake_tui_push_str("Andi");
    fake_tui_push_str("Budi");
    fake_tui_push_str("Caca");
    fake_tui_push_str("Dedi");
    fake_tui_push_str("Ega");
    fake_tui_push_str("");            /* selesai */
    fake_tui_push_key(TUI_KEY_ENTER); /* footer */
    fake_tui_push_key('q');           /* keluar dari menu */
    fake_tui_push_str("y");           /* confirm keluar */
    cli_surfaces_menu_run(&reg, &sc, &rk, &sr, &rc, &state, &dp, &sn);
    assert(state.participant_count == 5);

    /* Menu help screen ('h') lalu keluar. */
    fake_tui_reset_input();
    fake_tui_push_key('h');           /* help */
    fake_tui_push_key(TUI_KEY_ENTER); /* kembali */
    fake_tui_push_key('0');           /* keluar */
    fake_tui_push_str("y");           /* confirm */
    cli_surfaces_menu_run(&reg, &sc, &rk, &sr, &rc, &state, &dp, &sn);

    printf("  [PASS] test_menu_dispatch_and_exit\n");
}

/* Surface dengan state salah -> pesan gagal + readkey (cabang error). */
static void test_surfaces_guard_paths(void) {
    CompetitionState state = {0};  /* STATE_INIT */
    ScoringAggregate sc = root_scoring_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build();
    SanitizeAggregate sn = root_sanitize_build();
    DisplayPort dp = root_display_build();

    fake_tui_reset_input();
    fake_tui_push_key(TUI_KEY_ENTER);
    cli_surfaces_scoring_execute(&sc, &state, &dp, &sn);  /* belum daftar */

    fake_tui_reset_input();
    fake_tui_push_key(TUI_KEY_ENTER);
    cli_surfaces_search_execute(&sr, &state, &dp);        /* belum daftar */

    fake_tui_reset_input();
    fake_tui_push_key(TUI_KEY_ENTER);
    cli_surfaces_recap_execute(&rc, &state, &dp);         /* belum completed */

    printf("  [PASS] test_surfaces_guard_paths\n");
}

void run_cli_tui_tests(void) {
    printf("=== CLI/TUI Tests (fake DisplayPort) ===\n");
    test_full_game_via_surfaces();
    test_menu_dispatch_and_exit();
    test_surfaces_guard_paths();
    printf("  All CLI/TUI tests passed!\n\n");
}
