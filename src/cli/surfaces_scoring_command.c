#include "cli/module.cli.h"

#include <stdio.h>
#include <ctype.h>

static ScoringError read_zone(ZoneVO *out) {
    char buf[32];
    if (fgets(buf, sizeof buf, stdin) == NULL) return SC_INVALID_ZONE;
    int i = 0;
    while (buf[i] != '\0' && buf[i] != '\n') {
        if (!isdigit((unsigned char)buf[i]) && buf[i] != '-') return SC_INVALID_ZONE;
        i++;
    }
    int z;
    if (sscanf(buf, "%d", &z) != 1) return SC_INVALID_ZONE;
    out->value = z;
    return SC_OK;
}

void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;
    if (state->state == STATE_INIT) {
        printf("  [GAGAL] Daftar peserta dulu (Menu 1).\n");
        return;
    }

    printf("\n=== INPUT TENDANGAN DAN SKOR ===\n");
    printf("Setiap peserta %d tendangan (zona %d-%d).\n\n", TOTAL_KICKS, MIN_ZONE, MAX_ZONE);

    for (int p = 0; p < state->participant_count; p++) {
        ParticipantEntity *part = &state->participants[p];
        while (part->kick_count < TOTAL_KICKS) {
            printf("  %s - tendangan %d/%d, zona (%d-%d): ",
                   part->name.value, part->kick_count + 1, TOTAL_KICKS, MIN_ZONE, MAX_ZONE);
            fflush(stdout);

            ZoneVO z;
            if (read_zone(&z) != SC_OK) { printf("  [GAGAL] Zona harus %d-%d.\n", MIN_ZONE, MAX_ZONE); continue; }

            ScoringError e = agent_scoring_record(agg, state, p, z);
            if (e == SC_OK) printf("  Zona %d -> %d poin\n", z.value, z.value);
            else if (e == SC_INVALID_ZONE) printf("  [GAGAL] Zona harus %d-%d.\n", MIN_ZONE, MAX_ZONE);
            else printf("  [GAGAL] Kesalahan pencatatan.\n");
        }
        printf("  Selesai %s. Total: %d\n", part->name.value, part->total_score);
    }

    if (state->state == STATE_COMPLETED)
        printf("\nSemua tendangan selesai. Status: COMPLETED.\n");
    printf("Tekan Enter untuk melanjutkan...");
    getchar();
}
