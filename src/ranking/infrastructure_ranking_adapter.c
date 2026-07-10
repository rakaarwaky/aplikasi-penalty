#include "ranking/module.ranking.h"

#include <stdio.h>

static void infra_display_header(void) {
    printf("\n=== RANKING PESERTA ===\n\n");
    printf("%-7s %-30s %-6s %s\n", "Rank", "Nama", "Skor", "Zona(5|4|3|2|1|0)");
    printf("--------------------------------------------------------------\n");
}

static void infra_display_entry(const CompetitionState *state, const RankingEntryVO *e) {
    const char *name = state->participants[e->participant_id].name.value;
    printf("%-7d %-30s %-6d %d|%d|%d|%d|%d|%d\n",
           e->rank, name, e->total_score,
           e->zone_freq[5], e->zone_freq[4], e->zone_freq[3],
           e->zone_freq[2], e->zone_freq[1], e->zone_freq[0]);
}

static void infra_display_not_ready(void) {
    printf("  [GAGAL] Tendangan belum selesai untuk semua peserta.\n");
}

static void infra_display_wait(void) {
    printf("\nTekan Enter untuk melanjutkan...");
    getchar();
}

RankingPort *create_ranking_port(void) {
    static RankingPort port;
    port.display_header = infra_display_header;
    port.display_entry = infra_display_entry;
    port.display_not_ready = infra_display_not_ready;
    port.display_wait = infra_display_wait;
    return &port;
}
