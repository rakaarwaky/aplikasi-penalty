#include "cli/module.cli.h"

#include <stdio.h>

void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        printf("  [GAGAL] Tendangan belum selesai.\n");
        return;
    }

    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agent_recap_prepare(agg, state, ranking, details, MAX_PARTICIPANTS);
    if (e != RC_OK) { printf("  [GAGAL] Rekap belum siap.\n"); return; }

    printf("\n=== REKAPITULASI LENGKAP ===\n\n");
    printf("+------+------------------------------+--------+-------------------------+\n");
    printf("| Rank | Nama                         | Skor   | Frekuensi Zona (0-5)    |\n");
    printf("+------+------------------------------+--------+-------------------------+\n");
    for (int i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &ranking[i];
        const SearchResultVO *d = &details[r->participant_id];
        printf("| %4d | %-28s | %6d | ", r->rank, d->name, d->total_score);
        for (int z = 0; z <= MAX_ZONE; z++) printf("%d ", d->zone_freq[z]);
        printf("|\n");
    }
    printf("+------+------------------------------+--------+-------------------------+\n");

    printf("Tekan Enter untuk melanjutkan...");
    getchar();
}
