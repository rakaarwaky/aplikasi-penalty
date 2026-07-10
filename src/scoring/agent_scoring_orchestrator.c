/**
 * @file agent_scoring_orchestrator.c
 * @brief Urutkan pencatatan tendangan: validasi zona dulu, lalu catat.
 */

#include "scoring/module.scoring.h"

/**
 * Catat tendangan peserta: pastikan zona sah, lalu simpan.
 *
 * @param agg   Penghubung ke fungsi cek & catat tendangan.
 * @param state Data lomba yang akan diubah.
 * @param id    Nomor peserta.
 * @param zone  Zona tendangan.
 * @return      Hasil validasi/pencatatan, atau SC_NOT_REGISTERED bila kosong.
 */
ScoringError agent_scoring_record(ScoringAggregate *agg,
                                  CompetitionState *state,
                                  int id, ZoneVO zone) {
    /* Parameter wajib tidak boleh kosong. */
    if (agg == NULL || state == NULL) return SC_NOT_REGISTERED;

    /* Langkah 1: periksa dulu apakah zona sah. */
    ScoringError ve = agg->protocol->validate_zone(zone);
    if (ve != SC_OK) return ve; /* zona tak sah -> hentikan */

    /* Langkah 2: zona sah -> catat tendangan. */
    return agg->protocol->record_kick(state, id, zone);
}
