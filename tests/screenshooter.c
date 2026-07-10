/**
 * screenshooter.c — Populate CompetitionState directly (no TUI input),
 * render each screen, and dump ANSI to stdout for screenshot capture.
 */
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"
#include "sanitizer/module.sanitizer.h"
#include "storage/module.storage.h"
#include "export/module.export.h"
#include "cli/module.cli.h"
#include "shared/taxonomy_game_constant.h"
#include "tui/infrastructure_tui_adapter.h"

#include <stdio.h>
#include <string.h>

extern void fake_tui_push_key(int k);
extern void fake_tui_push_str(const char *s);
extern void fake_tui_reset_input(void);

static void render_menu(DisplayPort *dp, CompetitionState *state) {
    /* Simulate menu screen: draw menu and wait for Enter */
    fake_tui_reset_input();
    fake_tui_push_key(10); /* Enter to dismiss */
    cli_surfaces_menu_draw(dp, 1, state);
}

int main(void) {
    RegistrationAggregate reg = root_registration_build();
    ScoringAggregate sc = root_scoring_build();
    RankingAggregate rk = root_ranking_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build(rk.protocol);
    SanitizeAggregate sn = root_sanitize_build();
    ExportAggregate ex = root_export_build();
    DisplayPort dp = root_display_build();

    CompetitionState state = {0};
    state.state = STATE_INIT;

    tui_init();

    /* === REGISTER 5 PARTICIPANTS === */
    const char *names[] = {"Budi", "Sari", "Tono", "Wati", "Joko"};
    for (int i = 0; i < 5; i++) {
        fake_tui_reset_input();
        fake_tui_push_str(names[i]);
        fake_tui_push_str(""); /* empty = done */
        fake_tui_push_key(10); /* Enter */
        cli_surfaces_registration_execute(&reg, &state, &dp, &sn);
    }

    /* Finish registration: empty name + Enter to go back to menu */
    fake_tui_reset_input();
    fake_tui_push_str("");
    fake_tui_push_key(10);
    cli_surfaces_registration_execute(&reg, &state, &dp, &sn);

    /* Go back to menu */
    fake_tui_reset_input();
    fake_tui_push_key(10);
    cli_surfaces_registration_execute(&reg, &state, &dp, &sn);

    /* === SCORING === */
    int kicks[5][7] = {
        {5, 5, 3, 2, 4, 5, 5},  /* Budi: 29 */
        {4, 4, 4, 3, 3, 2, 1},  /* Sari: 21 */
        {3, 3, 3, 2, 2, 1, 1},  /* Tono: 15 */
        {2, 2, 2, 1, 1, 1, 1},  /* Wati: 10 */
        {1, 1, 1, 1, 1, 1, 1},  /* Joko: 7  */
    };

    fake_tui_reset_input();
    for (int p = 0; p < 5; p++) {
        for (int k = 0; k < 7; k++) {
            char buf[4];
            snprintf(buf, sizeof buf, "%d", kicks[p][k]);
            fake_tui_push_str(buf);
            fake_tui_push_key(10); /* Enter per zone */
        }
        fake_tui_push_key(10); /* Enter after SELESAI screen */
    }
    fake_tui_push_key(10); /* Final Enter for "Semua selesai" */
    cli_surfaces_scoring_execute(&sc, &state, &dp, &sn);

    /* Go back to menu */
    fake_tui_reset_input();
    fake_tui_push_key(10);

    printf("State: %d participants, phase=%d\n", state.participant_count, state.state);
    printf("DONE\n");

    tui_end();
    return 0;
}
