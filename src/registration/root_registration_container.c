/**
 * @file root_registration_container.c
 * @brief Root container: rakit RegistrationAggregate (wiring protocol ke capabilities).
 */

#include "registration/module.registration.h"

/**
 * Bangun RegistrationAggregate dengan menyambungkan function-pointer
 * protocol ke implementasi capabilities konkret.
 *
 * Protocol disimpan di `static` agar alamatnya stabil (tidak dibebas
 * setelah fungsi kembali — aggregate hanya menyimpan pointer).
 *
 * @return RegistrationAggregate — berisi pointer protocol yang terisi.
 */
RegistrationAggregate root_registration_build(void) {
    static RegistrationProtocol protocol;
    protocol.validate_name = capabilities_registration_validate_name;
    protocol.append_participant = capabilities_registration_append;

    RegistrationAggregate a;
    a.protocol = &protocol;
    return a;
}
