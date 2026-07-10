# Feature: `cli` (v0.1.0)

Dokumen ini berisi source code fitur `cli` beserta definisi shared yang diimport oleh feature tersebut.

Arsitektur: **AES C99** — layer prefix: `taxonomy_` -> `contract_` -> `capabilities_` / `infrastructure_` -> `agent_` -> `surfaces_` -> `root_`

## Problem Statement

Isu berikut terdeteksi oleh `lint-arwaky-cli scan`:

```
============================================================
  AES Architecture Compliance Report
============================================================
  Project: /home/raka/Study/PengantarCoding/aplikasi-penalty/src/cli
  Violations: 0
Warning: No AES-compliant workspace members (crates/, packages/, or modules/) found in '/home/raka/Study/PengantarCoding/aplikasi-penalty/src/cli'. This system mandates a multi-module structure. Please refactor your project.
[warn] Unknown language 'unknown', using empty default config.
```

---

## Daftar File

### Feature: `src/cli/`

- [src/cli/module.cli.h](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/cli/module.cli.h) — layer: `module`
- [src/cli/surfaces_menu_command.c](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/cli/surfaces_menu_command.c) — layer: `surfaces`
- [src/cli/surfaces_ranking_command.c](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/cli/surfaces_ranking_command.c) — layer: `surfaces`
- [src/cli/surfaces_recap_command.c](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/cli/surfaces_recap_command.c) — layer: `surfaces`
- [src/cli/surfaces_registration_command.c](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/cli/surfaces_registration_command.c) — layer: `surfaces`
- [src/cli/surfaces_scoring_command.c](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/cli/surfaces_scoring_command.c) — layer: `surfaces`
- [src/cli/surfaces_search_command.c](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/cli/surfaces_search_command.c) — layer: `surfaces`

### Shared (`src/shared/`)

