/**
 * @file infrastructure_export_adapter.c
 * @brief Infrastructure: implementasi ExportProtocol — tulis ranking ke file TXT (AES404: port impl).
 */

/* EXPORT — Infrastructure Adapter (TXT file writer) */
#include "module.export.h"
#include <stdio.h>
#include <string.h>

/* ──────────────────────────────────────────────
 * Tulis hasil ranking ke file teks (.txt) berformat
 * tabel: header + baris per peserta (rank, nama, skor)
 * + footer jumlah peserta.
 *
 * Nama diambil dari state->participants[pid].name
 * (bukan dari entries) agar selalu konsisten dgn state.
 * ────────────────────────────────────────────── */
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

    /* Header */
    fprintf(file, "========================================\n");
    fprintf(file, "  RANKING — C-AES Penalty Shootout\n");
    fprintf(file, "========================================\n\n");

    fprintf(file, "%-6s %-20s %s\n", "Rank", "Name", "Score");
    fprintf(file, "------ -------------------- -----\n");

    /* Baris per peserta. */
    for (int i = 0; i < count; i++) {
        int pid = entries[i].participant_id;
        const char *name = "Unknown";
        if (pid >= 0 && pid < state->participant_count) {
            name = state->participants[pid].name.value;
        }
        fprintf(file, "%-6d %-20s %d\n",
                entries[i].rank,
                name,
                entries[i].total_score);
    }

    /* Footer */
    fprintf(file, "\n========================================\n");
    fprintf(file, "  Total participants: %d\n", count);
    fprintf(file, "========================================\n");

    fclose(file);
    return EXP_OK;
}

/**
 * Bangun ExportProtocol dengan menyambungkan function-pointer
 * ke penulis file TXT di atas. Dipanggil oleh root container.
 */
ExportProtocol export_adapter_create(void) {
    ExportProtocol p;
    p.export_ranking = export_ranking_txt;
    return p;
}
