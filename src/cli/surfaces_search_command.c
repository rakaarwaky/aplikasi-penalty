#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>

void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;
    if (state->state == STATE_INIT) {
        printf("  [GAGAL] Daftar peserta dulu.\n");
        return;
    }

    printf("\n=== CARI PESERTA ===\n");
    printf("Masukkan nama peserta: ");
    fflush(stdout);

    char buffer[MAX_NAME_LENGTH + 2];
    if (fgets(buffer, sizeof buffer, stdin) == NULL) return;
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
        buffer[--len] = '\0';
    if (len == 0) { printf("  [GAGAL] Nama pencarian kosong.\n"); return; }

    ParticipantNameVO name;
    strncpy(name.value, buffer, MAX_NAME_LENGTH);
    name.value[MAX_NAME_LENGTH] = '\0';

    SearchResultVO r;
    SearchError e = agent_search_find(agg, state, &name, &r);
    if (e == SR_OK) {
        printf("\n  === Detail Peserta ===\n");
        printf("  Nama       : %s\n", r.name);
        printf("  Total Skor : %d\n", r.total_score);
        printf("  Tendangan  : ");
        for (int k = 0; k < TOTAL_KICKS; k++) printf("%d ", r.kicks[k]);
        printf("\n  Zona       : ");
        for (int z = 0; z <= MAX_ZONE; z++) printf("Z%d:%d ", z, r.zone_freq[z]);
        printf("\n");
    } else {
        printf("  Peserta '%s' tidak ditemukan.\n", buffer);
    }

    printf("\nTekan Enter untuk melanjutkan...");
    getchar();
}
