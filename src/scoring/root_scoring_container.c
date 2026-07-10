/**
 * @file root_scoring_container.c
 * @brief Siapkan struct scoring dan sambungkan ke fungsi cek & catat.
 */

#include "scoring/module.scoring.h"

/**
 * Bangun struct scoring: isi dengan alamat fungsi cek zona dan
 * fungsi catat tendangan agar siap dipakai seluruh program.
 *
 * @return ScoringAggregate yang sudah terisi.
 */
ScoringAggregate root_scoring_build(void) {
    static ScoringProtocol protocol;
    protocol.validate_zone = capabilities_scoring_validate_zone;
    protocol.record_kick = capabilities_scoring_record_kick;

    ScoringAggregate a;
    a.protocol = &protocol;
    return a;
}
