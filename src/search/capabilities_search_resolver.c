/**
 * @file capabilities_search_resolver.c
 * @brief Capability: cari peserta berdasar nama (murni, tanpa I/O).
 */

#include "search/module.search.h"

#include <string.h>
#include <ctype.h>

/* ──────────────────────────────────────────────
 * Helper lokal (static): perbandingan string
 * case-insensitive. Sama persis dengan yg di
 * registration validator — duplikasi disengaja
 * (masing-masing capability mandiri, tanpa shar
 * util lintas file untuk jaga batas layer).
 * ────────────────────────────────────────────── */
static int ci_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

/**
 * Cari peserta berdasar nama (case-insensitive, kecocokan persis).
 *
 * Bila ditemukan, isikan SearchResultVO: found=1, participant_id,
 * salin nama/total_score/kicks/zone_freq. Bila tidak, found=0.
 *
 * @param state  Pointer ke state kompetisi (read-only).
 * @param name   Pointer ke VO nama yang dicari.
 * @param out    Pointer ke SearchResultVO tujuan hasil.
 * @return       SearchError: SR_NOT_FOUND (NULL/kosong/tidak ada),
 *               SR_EMPTY_QUERY (query kosong), SR_OK.
 */
SearchError capabilities_search_find(const CompetitionState *state,
                                     const ParticipantNameVO *name,
                                     SearchResultVO *out) {
    /* Guard: pointer wajib. */
    if (state == NULL || name == NULL || out == NULL) return SR_NOT_FOUND;
    /* Guard: query kosong. */
    if (name->value[0] == '\0') return SR_EMPTY_QUERY;

    /* Linear scan peserta terdaftar. */
    for (int i = 0; i < state->participant_count; i++) {
        if (ci_equal(state->participants[i].name.value, name->value)) {
            out->found = 1;
            out->participant_id = i;
            strncpy(out->name, state->participants[i].name.value, MAX_NAME_LENGTH);
            out->name[MAX_NAME_LENGTH] = '\0';
            out->total_score = state->participants[i].total_score;
            for (int k = 0; k < TOTAL_KICKS; k++) out->kicks[k] = state->participants[i].kicks[k];
            for (int z = 0; z <= MAX_ZONE; z++) out->zone_freq[z] = state->participants[i].zone_freq[z];
            return SR_OK;
        }
    }
    /* Tidak ditemukan. */
    out->found = 0;
    return SR_NOT_FOUND;
}
