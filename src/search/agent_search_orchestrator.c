#include "search/module.search.h"

#include <stdio.h>
#include <string.h>

SearchError agent_search_run(SearchAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return SR_NOT_FOUND;
    if (state->state == STATE_INIT) {
        printf("  [GAGAL] Daftar peserta dulu.\n");
        return SR_NOT_FOUND;
    }
    agg->port->display_header();
    agg->port->display_prompt();

    char buffer[MAX_NAME_LENGTH + 2];
    SearchError re = agg->port->read_name(buffer, sizeof buffer);
    if (re != SR_OK) { agg->port->display_wait(); return re; }

    ParticipantNameVO name;
    strncpy(name.value, buffer, MAX_NAME_LENGTH);
    name.value[MAX_NAME_LENGTH] = '\0';

    SearchResultVO result;
    SearchError fe = agg->protocol->find_participant(state, &name, &result);
    if (fe == SR_OK) agg->port->display_result(&result);
    else agg->port->display_not_found(buffer);

    agg->port->display_wait();
    return fe;
}
