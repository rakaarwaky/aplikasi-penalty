/**
 * @file capabilities_scoring_score_calculator.c
 * @brief Capability: catat satu tendangan & akumulasi skor (murni, tanpa I/O).
 */

#include "scoring/module.scoring.h"

/**
 * Catat tendangan ke-`kick_count` milik peserta `id`.
 *
 * Efek samping (mutasi state):
 *   - kicks[kick_count]   = zona
 *   - total_score         += zona
 *   - zone_freq[zona]     += 1
 *   - kick_count          += 1
 *   - Bila SELURUH peserta sudah TOTAL_KICKS tendangan → state ke STATE_COMPLETED.
 *
 * @param state  Pointer ke state kompetisi (dimodifikasi).
 * @param id     ID peserta (indeks array).
 * @param zone   ZoneVO zona tendangan.
 * @return       ScoringError: SC_PARTICIPANT_NOT_FOUND (id salah),
 *               SC_ALREADY_DONE (kuota tendangan peserta penuh), SC_OK.
 */
ScoringError capabilities_scoring_record_kick(CompetitionState *state, int id, ZoneVO zone) {
    /* Guard: id harus valid & dalam rentang peserta terdaftar. */
    if (state == NULL || id < 0 || id >= state->participant_count)
        return SC_PARTICIPANT_NOT_FOUND;
    ParticipantEntity *p = &state->participants[id];

    /* Guard: peserta sudah menyelesaikan semua tendangannya. */
    if (p->kick_count >= TOTAL_KICKS) return SC_ALREADY_DONE;

    /* Rekam tendangan ke slot berikutnya. */
    int k = p->kick_count;
    p->kicks[k] = zone.value;
    p->total_score += zone.value;
    p->zone_freq[zone.value]++;
    p->kick_count++;

    /* Cek transisi state machine: REGISTERED -> COMPLETED bila semua selesai. */
    if (state->state == STATE_REGISTERED) {
        int all = 1;
        for (int i = 0; i < state->participant_count; i++)
            if (state->participants[i].kick_count < TOTAL_KICKS) { all = 0; break; }
        if (all) state->state = STATE_COMPLETED;
    }
    return SC_OK;
}
