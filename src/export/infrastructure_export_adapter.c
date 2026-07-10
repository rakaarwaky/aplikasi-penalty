/**
 * @file infrastructure_export_adapter.c
 * @brief Tulis hasil ke file teks berformat tabel.
 */

/* EXPORT — Adapter Infrastruktur (penulis file TXT) */
#include "module.export.h"
#include <stdio.h>
#include <string.h>

/* Tulis peringkat ke file .txt: judul, tabel rank/nama/skor, jumlah peserta. */
static ExportError export_ranking_txt(const char *filename,
                                       const CompetitionState *state,
                                       const RankingEntryVO *entries,
                                       int count) {
    if (filename == NULL || state == NULL || entries == NULL) {
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    if (count <= 0) {
        return EXP_ERROR_EMPTY_DATA;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return EXP_ERROR_PERMISSION;
    }

    fprintf(file, "========================================\n");
    fprintf(file, "  RANKING — Lomba Tendangan Penalti\n");
    fprintf(file, "========================================\n\n");

    fprintf(file, "%-6s %-20s %s\n", "Rank", "Nama", "Skor");
    fprintf(file, "------ -------------------- -----\n");

    for (int i = 0; i < count; i++) {
        int pid = entries[i].participant_id;
        const char *name = "Tidak diketahui";
        if (pid >= 0 && pid < state->participant_count) {
            name = state->participants[pid].name.value;
        }
        fprintf(file, "%-6d %-20s %d\n",
                entries[i].rank,
                name,
                entries[i].total_score);
    }

    fprintf(file, "\n========================================\n");
    fprintf(file, "  Total peserta: %d\n", count);
    fprintf(file, "========================================\n");

    fclose(file);
    return EXP_OK;
}

/* Tulis rekapitulasi lengkap ke file .txt. */
static ExportError export_recap_txt(const char *filename,
                                     const CompetitionState *state,
                                     const RankingEntryVO *ranking,
                                     const SearchResultVO *details,
                                     int count,
                                     int total_score,
                                     int avg_score,
                                     int highest_score) {
    if (filename == NULL || state == NULL || ranking == NULL || details == NULL) {
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    if (count <= 0) {
        return EXP_ERROR_EMPTY_DATA;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return EXP_ERROR_PERMISSION;
    }

    fprintf(file, "========================================\n");
    fprintf(file, "  REKAPITULASI LENGKAP\n");
    fprintf(file, "  Lomba Tendangan Penalti\n");
    fprintf(file, "========================================\n\n");

    fprintf(file, "%-4s %-20s %-6s %s\n", "No", "Nama", "Skor", "Zona(0 1 2 3 4 5)");
    fprintf(file, "---- -------------------- ------ ------------------\n");

    for (int i = 0; i < count; i++) {
        const RankingEntryVO *r = &ranking[i];
        const SearchResultVO *d = &details[r->participant_id];
        fprintf(file, "%-4d %-20s %-6d ", r->rank, d->name, d->total_score);
        for (int z = 0; z <= MAX_ZONE; z++) {
            fprintf(file, "%d ", d->zone_freq[z]);
        }
        fprintf(file, "\n");
    }

    fprintf(file, "\n========================================\n");
    fprintf(file, "  Peserta     : %d\n", count);
    fprintf(file, "  Total Skor  : %d\n", total_score);
    fprintf(file, "  Rata-rata   : %d\n", avg_score);
    fprintf(file, "  Tertinggi   : %d\n", highest_score);

    if (count > 0) {
        const char *winner = state->participants[ranking[0].participant_id].name.value;
        fprintf(file, "\n  JUARA: %s dengan %d poin!\n", winner, ranking[0].total_score);
    }

    fprintf(file, "========================================\n");

    fclose(file);
    return EXP_OK;
}

/* Tulis data satu peserta ke file .txt. */
static ExportError export_participant_txt(const char *filename,
                                           const SearchResultVO *participant) {
    if (filename == NULL || participant == NULL) {
        return EXP_ERROR_FILE_NOT_FOUND;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return EXP_ERROR_PERMISSION;
    }

    fprintf(file, "========================================\n");
    fprintf(file, "  DATA PESERTA\n");
    fprintf(file, "========================================\n\n");

    fprintf(file, "Nama        : %s\n", participant->name);
    fprintf(file, "Total Skor  : %d poin\n\n", participant->total_score);

    fprintf(file, "Riwayat Tendangan:\n");
    fprintf(file, "----------------------------------------\n");
    for (int k = 0; k < TOTAL_KICKS; k++) {
        if (participant->kicks[k] == -1) {
            fprintf(file, "  Tendangan %d: - (belum dilakukan)\n", k + 1);
        } else {
            fprintf(file, "  Tendangan %d: Zona %d\n", k + 1, participant->kicks[k]);
        }
    }

    fprintf(file, "\nFrekuensi Zona:\n");
    fprintf(file, "----------------------------------------\n");
    for (int z = 0; z <= MAX_ZONE; z++) {
        fprintf(file, "  Zona %d: %d kali\n", z, participant->zone_freq[z]);
    }

    fprintf(file, "\n========================================\n");

    fclose(file);
    return EXP_OK;
}

/**
 * Siapkan struct ekspor: sambungkan fungsi tulis file teks.
 */
ExportProtocol export_adapter_create(void) {
    ExportProtocol p;
    p.export_ranking = export_ranking_txt;
    p.export_recap = export_recap_txt;
    p.export_participant = export_participant_txt;
    return p;
}
