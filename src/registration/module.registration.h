/**
 * @file module.registration.h
 * @brief Kumpulan fungsi fitur pendaftaran (cari lewat satu file ini).
 */

#ifndef MODULE_REGISTRATION_H
#define MODULE_REGISTRATION_H

#include "shared/contract_registration_aggregate.h"

/* Cek & simpan peserta (diisi alamatnya saat rakit). */
RegistrationError capabilities_registration_validate_name(const CompetitionState *state,
                                                          const ParticipantNameVO *name);
RegistrationError capabilities_registration_append(CompetitionState *state,
                                                   const ParticipantNameVO *name);

/* Urutkan pendaftaran: validasi lalu simpan. */
RegistrationError agent_registration_add(RegistrationAggregate *agg,
                                         CompetitionState *state,
                                         const ParticipantNameVO *name);

/* Siapkan struct pendaftaran. */
RegistrationAggregate root_registration_build(void);

#endif /* MODULE_REGISTRATION_H */
