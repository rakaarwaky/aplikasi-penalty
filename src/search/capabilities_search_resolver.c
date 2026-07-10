#include "search/module.search.h"

#include <string.h>
#include "shared/taxonomy_string_utility.h"

SearchError capabilities_search_find(const CompetitionState *state,
                                     const ParticipantNameVO *name,
                                     SearchResultVO *out) {
    if (state == NULL || name == NULL || out == NULL) return SR_NOT_FOUND;
    if (name->value[0] == '\0') return SR_EMPTY_QUERY;

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
    out->found = 0;
    return SR_NOT_FOUND;
}
