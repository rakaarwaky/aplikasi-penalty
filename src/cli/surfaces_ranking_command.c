#include "cli/module.cli.h"

#include <stdio.h>

void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        printf("  [GAGAL] Tendangan belum selesai untuk semua peserta.\n");
        return;
    }

    RankingEntryVO entries[MAX_PARTICIPANTS];
    RankingError e = agent_ranking_compute(agg, state, entries, MAX_PARTICIPANTS);
    if (e != RK_OK) { printf("  [GAGAL] Ranking belum siap.\n"); return; }

    printf("\n=== RANKING PESERTA ===\n\n");
    printf("%-7s %-30s %-6s %s\n", "Rank", "Nama", "Skor", "Zona(5|4|3|2|1|0)");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &entries[i];
        const char *name = state->participants[r->participant_id].name.value;
        printf("%-7d %-30s %-6d %d|%d|%d|%d|%d|%d\n",
               r->rank, name, r->total_score,
               r->zone_freq[5], r->zone_freq[4], r->zone_freq[3],
               r->zone_freq[2], r->zone_freq[1], r->zone_freq[0]);
    }

    printf("\nTekan Enter untuk melanjutkan...");
    getchar();
}
