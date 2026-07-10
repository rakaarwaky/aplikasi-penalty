#include "cli/module.cli.h"

#include <stdio.h>

void cli_surfaces_menu_header(void) {
    printf("==================================================\n");
    printf("      APLIKASI LOMBA TENDANGAN PENALTI\n");
    printf("==================================================\n");
}

void cli_surfaces_menu_display(CompetitionStateKind state) {
    printf("\n--- MENU UTAMA ---\n");
    printf("0. Keluar\n");
    if (state == STATE_INIT) {
        printf("1. Pendaftaran Peserta\n");
        printf("2. Input Tendangan [BLOKIR - daftar dulu]\n");
        printf("3. Ranking [BLOKIR]\n4. Cari [BLOKIR]\n5. Rekap [BLOKIR]\n");
    } else if (state == STATE_REGISTERED) {
        printf("1. Pendaftaran [SUDAH]\n2. Input Tendangan\n");
        printf("3. Ranking [BLOKIR]\n4. Cari [BLOKIR]\n5. Rekap [BLOKIR]\n");
    } else {
        printf("1. Pendaftaran [SUDAH]\n2. Input Tendangan [SELESAI]\n");
        printf("3. Ranking\n4. Cari\n5. Rekap\n");
    }
    printf("Pilih: ");
}

void cli_surfaces_menu_dispatch(int choice, RegistrationAggregate *reg,
                                ScoringAggregate *sc, RankingAggregate *rk,
                                SearchAggregate *sr, RecapAggregate *rc,
                                CompetitionState *state) {
    switch (choice) {
        case MENU_EXIT: break;
        case MENU_REGISTRATION:
            if (state->state == STATE_INIT)
                cli_surfaces_registration_execute(reg, state);
            else printf("Pendaftaran sudah ditutup.\n");
            break;
        case MENU_SCORING:
            if (state->state == STATE_INIT) printf("Daftar peserta dulu.\n");
            else cli_surfaces_scoring_execute(sc, state);
            break;
        case MENU_RANKING:
            if (state->state != STATE_COMPLETED) printf("Selesaikan tendangan dulu.\n");
            else cli_surfaces_ranking_execute(rk, state);
            break;
        case MENU_SEARCH:
            if (state->state == STATE_INIT) printf("Daftar peserta dulu.\n");
            else cli_surfaces_search_execute(sr, state);
            break;
        case MENU_RECAP:
            if (state->state != STATE_COMPLETED) printf("Selesaikan tendangan dulu.\n");
            else cli_surfaces_recap_execute(rc, state);
            break;
        default: printf("Menu tidak valid.\n");
    }
}
