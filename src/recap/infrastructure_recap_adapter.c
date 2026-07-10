#include "recap/module.recap.h"

#include <stdio.h>

static void infra_display_header(void) {
    printf("\n=== REKAPITULASI LENGKAP ===\n\n");
    printf("+------+------------------------------+--------+-------------------------+\n");
    printf("| Rank | Nama                         | Skor   | Frekuensi Zona (0-5)    |\n");
    printf("+------+------------------------------+--------+-------------------------+\n");
}

static void infra_display_row(const CompetitionState *state,
                              const RankingEntryVO *e,
                              const SearchResultVO *d) {
    (void)state;
    printf("| %4d | %-28s | %6d | ", e->rank, d->name, d->total_score);
    for (int z = 0; z <= MAX_ZONE; z++) printf("%d ", d->zone_freq[z]);
    printf("|\n");
}

static void infra_display_not_ready(void) {
    printf("  [GAGAL] Tendangan belum selesai.\n");
}

static void infra_display_wait(void) {
    printf("+------+------------------------------+--------+-------------------------+\n");
    printf("Tekan Enter untuk melanjutkan...");
    getchar();
}

RecapPort *create_recap_port(void) {
    static RecapPort port;
    port.display_header = infra_display_header;
    port.display_row = infra_display_row;
    port.display_not_ready = infra_display_not_ready;
    port.display_wait = infra_display_wait;
    return &port;
}
