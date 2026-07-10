/**
 * @file infrastructure_export_adapter.c
 * @brief Tulis hasil peringkat ke file teks berformat tabel.
 */

/* EXPORT — Infrastructure Adapter (TXT file writer) */
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
        return EXP_ERROR_EMPTY_DATA; /* tidak ada data untuk diekspor */
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return EXP_ERROR_PERMISSION; /* tidak bisa buka file */
    }

    /* Judul. */
    fprintf(file, "========================================\n");
    fprintf(file, "  RANKING — C-AES Penalty Shootout\n");
    fprintf(file, "========================================\n\n");

    fprintf(file, "%-6s %-20s %s\n", "Rank", "Name", "Score");
    fprintf(file, "------ -------------------- -----\n");

    /* Satu baris per peserta. */
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

    /* Jumlah peserta. */
    fprintf(file, "\n========================================\n");
    fprintf(file, "  Total participants: %d\n", count);
    fprintf(file, "========================================\n");

    fclose(file);
    return EXP_OK;
}

/**
 * Siapkan struct ekspor: sambungkan fungsi tulis file teks.
 */
ExportProtocol export_adapter_create(void) {
    ExportProtocol p;
    p.export_ranking = export_ranking_txt;
    return p;
}
