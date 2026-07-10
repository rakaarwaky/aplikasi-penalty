#include <stdio.h>

#include "cli/module.cli.h"

static void print_header(void) {
    printf("==================================================\n");
    printf("      APLIKASI LOMBA TENDANGAN PENALTI\n");
    printf("==================================================\n");
}

static void print_menu(CompetitionStateKind state) {
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

int main(void) {
    CompetitionState state;
    state.participant_count = 0;
    state.state = STATE_INIT;

    RegistrationAggregate reg = root_registration_build();
    ScoringAggregate sc = root_scoring_build();
    RankingAggregate rk = root_ranking_build();
    SearchAggregate sr = root_search_build();
    RecapAggregate rc = root_recap_build();

    int running = 1;
    while (running) {
        print_header();
        print_menu(state.state);
        int choice;
        if (scanf("%d", &choice) != 1) { getchar(); continue; }
        getchar(); /* consume newline */

        switch (choice) {
            case MENU_EXIT: running = 0; break;
            case MENU_REGISTRATION:
                if (state.state == STATE_INIT) cli_surfaces_registration_execute(&reg, &state);
                else printf("Pendaftaran sudah ditutup.\n");
                break;
            case MENU_SCORING:
                if (state.state == STATE_INIT) printf("Daftar peserta dulu.\n");
                else cli_surfaces_scoring_execute(&sc, &state);
                break;
            case MENU_RANKING:
                if (state.state != STATE_COMPLETED) printf("Selesaikan tendangan dulu.\n");
                else cli_surfaces_ranking_execute(&rk, &state);
                break;
            case MENU_SEARCH:
                if (state.state == STATE_INIT) printf("Daftar peserta dulu.\n");
                else cli_surfaces_search_execute(&sr, &state);
                break;
            case MENU_RECAP:
                if (state.state != STATE_COMPLETED) printf("Selesaikan tendangan dulu.\n");
                else cli_surfaces_recap_execute(&rc, &state);
                break;
            default: printf("Menu tidak valid.\n");
        }
    }
    printf("Terima kasih.\n");
    return 0;
}
