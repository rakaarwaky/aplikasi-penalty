#include "scoring/module.scoring.h"

#include <stdio.h>
#include <ctype.h>

static void infra_display_header(void) {
    printf("\n=== INPUT TENDANGAN DAN SKOR ===\n");
    printf("Setiap peserta %d tendangan (zona %d-%d).\n\n", TOTAL_KICKS, MIN_ZONE, MAX_ZONE);
}

static void infra_display_prompt(const ParticipantEntity *p, int kick_no) {
    printf("  %s - tendangan %d/%d, zona (0-5): ",
           p->name.value, kick_no, TOTAL_KICKS);
    fflush(stdout);
}

static ScoringError infra_read_zone(ZoneVO *out) {
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

static void infra_display_result(int zone, int points) {
    printf("  Zona %d -> %d poin\n", zone, points);
}

static void infra_display_error(ScoringError e) {
    if (e == SC_INVALID_ZONE) printf("  [GAGAL] Zona harus 0-5.\n");
    else printf("  [GAGAL] Kesalahan pencatatan.\n");
}

static void infra_display_wait(void) {
    printf("\nTekan Enter untuk melanjutkan...");
    getchar();
}

static void infra_clear_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

ScoringPort *create_scoring_port(void) {
    static ScoringPort port;
    port.display_header = infra_display_header;
    port.display_prompt = infra_display_prompt;
    port.read_zone = infra_read_zone;
    port.display_result = infra_display_result;
    port.display_error = infra_display_error;
    port.display_wait = infra_display_wait;
    port.clear_buffer = infra_clear_buffer;
    return &port;
}
