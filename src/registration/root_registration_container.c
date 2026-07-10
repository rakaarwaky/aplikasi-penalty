#include "registration/module.registration.h"

RegistrationAggregate root_registration_build(void) {
    static RegistrationProtocol protocol;
    protocol.validate_name = capabilities_registration_validate_name;
    protocol.append_participant = capabilities_registration_append;

    RegistrationAggregate a;
    a.protocol = &protocol;
    return a;
}
