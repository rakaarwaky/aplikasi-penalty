/**
 * @file root_registration_container.c
 * @brief Siapkan struct pendaftaran dan sambungkan ke fungsi cek & simpan.
 */

#include "registration/module.registration.h"

/**
 * Bangun struct pendaftaran: isi dengan alamat fungsi cek nama
 * dan fungsi simpan peserta agar siap dipakai seluruh program.
 *
 * @return RegistrationAggregate yang sudah terisi.
 */
RegistrationAggregate root_registration_build(void) {
    static RegistrationProtocol protocol;
    protocol.validate_name = capabilities_registration_validate_name;
    protocol.append_participant = capabilities_registration_append;

    RegistrationAggregate a;
    a.protocol = &protocol;
    return a;
}
