#include "search/module.search.h"

#include <stdio.h>
#include <string.h>

static void infra_display_header(void) {
    printf("\n=== CARI PESERTA ===\n");
}

static void infra_display_prompt(void) {
    printf("Masukkan nama peserta: ");
    fflush(stdout);
}

static SearchError infra_read_name(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) return SR_NOT_FOUND;
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
        buffer[--len] = '\0';
    if (len == 0) return SR_EMPTY_QUERY;
    return SR_OK;
}

static void infra_display_result(const SearchResultVO *r) {
    printf("\n  === Detail Peserta ===\n");
    printf("  Nama       : %s\n", r->name);
    printf("  Total Skor : %d\n", r->total_score);
    printf("  Tendangan  : ");
    for (int k = 0; k < TOTAL_KICKS; k++) printf("%d ", r->kicks[k]);
    printf("\n  Zona       : ");
    for (int z = 0; z <= MAX_ZONE; z++) printf("Z%d:%d ", z, r->zone_freq[z]);
    printf("\n");
}

static void infra_display_not_found(const char *name) {
    printf("  Peserta '%s' tidak ditemukan.\n", name);
}

static void infra_display_wait(void) {
    printf("\nTekan Enter untuk melanjutkan...");
    getchar();
}

SearchPort *create_search_port(void) {
    static SearchPort port;
    port.display_header = infra_display_header;
    port.display_prompt = infra_display_prompt;
    port.read_name = infra_read_name;
    port.display_result = infra_display_result;
    port.display_not_found = infra_display_not_found;
    port.display_wait = infra_display_wait;
    return &port;
}
