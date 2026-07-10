/**
 * @file root_sanitize_container.c
 * @brief Siapkan struct sanitasi dan sambungkan ke fungsi validasi.
 */

/* SANITIZER — Root Container (wiring) */
#include "module.sanitizer.h"
#include "shared/contract_sanitize_aggregate.h"

/**
 * Bangun struct sanitasi: isi dengan fungsi validasi string & angka
 * agar siap dipakai seluruh program.
 *
 * @return SanitizeAggregate yang sudah terisi.
 */
SanitizeAggregate root_sanitize_build(void) {
    SanitizeAggregate agg;
    static SanitizeProtocol proto;
    proto = sanitizer_create();
    agg.protocol = &proto;
    return agg;
}
