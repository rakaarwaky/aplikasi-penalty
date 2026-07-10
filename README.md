# C-AES Penalty Shootout

Aplikasi **console berbasis TUI** (Terminal User Interface) bahasa **C (C99)** untuk
mengelola lomba tendangan penalti. Dibangun dengan arsitektur **AES v3.0** (Agentic
Engineering System) menggunakan pendekatan *feature-based vertical slicing* dan
7 lapisan terstruktur.

## Fitur

- **Pendaftaran peserta** — 5–7 peserta, nama unik (case-insensitive), validasi karakter.
- **Input tendangan & skor** — setiap peserta 7 tendangan, zona 0–5 (zona == poin).
- **Ranking** — urut skor tertinggi; *tie-breaker* frekuensi zona 5→1; peringkat seri (1,1,3…).
- **Pencarian peserta** — cari by nama, tampilkan detail tendangan & zona.
- **Rekapitulasi** — tabel lengkap ranking + frekuensi zona tiap peserta.

Alur mengikuti *state machine*: `INIT → REGISTERED (≥5 peserta) → COMPLETED (semua 7 tendangan)`.

## Cara Build & Run

```bash
make        # kompilasi -> ./penalty_shootout (build artifacts di folder build/)
make run    # build lalu jalankan
make clean  # hapus build/ dan binari
```

Prasyarat: GCC, `make`, dan library **ncurses** (untuk TUI).

## Struktur Proyek

Arah dependency (bottom-up):

```
taxonomy → contract → capabilities / infrastructure → agent → surface → root (wiring only)
```

```
src/
├── shared/                         ← taxonomy + contract lintas fitur (taxonomy_*, contract_*)
│   ├── taxonomy_game_constant.h    taxonomy_participant_vo.h   taxonomy_zone_vo.h
│   ├── taxonomy_kick_vo.h          taxonomy_participant_entity.h
│   ├── taxonomy_competition_state_vo.h  taxonomy_rank_vo.h
│   ├── taxonomy_search_result_vo.h taxonomy_competition_error.h
│   ├── contract_registration_*.h   contract_scoring_*.h
│   ├── contract_ranking_*.h        contract_search_*.h
│   └── contract_recap_*.h
├── registration/                   ← fitur pendaftaran (module + 4 .c)
├── scoring/                        ← fitur pencatatan & skor
├── ranking/                        ← fitur ranking + tie-breaker
├── search/                         ← fitur pencarian peserta
├── recap/                          ← fitur rekapitulasi
├── tui/                            ← infrastructure_tui_adapter (ncurses)
├── cli/                            ← surfaces_*: delegasi ke agent (I/O user)
└── root_cli_main_entry.c           ← titik masuk (wiring semua container)
```

> Folder fitur hanya punya **satu header** (`module.<fitur>.h`) sebagai barrel deklarasi,
> namun boleh banyak file `.c` (capabilities, agent, root).

## Konvensi Kunci

- **State** disimpan di `CompetitionState` (di-alloc di `root_cli_main_entry.c`, di-pass via pointer). Tanpa variabel global.
- **Pemisahan I/O**: logic murni di `capabilities`/`agent`; **semua I/O terminal ada di `cli/surfaces_*`** (layer TUI adapter di `tui/`).
- **Kontrak di-shared**: fitur meminjam kontrak fitur lain lewat `shared/contract_*` (bukan implementasi), sehingga tak ada dependensi ke atas.
- **Header guard**: `#ifndef ..._H` pada tiap `.h`.
- **Error**: enum per fitur di `shared/taxonomy_competition_error.h`; alur `capabilities → agent → surface`.

## Aturan AES

Kode mengikuti aturan AES (detail di `RULES_AES.md`):

| Kelompok | Kode Aturan    | Topik                                                          |
| -------- | -------------- | ------------------------------------------------------------- |
| Naming   | AES101, AES102 | Nama file `prefix_concept_suffix`, suffix per layer            |
| Import   | AES201–AES205  | Batas dependency, larangan circular import                    |
| Quality  | AES301–AES305  | Batas ukuran file, mandatory definition, bypass comment       |
| Role     | AES401–AES406  | Peran tiap layer (taxonomy, contract, capabilities, dst.)      |
| Orphan   | AES501–AES506  | Deteksi kode tak terhubung                                    |

## Dokumentasi

| Dokumen           | Isi                                                                |
| ----------------- | ------------------------------------------------------------------ |
| `PRD.md`          | Requirement, aturan bisnis, state machine, library diizinkan       |
| `ARCHITECTURE.md` | Definisi layer, naming, diagram dependency                        |
| `RULES_AES.md`    | Seluruh kode aturan AES + severity + cara perbaikan               |
| `AGENTS.md`       | Ringkasan navigasi untuk developer/AI agent                        |
```
