/**
 * @file surfaces_storage_command.c
 * @brief Smart surface: navigasi & aksi penyimpanan data lomba.
 */

/* CLI — Smart Surface (Command) */
#include "cli/module.cli.h"
#include "storage/module.storage.h"
#include "shared/taxonomy_display_constant.h"
#include "shared/taxonomy_game_constant.h"

#include <stdio.h>

/* Cek ada/tidaknya file tersimpan (tanpa membukanya lewat storage API). */
static int file_exists(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) return 0;
    fclose(f);
    return 1;
}

/* Tampilkan pesan hasil aksi lalu tunggu ENTER. */
static void show_result(DisplayPort *dp, int selected, int show_file,
                        const char *msg, int color) {
    storage_page_draw(dp, selected, show_file, msg, color);
    dp->readkey();
}

void cli_surfaces_storage_execute(StorageAggregate *agg,
                                  CompetitionState *state,
                                  DisplayPort *dp) {
    int selected = 1;
    int running = 1;

    while (running) {
        int show_file = file_exists(DEFAULT_STORAGE_FILENAME);
        storage_page_draw(dp, selected, show_file, "", COLOR_DIM);

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
            case TUI_KEY_ENTER:
                if (selected == 1) {
                    /* Simpan */
                    StorageError r = agent_storage_save(agg, DEFAULT_STORAGE_FILENAME, state);
                    if (r == ST_OK)
                        show_result(dp, selected, 1, "[OK] Data lomba tersimpan.", COLOR_SUCCESS);
                    else
                        show_result(dp, selected, show_file,
                                    "[GAGAL] Tidak bisa menyimpan data lomba.", COLOR_ERROR);
                } else if (selected == 2) {
                    /* Muat */
                    if (!show_file) {
                        show_result(dp, selected, 0,
                                    "[INFO] Belum ada file tersimpan.", COLOR_WARNING);
                        break;
                    }
                    if (!dp->confirm("Muat akan MENGGANTI data lomba saat ini. Lanjut?"))
                        break;
                    StorageError r = agent_storage_load(agg, DEFAULT_STORAGE_FILENAME, state);
                    if (r == ST_OK)
                        show_result(dp, selected, 1, "[OK] Data lomba dimuat.", COLOR_SUCCESS);
                    else
                        show_result(dp, selected, show_file,
                                    "[GAGAL] File rusak atau bukan data lomba.", COLOR_ERROR);
                } else if (selected == 3) {
                    /* Hapus file */
                    if (!show_file) {
                        show_result(dp, selected, 0,
                                    "[INFO] Tidak ada file yang bisa dihapus.", COLOR_WARNING);
                        break;
                    }
                    if (!dp->confirm("Hapus file data tersimpan? Tidak bisa dibatalkan."))
                        break;
                    StorageError r = agent_storage_delete(agg, DEFAULT_STORAGE_FILENAME);
                    if (r == ST_OK)
                        show_result(dp, selected, 0, "[OK] File tersimpan dihapus.", COLOR_SUCCESS);
                    else
                        show_result(dp, selected, show_file,
                                    "[GAGAL] Tidak bisa menghapus file.", COLOR_ERROR);
                } else if (selected == 4) {
                    /* Reset lomba (bersihkan memori) */
                    if (!dp->confirm("Reset akan menghapus SEMUA peserta & skor dari memori. Lanjut?"))
                        break;
                    CompetitionState reset = {0};
                    *state = reset;
                    show_result(dp, selected, show_file,
                                "[OK] Lomba di-reset. Mulai dari pendaftaran.", COLOR_SUCCESS);
                }
                break;
            case '0':
            case 'q':
            case 'Q':
            case TUI_KEY_ESC:
                running = 0;
                break;
            default:
                break;
        }
    }
}
