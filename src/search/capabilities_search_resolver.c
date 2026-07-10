/**
 * @file capabilities_search_resolver.c
 * @brief Cari peserta berdasar namanya.
 */

#include "search/module.search.h"

#include <string.h>
#include <ctype.h>

/* Bandingkan dua string tanpa peduli huruf besar/kecil. */
static int ci_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

/**
 * Cari peserta dari namanya (cocok persis, tak peduli kapital).
 *
 * Bila ketemu, isi SearchResultVO dengan data peserta tersebut.
 *
 * @param state  Data lomba (hanya dibaca).
 * @param name   Nama yang dicari.
 * @param out    Tempat menulis hasil pencarian.
 * @return       SR_OK bila ketemu, SR_NOT_FOUND bila tak ada,
 *               SR_EMPTY_QUERY bila nama kosong.
 */
SearchError capabilities_search_find(const CompetitionState *state,
                                     const ParticipantNameVO *name,
                                     SearchResultVO *out) {
    /* Data atau nama tidak sah. */
    if (state == NULL || name == NULL || out == NULL) return SR_NOT_FOUND;
    /* Nama pencarian kosong. */
    if (name->value[0] == '\0') return SR_EMPTY_QUERY;

    /* Cari satu-per-satu peserta yang namanya cocok. */
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