- [src/shared/contract_display_port.h](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/shared/contract_display_port.h) — layer: `contract`
- [src/shared/contract_sanitize_aggregate.h](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/shared/contract_sanitize_aggregate.h) — layer: `contract`
- [src/shared/contract_sanitize_protocol.h](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/shared/contract_sanitize_protocol.h) — layer: `contract`
- [src/shared/taxonomy_display_constant.h](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/shared/taxonomy_display_constant.h) — layer: `taxonomy`
- [src/shared/taxonomy_sanitize_error.h](file:///home/raka/Study/PengantarCoding/aplikasi-penalty/src/shared/taxonomy_sanitize_error.h) — layer: `taxonomy`

---

## `src/cli/module.cli.h`

> **Layer AES:** `module`

```c
/**
 * @file module.cli.h
 * @brief Kumpulan fungsi layar menu & tiap fitur (cari lewat satu file ini).
 */

#ifndef MODULE_CLI_H
#define MODULE_CLI_H

#include "shared/contract_display_port.h"
#include "shared/contract_sanitize_aggregate.h"
#include "shared/taxonomy_display_constant.h"

/* Fungsi tiap fitur (supaya layar tahu prototipenya). */
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"

/* Factory DisplayPort (wiring dari root layer). */
DisplayPort root_display_build(void);

/* Loop menu utama. */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          CompetitionState *state,
                          DisplayPort *dp,
                          SanitizeAggregate *sn);

/* Layar tiap fitur. */
void cli_surfaces_registration_execute(RegistrationAggregate *agg,
                                       CompetitionState *state, DisplayPort *dp,
                                       SanitizeAggregate *sn);
void cli_surfaces_scoring_execute(ScoringAggregate *agg,
                                  CompetitionState *state, DisplayPort *dp,
                                  SanitizeAggregate *sn);
void cli_surfaces_ranking_execute(RankingAggregate *agg,
                                  CompetitionState *state, DisplayPort *dp);
void cli_surfaces_search_execute(SearchAggregate *agg,
                                 CompetitionState *state, DisplayPort *dp);
void cli_surfaces_recap_execute(RecapAggregate *agg,
                                CompetitionState *state, DisplayPort *dp);

#endif /* MODULE_CLI_H */
```

---

## `src/cli/surfaces_menu_command.c`

> **Layer AES:** `surfaces`

```c
/**
 * @file surfaces_menu_command.c
 * @brief Layar menu utama: tampilkan daftar menu & alihkan ke fitur yang dipilih.
 */

#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>

#define MENU_ITEMS 6
#define BOX_WIDTH  60
#define BOX_START_ROW 3
#define BOX_START_COL 2

/* Label menu (indeks 0 = keluar, 1-5 = fitur). */
static const char *menu_labels[MENU_ITEMS] = {
    "Keluar",
    "Pendaftaran Peserta",
    "Input Tendangan dan Skor",
    "Tampilkan Ranking",
    "Cari Peserta",
    "Rekapitulasi Lengkap"
};

/**
 * Gambar ulang layar menu; 2 kolom: MENU (kiri) + STATUS (kanan).
 * participant_count ditampilkan di baris info.
 */
static void draw_menu(DisplayPort *dp, int selected, CompetitionStateKind state,
                      int participant_count) {
    char buf[128];
    dp->cls();

    /* ── Header: double-line border ── */
    dp->print_centered_colored(0, "==========================================================", COLOR_GOLD, 1);
    dp->print_centered_colored(1, "    APLIKASI PERHITUNGAN PENALTI    ", COLOR_TITLE, 1);
    dp->print_centered_colored(2, "==========================================================", COLOR_GOLD, 1);

    /* ── Bingkai utama (1 baris kosong dari header = Gestalt Proximity) ── */
    int gw = BOX_WIDTH;
    int grid_height = 14;
    dp->box(BOX_START_ROW, BOX_START_COL, gw, grid_height);
    dp->separator(BOX_START_ROW + 1, BOX_START_COL, gw);

    /* ── Layout 2 kolom ── */
    int col1_x = BOX_START_COL + 3;                          /* Kolom kiri: menu    */
    int col2_x = BOX_START_COL + gw - 16;            /* Kolom kanan: status */
    int col2_width = 14;

    /* Header kolom */
    snprintf(buf, sizeof buf, "%-*s", col2_x - col1_x, "MENU");
    dp->draw_colored(BOX_START_ROW + 2, col1_x, COLOR_HEADER, 1, buf);
    snprintf(buf, sizeof buf, "%-*s", col2_width, "STATUS");
    dp->draw_colored(BOX_START_ROW + 2, col2_x, COLOR_HEADER, 1, buf);
    dp->separator(BOX_START_ROW + 3, BOX_START_COL, gw);

    /* ── Baris menu 1-5 ── */
    int i;
    for (i = 1; i <= 5; i++) {
        int row = BOX_START_ROW + 3 + i;

        /* Kolom 1: nomor + nama menu (dengan padding) */
        int item_sel = (i == selected);
        int item_color = item_sel ? COLOR_HIGHLIGHT : COLOR_MENU;

        /* Highlight: gambar background full-width agar simetris (Grid System) */
        if (item_sel) {
            int j;
            for (j = 1; j < gw - 1; j++)
                dp->draw_at(row, BOX_START_COL + j, " ");
        }

        snprintf(buf, sizeof buf, "  [%d]  %s", i, menu_labels[i]);
        dp->draw_colored(row, col1_x, item_color, item_sel, buf);

        /* Kolom 2: status */
        const char *status = "";
        int status_color = COLOR_DIM;

        if (i == 1) {
            if (state == STATE_INIT)      { status = "[Aktif]";     status_color = COLOR_SUCCESS; }
            else                          { status = "[Selesai]";   status_color = COLOR_WARNING; }
        } else if (i == 2) {
            if (state == STATE_INIT)      { status = "[Terkunci]";  status_color = COLOR_DIM;     }
            else if (state == STATE_COMPLETED) { status = "[Selesai]"; status_color = COLOR_WARNING; }
            else                          { status = "[Aktif]";     status_color = COLOR_SUCCESS; }
        } else if (i == 3) {
            if (state != STATE_COMPLETED) { status = "[Terkunci]";  status_color = COLOR_DIM;     }
            else                          { status = "[Aktif]";     status_color = COLOR_SUCCESS; }
        } else if (i == 4) {
            if (state == STATE_INIT)      { status = "[Terkunci]";  status_color = COLOR_DIM;     }
            else                          { status = "[Aktif]";     status_color = COLOR_SUCCESS; }
        } else if (i == 5) {
            if (state != STATE_COMPLETED) { status = "[Terkunci]";  status_color = COLOR_DIM;     }
            else                          { status = "[Aktif]";     status_color = COLOR_SUCCESS; }
        }

        snprintf(buf, sizeof buf, "%-*s", col2_width, status);
        dp->draw_colored(row, col2_x, status_color, 0, buf);
    }

    /* Separator sebelum info */
    dp->separator(BOX_START_ROW + 9, BOX_START_COL, gw);

    /* ── Info status + peserta ── */
    const char *state_text = "";
    int state_color = COLOR_DIM;
    switch (state) {
        case STATE_INIT:       state_text = "Fase: Pendaftaran";     state_color = COLOR_INFO;    break;
        case STATE_REGISTERED: state_text = "Fase: Input Tendangan"; state_color = COLOR_WARNING; break;
        case STATE_COMPLETED:  state_text = "Fase: Selesai";         state_color = COLOR_SUCCESS; break;
    }
    dp->draw_colored(BOX_START_ROW + 10, BOX_START_COL + 2, state_color, 0, state_text);

    snprintf(buf, sizeof buf, "Peserta: %d/7", participant_count);
    dp->draw_colored(BOX_START_ROW + 10, BOX_START_COL + 24, COLOR_MENU, 0, buf);

    /* Tombol keluar (kanan bawah) */
    int exit_color = (0 == selected) ? COLOR_HIGHLIGHT : COLOR_ERROR;
    snprintf(buf, sizeof buf, "  [0] Keluar  ");
    dp->draw_colored(BOX_START_ROW + 10, BOX_START_COL + gw - 18, exit_color, 1, buf);

    /* ── Footer navigasi (ASCII-safe) ── */
    dp->footer("[Up/Down] Navigasi   [ENTER] Pilih   [1-5] Shortcut   [h] Bantuan");

    dp->screen_refresh();
}

/* C1: Layar bantuan "Cara Bermain" — tampilkan aturan & navigasi. */
static void show_help_screen(DisplayPort *dp) {
    char buf[128];
    dp->cls();

    dp->print_centered_colored(0, "==========================================================", COLOR_GOLD, 1);
    dp->print_centered_colored(1, "  PANDUAN PENGGUNAAN  ", COLOR_TITLE, 1);
    dp->print_centered_colored(2, "==========================================================", COLOR_GOLD, 1);

    int box_col = 2;
    int box_width = BOX_WIDTH;
    dp->box(3, box_col, box_width, 18);
    dp->separator(4, box_col, box_width);

    dp->draw_colored(5, box_col + 2, COLOR_GOLD, 1, "ATURAN LOMBA:");

    const char *rules[] = {
        "- Jumlah peserta : 5 sampai 7 orang",
        "- Setiap peserta : 7 tendangan penalti",
        "- Nilai zona     : 0=Miss  1=Mudah  2=Sedang",
        "                   3=Sulit 4=Sangat Sulit  5=Top!",
        "- Pemenang       : peserta dengan total skor tertinggi",
        NULL
    };
    int ri;
    for (ri = 0; rules[ri] != NULL; ri++) {
        dp->draw_colored(6 + ri, box_col + 4, COLOR_MENU, 0, rules[ri]);
    }

    dp->separator(11, box_col, box_width);

    dp->draw_colored(12, box_col + 2, COLOR_GOLD, 1, "NAVIGASI:");

    const char *navs[] = {
        "[Panah \xe2\x86\x91/\xe2\x86\x93]  : Pindah pilihan menu",
        "[1-5]        : Langsung pilih menu 1-5",
        "[ENTER]      : Konfirmasi pilihan",
        "[q / 0]      : Keluar dari aplikasi",
        "[h]          : Tampilkan layar bantuan ini",
        NULL
    };
    int ni;
    for (ni = 0; navs[ni] != NULL; ni++) {
        dp->draw_colored(13 + ni, box_col + 4, COLOR_MENU, 0, navs[ni]);
    }

    dp->separator(18, box_col, box_width);

    dp->footer("Tekan sembarang tombol untuk kembali ke menu");
    dp->screen_refresh();

    (void)buf; /* suppress unused warning */
    dp->readkey();
}

/**
 * Loop menu utama: baca tombol, pindah pilihan, lalu buka layar
 * fitur yang dipilih (hanya bila kondisi lomba mengizinkan).
 */
int cli_surfaces_menu_run(RegistrationAggregate *reg,
                          ScoringAggregate *sc, RankingAggregate *rk,
                          SearchAggregate *sr, RecapAggregate *rc,
                          CompetitionState *state,
                          DisplayPort *dp,
                          SanitizeAggregate *sn) {
    int selected = 1;
    int running = 1;

    /* Minimum terminal size */
    const int MIN_LINES = 18;
    const int MIN_COLS  = 64;

    while (running) {
        /* Cek ukuran terminal — tampilkan pesan bila terlalu kecil */
        int lines = dp->get_lines();
        int cols  = dp->get_cols();
        if (lines < MIN_LINES || cols < MIN_COLS) {
            dp->cls();
            dp->print_centered_colored(4, "TERMINAL TERLALU KECIL!", COLOR_ERROR, 1);
            char msg[64];
            snprintf(msg, sizeof msg, "Minimum: %dx%d  Saat ini: %dx%d",
                     MIN_COLS, MIN_LINES, cols, lines);
            dp->print_centered_colored(6, msg, COLOR_WARNING, 0);
            dp->print_centered_colored(8, "Perbesar jendela terminal, lalu tekan apa saja.",
                                       COLOR_INFO, 0);
            dp->screen_refresh();
            int key = dp->readkey();
            if (key == 'q' || key == 'Q' || key == TUI_KEY_ESC) running = 0;
            continue;  /* ulang cek ukuran */
        }

        draw_menu(dp, selected, state->state, state->participant_count);
        int key = dp->readkey();

        switch (key) {
            case TUI_KEY_UP:
                selected--;
                if (selected < 0) selected = MENU_ITEMS - 1;
                break;
            case TUI_KEY_DOWN:
                selected++;
                if (selected >= MENU_ITEMS) selected = 0;
                break;

            /* Resize: cukup loop ulang (draw_menu akan pakai LINES/COLS baru) */
            case TUI_KEY_RESIZE:
                break;

            /* C3: Shortcut angka langsung aktifkan menu (1-5). */
            case '1': selected = 1; goto do_enter;
            case '2': selected = 2; goto do_enter;
            case '3': selected = 3; goto do_enter;
            case '4': selected = 4; goto do_enter;
            case '5': selected = 5; goto do_enter;

            case TUI_KEY_ENTER:
            do_enter:
                if (selected == 0) {
                    if (dp->confirm("Yakin ingin keluar?"))
                        running = 0;
                } else if (selected == 1) {
                    if (state->state == STATE_INIT || state->state == STATE_REGISTERED)
                        cli_surfaces_registration_execute(reg, state, dp, sn);
                } else if (selected == 2) {
                    if (state->state == STATE_REGISTERED)
                        cli_surfaces_scoring_execute(sc, state, dp, sn);
                } else if (selected == 3) {
                    if (state->state == STATE_COMPLETED)
                        cli_surfaces_ranking_execute(rk, state, dp);
                } else if (selected == 4) {
                    if (state->state == STATE_REGISTERED || state->state == STATE_COMPLETED)
                        cli_surfaces_search_execute(sr, state, dp);
                } else if (selected == 5) {
                    if (state->state == STATE_COMPLETED)
                        cli_surfaces_recap_execute(rc, state, dp);
                }
                break;

            /* A2: Konfirmasi sebelum keluar via ESC. */
            case TUI_KEY_ESC:
                if (dp->confirm("Yakin ingin keluar?"))
                    running = 0;
                break;
            case '0':
                if (dp->confirm("Yakin ingin keluar?"))
                    running = 0;
                break;
            case 'q':
                if (dp->confirm("Yakin ingin keluar?"))
                    running = 0;
                break;

            /* C1: Tampilkan layar bantuan. */
            case 'h':
            case 'H':
                show_help_screen(dp);
                break;
        }
    }
    return 0;
}
```

---

## `src/cli/surfaces_ranking_command.c`

> **Layer AES:** `surfaces`

```c
/**
 * @file surfaces_ranking_command.c
 * @brief Layar peringkat: hitung & tampilkan urutan peserta.
 */

#include "cli/module.cli.h"

#include <stdio.h>

/**
 * Layar ranking: hitung peringkat lalu tampilkan tabel
 * (peringkat, nama, skor, frekuensi zona). Tolak bila lomba belum selesai.
 */
void cli_surfaces_ranking_execute(RankingAggregate *agg, CompetitionState *state,
                                  DisplayPort *dp) {
    char buf[128];
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Tendangan belum selesai.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    /* Hitung peringkat */
    RankingEntryVO entries[MAX_PARTICIPANTS];
    RankingError e = agent_ranking_compute(agg, state, entries, MAX_PARTICIPANTS);
    if (e != RK_OK) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Ranking belum siap.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    dp->cls();

    /* Breadcrumb */
    dp->print_centered_colored(0, "Menu Utama > Ranking Peserta", COLOR_DIM, 0);

    /* Judul dengan dekorasi */
    dp->print_centered_colored(1, "============================================", COLOR_GOLD, 1);
    dp->print_centered_colored(2, "RANKING PESERTA", COLOR_TITLE, 1);
    dp->print_centered_colored(3, "============================================", COLOR_GOLD, 1);

    /* Bingkai */
    int box_height = state->participant_count + 8 + 2; /* +2 untuk baris bar chart */
    dp->box(4, 2, 64, box_height);

    /* Separator header */
    dp->separator(5, 2, 64);

    /* Header kolom dengan warna */
    snprintf(buf, sizeof buf, " %-5s %-6s %-22s %-6s %s",
             "No", "Medali", "Nama", "Skor", "Zona(5|4|3|2|1|0)");
    dp->draw_colored(6, 4, COLOR_HEADER, 1, buf);

    /* Separator setelah header */
    dp->separator(7, 2, 64);

    /* Hitung skor maksimum untuk normalisasi bar chart (B1). */
    int max_score = 1;
    {
        int ms;
        for (ms = 0; ms < state->participant_count; ms++)
            if (entries[ms].total_score > max_score) max_score = entries[ms].total_score;
    }

    /* Satu baris per peserta */
    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &entries[i];
        const char *name = state->participants[r->participant_id].name.value;
        int row = 8 + i * 2; /* *2 karena tiap peserta 2 baris: teks + bar */

        /* Pilih warna berdasarkan peringkat */
        int row_color = COLOR_MENU;
        const char *medal = "";
        if (r->rank == 1) {
            row_color = COLOR_GOLD;
            medal = "  *1* ";
        } else if (r->rank == 2) {
            row_color = COLOR_SILVER;
            medal = "  *2* ";
        } else if (r->rank == 3) {
            row_color = COLOR_BRONZE;
            medal = "  *3* ";
        }

        /* Zebra stripe untuk baris selain juara */
        if (r->rank > 3 && i % 2 == 0)
            row_color = COLOR_DIM;

        snprintf(buf, sizeof buf, " %-5d %-6s %-22s %-6d ",
                 r->rank, medal, name, r->total_score);
        dp->draw_colored(row, 4, row_color, (r->rank <= 3), buf);

        /* B2: Cetak frekuensi zona dengan warna berbeda. */
        int z;
        for (z = 5; z >= 0; z--) {
            int zcolor;
            if (z == 5)      zcolor = COLOR_SUCCESS;
            else if (z == 0) zcolor = COLOR_ERROR;
            else             zcolor = COLOR_MENU;
            int zbold = (z == 5);
            snprintf(buf, sizeof buf, "%d", r->zone_freq[z]);
            dp->draw_append_colored(zcolor, zbold, buf);
            if (z > 0) dp->draw_append("|");
        }

        /* B1: Bar chart skor di baris kedua. */
        int bar_pct = (max_score > 0) ? (r->total_score * 100) / max_score : 0;
        dp->progress_bar(row + 1, 10, 30, bar_pct,
                         (r->rank == 1) ? COLOR_GOLD : COLOR_SUCCESS);

        /* Separator antar peserta */
        if (i < state->participant_count - 1) {
            dp->separator(row + 2, 2, 64);
        }
    }

    /* Footer */
    int footer_sep_row = 8 + state->participant_count * 2;
    dp->separator(footer_sep_row, 2, 64);

    /* Tampilkan juara jika ada */
    if (state->participant_count > 0) {
        const char *winner = state->participants[entries[0].participant_id].name.value;
        snprintf(buf, sizeof buf, "JUARA: %s dengan %d poin!", winner, entries[0].total_score);
        dp->draw_colored(footer_sep_row + 1, 4, COLOR_GOLD, 1, buf);

        /* Penjelasan tie-breaker untuk posisi 1 vs 2 */
        if (state->participant_count >= 2) {
            const RankingEntryVO *first  = &entries[0];
            const RankingEntryVO *second = &entries[1];
            if (first->total_score == second->total_score) {
                const char *name2 = state->participants[second->participant_id].name.value;
                snprintf(buf, sizeof buf, "Seri! %s & %s sama-sama %d poin.",
                         winner, name2, first->total_score);
                dp->draw_colored(footer_sep_row + 2, 4, COLOR_INFO, 0, buf);
                /* Cari zona penentu */
                int z;
                for (z = 5; z >= 0; z--) {
                    if (first->zone_freq[z] != second->zone_freq[z]) {
                        snprintf(buf, sizeof buf,
                                 "%s menang: zona %d lebih banyak (%d vs %d)",
                                 winner, z, first->zone_freq[z], second->zone_freq[z]);
                        dp->draw_colored(footer_sep_row + 3, 4, COLOR_INFO, 0, buf);
                        break;
                    }
                }
            }
        }
    }

    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}
```

---

## `src/cli/surfaces_recap_command.c`

> **Layer AES:** `surfaces`

```c
/**
 * @file surfaces_recap_command.c
 * @brief Layar rekap: tampilkan tabel lengkap seluruh peserta.
 */

#include "cli/module.cli.h"

#include <stdio.h>

/**
 * Layar rekapitulasi: siapkan data (peringkat + detail) lalu tampilkan
 * tabel per peserta. Tolak bila lomba belum selesai.
 *
 * @param exp_agg  Aggregate ekspor (disiapkan oleh root container, bukan di sini).
 */
void cli_surfaces_recap_execute(RecapAggregate *agg, CompetitionState *state,
                                DisplayPort *dp) {
    char buf[256];
    if (agg == NULL || state == NULL) return;
    if (state->state != STATE_COMPLETED) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Tendangan belum selesai.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    /* Siapkan data peringkat & detail */
    RankingEntryVO ranking[MAX_PARTICIPANTS];
    SearchResultVO details[MAX_PARTICIPANTS];
    RecapError e = agent_recap_prepare(agg, state, ranking, details, MAX_PARTICIPANTS);
    if (e != RC_OK) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Rekap belum siap.", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    dp->cls();

    /* Breadcrumb */
    dp->print_centered_colored(0, "Menu Utama > Rekapitulasi Lengkap", COLOR_DIM, 0);

    /* Judul dengan dekorasi */
    dp->print_centered_colored(1, "============================================", COLOR_GOLD, 1);
    dp->print_centered_colored(2, "REKAPITULASI LENGKAP", COLOR_TITLE, 1);
    dp->print_centered_colored(3, "============================================", COLOR_GOLD, 1);

    /* Dimensi & bingkai */
    int box_col = 1;
    int box_width = 66;
    int box_row = 4;
    int box_height = state->participant_count + 8;

    dp->box(box_row, box_col, box_width, box_height);

    /* Header kolom */
    dp->separator(box_row + 1, box_col, box_width);
    snprintf(buf, sizeof buf, " %-4s %-22s %-6s %s",
             "No", "Nama", "Skor", "Zona(0 1 2 3 4 5)");
    dp->draw_colored(box_row + 2, box_col + 2, COLOR_HEADER, 1, buf);
    dp->separator(box_row + 3, box_col, box_width);

    /* Satu baris per peserta */
    int i;
    for (i = 0; i < state->participant_count; i++) {
        const RankingEntryVO *r = &ranking[i];
        const SearchResultVO *d = &details[r->participant_id];
        int row = box_row + 4 + i;

        /* Pilih warna berdasarkan peringkat */
        int row_color = COLOR_MENU;
        if (r->rank == 1) row_color = COLOR_GOLD;
        else if (r->rank == 2) row_color = COLOR_SILVER;
        else if (r->rank == 3) row_color = COLOR_BRONZE;
        else if (i % 2 == 1) row_color = COLOR_DIM;

        snprintf(buf, sizeof buf, " %-4d %-22s %-6d ", r->rank, d->name, d->total_score);
        dp->draw_colored(row, box_col + 2, row_color, (r->rank <= 3), buf);

        /* B2: Frekuensi zona dengan warna — zona 5 hijau, zona 0 merah. */
        int z;
        for (z = 0; z <= MAX_ZONE; z++) {
            int zcolor;
            if (z == MAX_ZONE)  zcolor = COLOR_SUCCESS;
            else if (z == 0)    zcolor = COLOR_ERROR;
            else                zcolor = COLOR_MENU;
            int zbold = (z == MAX_ZONE);
            snprintf(buf, sizeof buf, "%d ", d->zone_freq[z]);
            dp->draw_append_colored(zcolor, zbold, buf);
        }

        /* Garis pemisah antar baris */
        if (i < state->participant_count - 1) {
            dp->separator(row + 1, box_col, box_width);
        }
    }

    /* Separator footer */
    dp->separator(box_row + 4 + state->participant_count, box_col, box_width);

    /* Hitung statistik */
    int total_score_all = 0;
    int highest_score   = 0;
    int i_stat;
    for (i_stat = 0; i_stat < state->participant_count; i_stat++) {
        total_score_all += ranking[i_stat].total_score;
        if (ranking[i_stat].total_score > highest_score)
            highest_score = ranking[i_stat].total_score;
    }
    int avg_score = (state->participant_count > 0)
                    ? total_score_all / state->participant_count : 0;

    /* Ringkasan statistik */
    snprintf(buf, sizeof buf,
             "Peserta: %d | Total Skor: %d | Rata-rata: %d | Tertinggi: %d",
             state->participant_count, total_score_all, avg_score, highest_score);
    dp->draw_colored(box_row + 5 + state->participant_count, box_col + 2,
                     COLOR_INFO, 0, buf);

    /* Tampilkan juara */
    if (state->participant_count > 0) {
        const char *winner = state->participants[ranking[0].participant_id].name.value;
        snprintf(buf, sizeof buf, "JUARA: %s dengan %d poin!", winner, ranking[0].total_score);
        dp->draw_colored(box_row + 6 + state->participant_count, box_col + 2,
                         COLOR_GOLD, 1, buf);
    }

    /* Penjelasan tie-breaker */
    if (state->participant_count >= 2) {
        const RankingEntryVO *first  = &ranking[0];
        const RankingEntryVO *second = &ranking[1];
        if (first->total_score == second->total_score) {
            const char *n1 = state->participants[first->participant_id].name.value;
            const char *n2 = state->participants[second->participant_id].name.value;
            snprintf(buf, sizeof buf, "Seri! %s & %s sama-sama %d poin.",
                     n1, n2, first->total_score);
            dp->draw_colored(box_row + 7 + state->participant_count, box_col + 2,
                             COLOR_INFO, 0, buf);
            int z;
            for (z = 5; z >= 0; z--) {
                if (first->zone_freq[z] != second->zone_freq[z]) {
                    snprintf(buf, sizeof buf,
                             "%s menang: zona %d lebih banyak (%d vs %d)",
                             n1, z, first->zone_freq[z], second->zone_freq[z]);
                    dp->draw_colored(box_row + 8 + state->participant_count, box_col + 2,
                                     COLOR_INFO, 0, buf);
                    break;
                }
            }
        }
    }

    dp->footer("[ENTER] Kembali");
    dp->screen_refresh();
    dp->readkey();
}
```

---

## `src/cli/surfaces_registration_command.c`

> **Layer AES:** `surfaces`

```c
/**
 * @file surfaces_registration_command.c
 * @brief Layar pendaftaran: baca nama peserta & simpan ke data lomba.
 */

#include "cli/module.cli.h"
#include "sanitizer/module.sanitizer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_WIDTH 56
#define BOX_ROW 3
#define BOX_COL 2

static size_t trim_spaces(char *str) {
    if (str == NULL) return 0;
    char *start = str;
    while (*start == ' ') start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    size_t len = strlen(str);
    while (len > 0 && str[len - 1] == ' ') str[--len] = '\0';
    return len;
}

static void show_error(DisplayPort *dp, const char *msg, int row) {
    char buf[128];
    snprintf(buf, sizeof buf, "  [!] %s", msg);
    dp->draw_colored(row, BOX_COL + 2, COLOR_ERROR, 1, buf);
}

static void draw_registration_screen(DisplayPort *dp, CompetitionState *state) {
    char buf[128];
    dp->cls();

    int count = state->participant_count;
    int box_height = count + 13;

    /* Breadcrumb */
    dp->print_centered_colored(0, "Menu Utama > Pendaftaran Peserta", COLOR_DIM, 0);
    dp->print_centered_colored(1, "PENDAFTARAN PESERTA", COLOR_TITLE, 1);
    dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, box_height);
    dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    int pct = (count * 100) / MAX_PARTICIPANTS;
    snprintf(buf, sizeof buf, "Kuota peserta: %d / %d", count, MAX_PARTICIPANTS);
    dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_MENU, 0, buf);
    dp->progress_bar(BOX_ROW + 3, BOX_COL + 2, 30, pct, COLOR_SUCCESS);

    /* C2: Label kuota permanen */
    snprintf(buf, sizeof buf, "Minimal 5, maksimal %d peserta", MAX_PARTICIPANTS);
    dp->draw_colored(BOX_ROW + 4, BOX_COL + 2, COLOR_WARNING, 0, buf);

    dp->draw_colored(BOX_ROW + 5, BOX_COL + 2, COLOR_DIM, 0,
                     "Ketik nama lalu Enter. Kosongkan untuk selesai.");
    dp->draw_colored(BOX_ROW + 6, BOX_COL + 2, COLOR_DIM, 0,
                     "Minimal 5, maksimal 7 peserta.");

    dp->separator(BOX_ROW + 7, BOX_COL, BOX_WIDTH);
    dp->draw_colored(BOX_ROW + 7, BOX_COL + 2, COLOR_WARNING, 1, "Peserta terdaftar:");

    int i;
    for (i = 0; i < count; i++) {
        snprintf(buf, sizeof buf, "%d. %s", i + 1, state->participants[i].name.value);
        dp->draw_colored(BOX_ROW + 8 + i, BOX_COL + 4, COLOR_SUCCESS, 0, buf);
    }

    dp->separator(BOX_ROW + 9 + count, BOX_COL, BOX_WIDTH);
    dp->screen_refresh();
}

void cli_surfaces_registration_execute(RegistrationAggregate *agg,
                                       CompetitionState *state, DisplayPort *dp,
                                       SanitizeAggregate *sn) {
    if (agg == NULL || state == NULL) return;
    char buf[128];

    draw_registration_screen(dp, state);

    int count = state->participant_count;
    int row = BOX_ROW + 10 + count;
    int error_row = BOX_ROW + count + 13 - 2;
    char buffer[64];

    while (state->participant_count < MAX_PARTICIPANTS) {
        count = state->participant_count;
        row = BOX_ROW + 10 + count;
        error_row = BOX_ROW + count + 13 - 2;

        snprintf(buf, sizeof buf, "Nama peserta #%d (contoh: \"Budi Santoso\"): ", count + 1);
        dp->draw_colored(row, BOX_COL + 2, COLOR_INFO, 1, buf);
        dp->screen_refresh();

        /* Input baca dari ncurses — harus melalui getnstr karena butuh posisi kursor */
        /* Ini adalah satu-satunya sisa interaksi langsung: echo/getnstr tidak ada di DisplayPort */
        /* karena ini bukan display melainkan input stream. */
        extern void tui_input_string(int row, int col, char *buf, int maxlen);
        tui_input_string(row, BOX_COL + 21, buffer, 30);

        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        trim_spaces(buffer);
        len = strlen(buffer);

        if (len == 0) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            show_error(dp, "Minimal 5 peserta untuk melanjutkan!", error_row);
            dp->screen_refresh();
            continue;
        }

        /* Validasi mentah via sanitizer agent sebelum domain agent */
        if (sn != NULL && agent_sanitize_validate_string(sn, buffer, MAX_NAME_LENGTH) != SANITIZE_OK) {
            show_error(dp, "Input nama tidak valid!", error_row);
            dp->screen_refresh();
            continue;
        }

        ParticipantNameVO name;
        strncpy(name.value, buffer, MAX_NAME_LENGTH);
        name.value[MAX_NAME_LENGTH] = '\0';

        RegistrationError e = agent_registration_add(agg, state, &name);
        if (e == REG_OK) {
            draw_registration_screen(dp, state);
            snprintf(buf, sizeof buf, "  [+] %s berhasil terdaftar!", name.value);
            dp->draw_colored(error_row, BOX_COL + 2, COLOR_SUCCESS, 1, buf);
            dp->screen_refresh();
        } else {
            const char *emsg = "Kesalahan pendaftaran!";
            switch (e) {
                case REG_NAME_EMPTY:        emsg = "Nama tidak boleh kosong!"; break;
                case REG_NAME_TOO_LONG:     emsg = "Nama maksimal 30 karakter!"; break;
                case REG_NAME_INVALID_CHAR: emsg = "Nama hanya boleh huruf dan spasi!"; break;
                case REG_NAME_DUPLICATE:    emsg = "Nama sudah terdaftar!"; break;
                case REG_FULL:              emsg = "Kuota peserta penuh!"; break;
                default: break;
            }
            show_error(dp, emsg, error_row);
            dp->screen_refresh();
        }
    }

    draw_registration_screen(dp, state);
    int final_count = state->participant_count;
    int final_error_row = BOX_ROW + final_count + 13 - 2;

    snprintf(buf, sizeof buf, "Total peserta: %d", final_count);
    dp->draw_colored(final_error_row, BOX_COL + 2, COLOR_SUCCESS, 1, buf);

    dp->footer("[ENTER] Lanjut  [q] Kembali");
    dp->screen_refresh();
    dp->readkey();
}
```

---

## `src/cli/surfaces_scoring_command.c`

> **Layer AES:** `surfaces`

```c
/**
 * @file surfaces_scoring_command.c
 * @brief Layar input tendangan: baca zona tiap peserta & catat skornya.
 */

#include "cli/module.cli.h"
#include "sanitizer/module.sanitizer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_WIDTH 58
#define BOX_HEIGHT 14
#define BOX_ROW 3
#define BOX_COL 2

/* Baca zona dari input pengguna; kembalikan SC_OK atau SC_INVALID_ZONE. */
static ScoringError read_zone(ZoneVO *out, char *raw_out, size_t raw_size) {
    extern void tui_input_string(int row, int col, char *buf, int maxlen);
    char buf[32];
    /* Posisi -1 berarti gunakan kursor saat ini */
    tui_input_string(-1, -1, buf, 10);

    if (raw_out != NULL && raw_size > 0)
        snprintf(raw_out, raw_size, "%s", buf);

    int i = 0;
    while (buf[i] != '\0' && buf[i] != '\n') {
        if (!isdigit((unsigned char)buf[i]) && buf[i] != '-') return SC_INVALID_ZONE;
        i++;
    }
    int z;
    if (sscanf(buf, "%d", &z) != 1) return SC_INVALID_ZONE;
    out->value = z;
    return SC_OK;
}

static void draw_scoring_screen(DisplayPort *dp, ParticipantEntity *part,
                                const char *msg, int msg_is_error) {
    char buf[128];
    /* A3: Hanya clear bila bukan pesan error, agar tidak flicker saat validasi zona. */
    if (!msg_is_error) dp->cls();

    /* Breadcrumb */
    dp->print_centered_colored(0, "Menu Utama > Input Tendangan", COLOR_DIM, 0);
    dp->print_centered_colored(1, "INPUT TENDANGAN DAN SKOR", COLOR_TITLE, 1);
    dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
    dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    snprintf(buf, sizeof buf, "Peserta: %s", part->name.value);
    dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_GOLD, 1, buf);

    int kick_pct = (part->kick_count.value * 100) / TOTAL_KICKS;
    snprintf(buf, sizeof buf, "Tendangan %d/%d: ", part->kick_count.value, TOTAL_KICKS);
    dp->draw_colored(BOX_ROW + 3, BOX_COL + 2, COLOR_MENU, 0, buf);
    dp->progress_bar(BOX_ROW + 3, BOX_COL + 18, 20, kick_pct, COLOR_SUCCESS);

    snprintf(buf, sizeof buf, "Skor sementara: %d poin", part->total_score.value);
    dp->draw_colored(BOX_ROW + 4, BOX_COL + 2, COLOR_WARNING, 1, buf);

    dp->separator(BOX_ROW + 5, BOX_COL, BOX_WIDTH);
    dp->draw_colored(BOX_ROW + 6, BOX_COL + 2, COLOR_MENU, 0, "Riwayat tendangan:");

    int k;
    for (k = 0; k < TOTAL_KICKS; k++) {
        int cx = BOX_COL + 4 + k * 4;
        int cy = BOX_ROW + 7;

        if (k < part->kick_count.value) {
            int zone = part->kicks[k].zone;
            int color = COLOR_DIM;
            if (zone >= 4) color = COLOR_SUCCESS;
            else if (zone >= 2) color = COLOR_WARNING;
            else if (zone == 0) color = COLOR_ERROR;
            snprintf(buf, sizeof buf, "Z%d ", zone);
            dp->draw_colored(cy, cx, color, 1, buf);
        } else if (k == part->kick_count.value) {
            dp->draw_colored(cy, cx, COLOR_INFO, 1, " -> ");
        } else {
            dp->draw_colored(cy, cx, COLOR_DIM, 0, " . ");
        }
    }

    dp->separator(BOX_ROW + 9, BOX_COL, BOX_WIDTH);
    dp->draw_colored(BOX_ROW + 10, BOX_COL + 2, COLOR_DIM, 0,
                     "Zona: 0=Miss 1=Mudah 2=Sedang 3=Sulit 4=SkSlt 5=Top");

    snprintf(buf, sizeof buf, "Masukkan zona (0-%d, contoh: 5): ", MAX_ZONE);
    dp->draw_colored(BOX_ROW + 11, BOX_COL + 2, COLOR_INFO, 1, buf);

    if (msg != NULL && msg[0] != '\0') {
        int msg_color = msg_is_error ? COLOR_ERROR : COLOR_SUCCESS;
        dp->draw_colored(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, msg_color, 1, msg);
    }

    dp->screen_refresh();
}

void cli_surfaces_scoring_execute(ScoringAggregate *agg, CompetitionState *state,
                                  DisplayPort *dp, SanitizeAggregate *sn) {
    char buf[128];
    if (agg == NULL || state == NULL) return;

    if (state->state == STATE_INIT) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Daftar peserta dulu (Menu 1).", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    if (state->state == STATE_COMPLETED) {
        dp->cls();
        dp->print_centered_colored(10, "[INFO] Semua peserta sudah selesai melakukan tendangan.",
                                   COLOR_SUCCESS, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    int p;
    for (p = 0; p < state->participant_count; p++) {
        ParticipantEntity *part = &state->participants[p];
        while (part->kick_count.value < TOTAL_KICKS) {
            draw_scoring_screen(dp, part, NULL, 0);

            ZoneVO z;
            char raw[32] = "";
            read_zone(&z, raw, sizeof raw);

            /* Validasi mentah via sanitizer agent sebelum domain agent */
            if (sn != NULL && raw[0] != '\0' &&
                agent_sanitize_validate_int(sn, raw, MIN_ZONE, MAX_ZONE) != SANITIZE_OK) {
                snprintf(buf, sizeof buf,
                         "[GAGAL] Input zona tidak valid: '%s'.", raw);
                dp->draw_colored(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, COLOR_ERROR, 1, buf);
                dp->screen_refresh();
                dp->readkey();
                continue;
            }

            if (z.value < MIN_ZONE || z.value > MAX_ZONE) {
                snprintf(buf, sizeof buf,
                         "[GAGAL] Zona harus %d-%d. Input: '%s'.", MIN_ZONE, MAX_ZONE, raw);
                dp->draw_colored(BOX_ROW + BOX_HEIGHT - 2, BOX_COL + 2, COLOR_ERROR, 1, buf);
                dp->screen_refresh();
                dp->readkey();
                continue;
            }

            ScoringError e = agent_scoring_record(agg, state, p, z);
            if (e == SC_OK) {
                snprintf(buf, sizeof buf, "[OK] Zona %d -> %d poin", z.value, z.value);
                draw_scoring_screen(dp, part, buf, 0);
            } else if (e == SC_INVALID_ZONE) {
                snprintf(buf, sizeof buf, "[GAGAL] Zona harus %d-%d. Anda memasukkan '%s'.",
                         MIN_ZONE, MAX_ZONE, raw);
                draw_scoring_screen(dp, part, buf, 1);
            } else {
                draw_scoring_screen(dp, part, "[GAGAL] Kesalahan pencatatan!", 1);
            }
            dp->screen_refresh();
            dp->readkey();
        }

        dp->cls();
        dp->print_centered_colored(4, "[SELESAI]", COLOR_SUCCESS, 1);
        dp->box(6, BOX_COL, BOX_WIDTH, 6);

        snprintf(buf, sizeof buf, "Peserta: %s", part->name.value);
        dp->draw_colored(8, BOX_COL + 4, COLOR_GOLD, 1, buf);

        snprintf(buf, sizeof buf, "Total Skor: %d poin", part->total_score.value);
        dp->draw_colored(9, BOX_COL + 4, COLOR_SUCCESS, 1, buf);

        dp->footer("[ENTER] Lanjut");
        dp->screen_refresh();
        dp->readkey();
    }

    dp->cls();
    if (state->state == STATE_COMPLETED) {
        dp->print_centered_colored(4, "* * * * * * * * * * * * * * * *", COLOR_GOLD, 1);
        dp->print_centered_colored(5, "SEMUA TENDANGAN SELESAI!", COLOR_SUCCESS, 1);
        dp->print_centered_colored(6, "* * * * * * * * * * * * * * * *", COLOR_GOLD, 1);
    }
    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}
```

---

## `src/cli/surfaces_search_command.c`

> **Layer AES:** `surfaces`

```c
/**
 * @file surfaces_search_command.c
 * @brief Layar cari: baca nama & tampilkan data peserta yang cocok.
 */

#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOX_WIDTH 58
#define BOX_HEIGHT 16
#define BOX_ROW 3
#define BOX_COL 2

/**
 * Layar pencarian: baca nama, cari di data, tampilkan detail peserta
 * atau pesan "tidak ditemukan". Tolak bila belum ada peserta terdaftar.
 */
void cli_surfaces_search_execute(SearchAggregate *agg, CompetitionState *state,
                                 DisplayPort *dp) {
    char buf[128];
    if (agg == NULL || state == NULL) return;
    if (state->state == STATE_INIT) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Daftar peserta dulu (Menu 1).", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    dp->cls();

    /* Breadcrumb */
    dp->print_centered_colored(0, "Menu Utama > Cari Peserta", COLOR_DIM, 0);
    dp->print_centered_colored(1, "CARI PESERTA", COLOR_TITLE, 1);

    /* Bingkai */
    dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
    dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

    /* Instruksi */
    dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_MENU, 0,
                     "Masukkan nama peserta yang dicari:");

    /* Input field label */
    dp->draw_colored(BOX_ROW + 4, BOX_COL + 2, COLOR_INFO, 0, "Nama: ");
    dp->screen_refresh();

    /* Baca nama */
    char buffer[64];
    extern void tui_input_string(int row, int col, char *buf, int maxlen);
    tui_input_string(BOX_ROW + 4, BOX_COL + 8, buffer, 30);

    /* Buang newline */
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
        buffer[--len] = '\0';

    /* Nama kosong */
    if (len == 0) {
        dp->cls();
        dp->print_centered_colored(10, "[GAGAL] Nama pencarian kosong!", COLOR_ERROR, 1);
        dp->screen_refresh();
        dp->readkey();
        return;
    }

    /* Cari peserta */
    ParticipantNameVO name;
    strncpy(name.value, buffer, MAX_NAME_LENGTH);
    name.value[MAX_NAME_LENGTH] = '\0';

    SearchResultVO r;
    SearchError e = agent_search_find(agg, state, &name, &r);

    /* Tampilkan hasil */
    dp->cls();

    if (e == SR_OK) {
        dp->print_centered_colored(1, "HASIL PENCARIAN", COLOR_TITLE, 1);
        dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
        dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

        /* Status ditemukan */
        dp->draw_colored(BOX_ROW + 2, BOX_COL + 2, COLOR_SUCCESS, 1,
                         "[DITEMUKAN] Peserta ditemukan!");

        dp->separator(BOX_ROW + 3, BOX_COL, BOX_WIDTH);

        /* Detail peserta */
        snprintf(buf, sizeof buf, "Nama        : %s", r.name);
        dp->draw_colored(BOX_ROW + 4, BOX_COL + 4, COLOR_GOLD, 1, buf);

        snprintf(buf, sizeof buf, "Total Skor  : %d poin", r.total_score);
        dp->draw_colored(BOX_ROW + 5, BOX_COL + 4, COLOR_SUCCESS, 1, buf);

        dp->separator(BOX_ROW + 6, BOX_COL, BOX_WIDTH);

        /* Riwayat tendangan */
        dp->draw_colored(BOX_ROW + 7, BOX_COL + 2, COLOR_MENU, 0, "Riwayat Tendangan:");

        int k;
        for (k = 0; k < TOTAL_KICKS; k++) {
            int cx = BOX_COL + 4 + k * 5;
            int cy = BOX_ROW + 8;

            if (r.kicks[k] == -1) {
                dp->draw_colored(cy, cx, COLOR_DIM, 0, " - ");
            } else {
                int zone = r.kicks[k];
                int color = COLOR_DIM;
                if (zone >= 4) color = COLOR_SUCCESS;
                else if (zone >= 2) color = COLOR_WARNING;
                else if (zone == 0) color = COLOR_ERROR;
                snprintf(buf, sizeof buf, "Z%d ", zone);
                dp->draw_colored(cy, cx, color, 1, buf);
            }
        }

        dp->separator(BOX_ROW + 10, BOX_COL, BOX_WIDTH);

        /* Frekuensi zona */
        dp->draw_colored(BOX_ROW + 11, BOX_COL + 2, COLOR_MENU, 0, "Frekuensi Zona:");

        int z;
        for (z = 0; z <= MAX_ZONE; z++) {
            int cx = BOX_COL + 4 + z * 7;
            int cy = BOX_ROW + 12;
            snprintf(buf, sizeof buf, "Z%d:%d", z, r.zone_freq[z]);
            dp->draw_colored(cy, cx, COLOR_MENU, 0, buf);
        }
    } else {
        dp->print_centered_colored(1, "HASIL PENCARIAN", COLOR_TITLE, 1);
        dp->box(BOX_ROW, BOX_COL, BOX_WIDTH, BOX_HEIGHT);
        dp->separator(BOX_ROW + 1, BOX_COL, BOX_WIDTH);

        dp->draw_colored(BOX_ROW + 4, BOX_COL + 4, COLOR_ERROR, 1, "[TIDAK DITEMUKAN]");

        snprintf(buf, sizeof buf, "Peserta '%s' tidak ditemukan.", buffer);
        dp->draw_colored(BOX_ROW + 6, BOX_COL + 4, COLOR_MENU, 0, buf);

        /* Fuzzy search: cari nama yang mirip */
        dp->draw_colored(BOX_ROW + 8, BOX_COL + 4, COLOR_INFO, 0, "Mungkin maksud Anda:");

        int suggestions = 0;
        int si;
        for (si = 0; si < state->participant_count && suggestions < 3; si++) {
            const char *pname = state->participants[si].name.value;
            int match = 0;
            size_t pi;
            for (pi = 0; pname[pi] != '\0' && !match; pi++) {
                char lower_p = (char)tolower((unsigned char)pname[pi]);
                size_t bi;
                for (bi = 0; buffer[bi] != '\0' && !match; bi++) {
                    if (tolower((unsigned char)buffer[bi]) == lower_p)
                        match = 1;
                }
            }
            if (match) {
                snprintf(buf, sizeof buf, "- %s", pname);
                dp->draw_colored(BOX_ROW + 9 + suggestions, BOX_COL + 6,
                                 COLOR_SUCCESS, 0, buf);
                suggestions++;
            }
        }
        if (suggestions == 0) {
            dp->draw_colored(BOX_ROW + 9, BOX_COL + 6, COLOR_DIM, 0,
                             "(tidak ada saran serupa)");
        }
    }

    dp->footer("[ENTER] Kembali ke menu");
    dp->screen_refresh();
    dp->readkey();
}
```

---

## `src/shared/contract_display_port.h`

> **Layer AES:** `contract`

```c
/**
 * @file contract_display_port.h
 * @brief Interface display: function pointers untuk rendering UI.
 */

#ifndef SHARED_CONTRACT_DISPLAY_PORT_H
#define SHARED_CONTRACT_DISPLAY_PORT_H

#include "shared/taxonomy_display_constant.h"

/**
 * DisplayPort — interface untuk semua operasi rendering.
 * Surfaces memanggil melalui struct ini, bukan langsung ke ncurses.
 */
typedef struct {
    /* Drawing primitives */
    void (*cls)(void);
    void (*screen_refresh)(void);
    void (*draw_at)(int row, int col, const char *text);
    void (*draw_colored)(int row, int col, int color, int bold, const char *text);
    void (*draw_append)(const char *text);
    void (*draw_append_colored)(int color, int bold, const char *text);

    /* Higher-level drawing */
    void (*print_centered_colored)(int row, const char *text, int color, int bold);
    void (*print_colored)(int row, int col, const char *text, int color);
    void (*box)(int row, int col, int width, int height);
    void (*box_double)(int row, int col, int width, int height);
    void (*separator)(int row, int col, int width);
    void (*separator_thick)(int row, int col, int width);
    void (*highlight_row)(int row, int col, int width, const char *text);
    void (*normal_row)(int row, int col, int width, const char *text);
    void (*footer)(const char *help);
    void (*progress_bar)(int row, int col, int width, int percent, int color);
    const char *(*medal)(int rank);

    /* Input */
    int  (*readkey)(void);
    int  (*confirm)(const char *prompt);
    void (*input_string)(int row, int col, char *buf, int maxlen);

    /* Terminal info */
    int  (*get_lines)(void);
    int  (*get_cols)(void);

} DisplayPort;

#endif /* SHARED_CONTRACT_DISPLAY_PORT_H */
```

---

## `src/shared/contract_sanitize_aggregate.h`

> **Layer AES:** `contract`

```c
/**
 * @file contract_sanitize_aggregate.h
 * @brief Wadah SanitizeProtocol yang dipakai seluruh program.
 */

#ifndef SHARED_CONTRACT_SANITIZE_AGGREGATE_H
#define SHARED_CONTRACT_SANITIZE_AGGREGATE_H

#include "shared/contract_sanitize_protocol.h"
#include "shared/taxonomy_sanitize_error.h"

/** Penghubung ke fungsi sanitasi (disimpan alamatnya saja). */
typedef struct {
    SanitizeProtocol *protocol; /**< Pointer ke fungsi-fungsi sanitasi. */
} SanitizeAggregate;

#endif /* SHARED_CONTRACT_SANITIZE_AGGREGATE_H */
```

---

## `src/shared/contract_sanitize_protocol.h`

> **Layer AES:** `contract`

```c
/**
 * @file contract_sanitize_protocol.h
 * @brief Daftar fungsi yang harus ada untuk periksa input pengguna.
 */

#ifndef CONTRACT_SANITIZE_PROTOCOL_H
#define CONTRACT_SANITIZE_PROTOCOL_H

#include "shared/taxonomy_sanitize_error.h"
#include <stddef.h>

/* Tipe fungsi: periksa string input (panjang & karakter). */
typedef SanitizeError (*validate_string_fn)(const char *input, size_t max_length);

/* Tipe fungsi: periksa string bisa dijadikan angka dalam rentang. */
typedef SanitizeError (*validate_int_fn)(const char *input, int min_val, int max_val);

/** Kumpulan fungsi sanitasi — diisi oleh root container. */
typedef struct {
    validate_string_fn validate_string;  /**< Cek string. */
    validate_int_fn validate_int;        /**< Cek angka. */
} SanitizeProtocol;

#endif /* CONTRACT_SANITIZE_PROTOCOL_H */
```

---

## `src/shared/taxonomy_display_constant.h`

> **Layer AES:** `taxonomy`

```c
/**
 * @file taxonomy_display_constant.h
 * @brief Konstanta display: warna, tombol, dan batas UI.
 */

#ifndef SHARED_TAXONOMY_DISPLAY_CONSTANT_H
#define SHARED_TAXONOMY_DISPLAY_CONSTANT_H

/* ── Warna (nomor pasangan ncurses; 0 = default) ── */
#define COLOR_DEFAULT   0
#define COLOR_TITLE     1
#define COLOR_MENU      2
#define COLOR_HIGHLIGHT 3
#define COLOR_SUCCESS   4
#define COLOR_ERROR     5
#define COLOR_BORDER    6
#define COLOR_WARN      7
#define COLOR_DIM       8
#define COLOR_GOLD      9
#define COLOR_SILVER    10
#define COLOR_BRONZE    11
#define COLOR_WARNING   12
#define COLOR_INFO      13
#define COLOR_HEADER    14

/* ── Kode tombol (nilai literal, tanpa dependensi ncurses) ── */
#define TUI_KEY_UP      259
#define TUI_KEY_DOWN    258
#define TUI_KEY_ENTER   10
#define TUI_KEY_ESC     27
#define TUI_KEY_RESIZE  410

/* ── Batas UI ── */
#define TUI_BAR_WIDTH   26

#endif /* SHARED_TAXONOMY_DISPLAY_CONSTANT_H */
```

---

## `src/shared/taxonomy_sanitize_error.h`

> **Layer AES:** `taxonomy`

```c
/**
 * @file taxonomy_sanitize_error.h
 * @brief Kode error pemeriksaan input teks/angka dari pengguna.
 */

#ifndef TAXONOMY_SANITIZE_ERROR_H
#define TAXONOMY_SANITIZE_ERROR_H

/** Hasil validasi input pengguna. */
typedef enum {
    SANITIZE_OK,                   /**< Input sah. */
    SANITIZE_ERROR_NULL_INPUT,     /**< Input kosong/null. */
    SANITIZE_ERROR_TOO_LONG,       /**< Melebihi panjang/rentang. */
    SANITIZE_ERROR_INVALID_CHARS   /**< Ada karakter tak sah. */
} SanitizeError;

#endif /* TAXONOMY_SANITIZE_ERROR_H */
```

---

