/**
 * @file capabilities_scoring_score_calculator.c
 * @brief Catat satu tendangan peserta dan akumulasikan skornya.
 */

#include "scoring/module.scoring.h"

/**
 * Rekam tendangan ke-`kick_count` milik peserta, lalu perbarui skor.
 *
 * Yang diubah: simpan zona ke riwayat, tambah skor, hitung frekuensi
 * zona, dan tandai bila seluruh peserta sudah menyelesaikan tendangan.
 *
 * @param state  Data lomba yang akan diubah.
 * @param id     Nomor peserta (indeks dalam data).
 * @param zone   Zona tendangan yang dicatat.
 * @return       SC_OK bila berhasil, atau error bila id/peserta tak sah.
 */
ScoringError capabilities_scoring_record_kick(CompetitionState *state, int id, ZoneVO zone) {
    /* Nomor peserta harus valid. */
    if (state == NULL || id < 0 || id >= state->participant_count)
        return SC_PARTICIPANT_NOT_FOUND;
    ParticipantEntity *p = &state->participants[id];

    /* Peserta sudah melakukan semua tendangannya. */
    if (p->kick_count.value >= TOTAL_KICKS) return SC_ALREADY_DONE;

    /* Simpan hasil tendangan ke slot berikutnya. */
    int k = p->kick_count.value;
    p->kicks[k].zone = zone.value;
    p->kicks[k].points = zone.value;
    p->total_score.value += zone.value;       /* skor = jumlah semua zona */
    p->zone_freq.freq[zone.value]++;          /* hitung frekuensi zona ini */
    p->kick_count.value++;

    /* Bila semua peserta sudah selesai, tandai lomba selesai. */
    if (state->state == STATE_REGISTERED) {
        int all = 1;
        for (int i = 0; i < state->participant_count; i++)
            if (state->participants[i].kick_count.value < TOTAL_KICKS) { all = 0; break; }
        if (all) state->state = STATE_COMPLETED;
    }
    return SC_OK;
}
