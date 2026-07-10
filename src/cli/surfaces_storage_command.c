/**
 * @file surfaces_storage_command.c
 * @brief Smart surface: layar penyimpanan (Simpan / Muat / Hapus File / Reset).
 */

#include "cli/module.cli.h"
#include "storage/module.storage.h"

#include <stdio.h>

/* Cek apakah file data tersimpan ada di disk. */
static int file_exists(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) return 0;
    fclose(f);
    return 1;
}

/* Konversi kode error penyimpanan jadi teks pesan. */
static const char *storage_err_text(StorageError e) {
    switch (e) {
        case ST_OK:                return "Berhasil.";
        case ST_ERROR_FILE_NOT_FOUND: return "File tidak ditemukan.";
        case ST_ERROR_PERMISSION:  return "Gagal (izin/akses).";
        case ST_ERROR_CORRUPT:     return "File rusak/tidak valid.";
        case ST_ERROR_FULL:        return "Penyimpanan penuh.";
        default:                   return "Galat tidak dikenal.";
    }
}
/**
 * Jalankan layar penyimpanan. Kembali bila user menekan ESC / [0].
 */
void cli_surfaces_storage_execute(StorageAggregate *agg,
                                  CompetitionState *state,
                                  DisplayPort *dp) {
    int selected = 1;
    char msg[80] = "";
    int msg_is_error = 0;
    int running = 1;

    while (running) {
        int exists = file_exists(STORAGE_DEFAULT_FILE);
        storage_page_draw(dp, selected, exists, msg, msg_is_error);

        int key = dp->readkey();
        switch (key) {
            case TUI_KEY_UP:
                selected--;
                if (selected < 1) selected = 4;
                break;
            case TUI_KEY_DOWN:
                selected++;
                if (selected > 4) selected = 1;
                break;
            case TUI_KEY_ESC:
            case '0':
                running = 0;
                break;
            case TUI_KEY_ENTER:
                msg[0] = '\0';
                msg_is_error = 0;
                if (selected == 1) {
                    /* Simpan */
                    StorageError e = agent_storage_save(agg, STORAGE_DEFAULT_FILE, state);
                    if (e == ST_OK) {
                        snprintf(msg, sizeof msg, "[OK] Tersimpan ke %s", STORAGE_DEFAULT_FILE);
                    } else {
                        msg_is_error = 1;
                        snprintf(msg, sizeof msg, "[GAGAL] %s", storage_err_text(e));
                    }
                } else if (selected == 2) {
                    /* Muat */
                    StorageError e = agent_storage_load(agg, STORAGE_DEFAULT_FILE, state);
                    if (e == ST_OK) {
                        snprintf(msg, sizeof msg, "[OK] Dimuat dari %s", STORAGE_DEFAULT_FILE);
                    } else {
                        msg_is_error = 1;
                        snprintf(msg, sizeof msg, "[GAGAL] %s", storage_err_text(e));
                    }
                } else if (selected == 3) {
                    /* Hapus file tersimpan */
                    if (!exists) {
                        msg_is_error = 1;
                        snprintf(msg, sizeof msg, "[GAGAL] File tidak ada.");
                    } else if (dp->confirm("Hapus file data tersimpan?")) {
                        StorageError e = agent_storage_delete(agg, STORAGE_DEFAULT_FILE);
                        if (e == ST_OK) {
                            snprintf(msg, sizeof msg, "[OK] File dihapus.");
                        } else {
                            msg_is_error = 1;
                            snprintf(msg, sizeof msg, "[GAGAL] %s", storage_err_text(e));
                        }
                    }
                } else if (selected == 4) {
                    /* Reset lomba (bersihkan memory) */
                    if (dp->confirm("Reset lomba? Semua peserta & skor hilang.")) {
                        *state = (CompetitionState){0};
                        snprintf(msg, sizeof msg, "[OK] Lomba direset.");
                    }
                }
                break;
            default:
                break;
        }
    }
}
