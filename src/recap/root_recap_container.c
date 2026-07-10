#include "recap/module.recap.h"

RecapAggregate root_recap_build(void) {
    static RecapProtocol protocol;
    protocol.prepare_recap = capabilities_recap_prepare;

    RecapAggregate a;
    a.protocol = &protocol;
    a.port = create_recap_port();
    return a;
}
