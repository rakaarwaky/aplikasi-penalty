#include "recap/module.recap.h"

#include <string.h>
#include <stdlib.h>

/* Recap menghitung ranking sendiri (tanpa import fitur ranking -> AES201 aman). */
static int compare_entries(const void *a, const void *b) {
    const RankingEntryVO *x = (const RankingEntryVO *)a;
    const RankingEntryVO *y = (const RankingEntryVO *)b;
    if (x->total_score != y->total_score) return y->total_score - x->total_score;
    for (int z = MAX_ZONE; z >= 1; z--)
        if (x->zone_freq[z] != y->zone_freq[z])
            return y->zone_freq[z] - x->zone_freq[z];
    return 0;
}

RecapError capabilities_recap_prepare(const CompetitionState *state,
                                      RankingEntryVO *ranking,
                                      SearchResultVO *details,
                                      int capacity) {
    if (state == NULL || ranking == NULL || details == NULL) return RC_NOT_READY;
    if (state->state != STATE_COMPLETED) return RC_NOT_READY;
    if (capacity < state->participant_count) return RC_NOT_READY;

    for (int i = 0; i < state->participant_count; i++) {
        ranking[i].participant_id = i;
        ranking[i].total_score = state->participants[i].total_score;
        for (int z = 0; z <= MAX_ZONE; z++)
            ranking[i].zone_freq[z] = state->participants[i].zone_freq[z];
        ranking[i].rank = 0;

        details[i].found = 1;
        details[i].participant_id = i;
        strncpy(details[i].name, state->participants[i].name.value, MAX_NAME_LENGTH);
        details[i].name[MAX_NAME_LENGTH] = '\0';
        details[i].total_score = state->participants[i].total_score;
        for (int k = 0; k < TOTAL_KICKS; k++) details[i].kicks[k] = state->participants[i].kicks[k];
        for (int z = 0; z <= MAX_ZONE; z++) details[i].zone_freq[z] = state->participants[i].zone_freq[z];
    }
    qsort(ranking, state->participant_count, sizeof(RankingEntryVO), compare_entries);
    if (state->participant_count > 0) {
        ranking[0].rank = 1;
        for (int i = 1; i < state->participant_count; i++) {
            if (compare_entries(&ranking[i - 1], &ranking[i]) == 0)
                ranking[i].rank = ranking[i - 1].rank;
            else
                ranking[i].rank = i + 1;
        }
    }
    return RC_OK;
}
