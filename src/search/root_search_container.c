#include "search/module.search.h"

SearchAggregate root_search_build(void) {
    static SearchProtocol protocol;
    protocol.find_participant = capabilities_search_find;

    SearchAggregate a;
    a.protocol = &protocol;
    return a;
}
