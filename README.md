# C-AES Penalty Shootout

Aplikasi **console berbasis TUI** (Terminal User Interface) bahasa **C (C99)** untuk
mengelola lomba tendangan penalti. Dibangun dengan arsitektur **AES v3.0** (Agentic
Engineering System) menggunakan pendekatan *feature-based vertical slicing* dan
7 lapisan terstruktur.

## Fitur

- **Pendaftaran peserta** — 5-7 peserta, nama unik (case-insensitive), validasi karakter.
- **Input tendangan & skor** — setiap peserta 7 tendangan, zona 0-5 (zona == poin).
- **Ranking** — urut skor tertinggi; *tie-breaker* frekuensi zona 5 ke 1; peringkat seri (1,1,3...).
- **Pencarian peserta** — cari by nama, tampilkan detail tendangan & zona.
- **Rekapitulasi** — tabel lengkap ranking + frekuensi zona tiap peserta.
- **Penyimpanan data** — simpan/load state ke file binary.
- **Export TXT** — ekspor ranking ke file teks yang rapi.
- **Input sanitization** — validasi input string dan integer.
- **Logging** — sistem logging berlevel (DEBUG, INFO, WARN, ERROR, FATAL).

Alur mengikuti *state machine*: `INIT -> REGISTERED (>=5 peserta) -> COMPLETED (semua 7 tendangan)`.

## Cara Build & Run

```bash
make            # kompilasi
make run        # build lalu jalankan
make test       # jalankan 113 unit tests
make clean      # hapus build/ dan binari
```

### Makefile Targets

| Target | Deskripsi |
|--------|-----------|
| `make` | Build binary `aplikasi_perhitungan_penalty` |
| `make run` | Build lalu jalankan |
| `make test` | Jalankan 113 unit tests (registration, scoring, ranking, search, recap, storage, sanitizer, export, logger, integration) |
| `make lint` | Static analysis dengan cppcheck |
| `make format` | Format code dengan clang-format |
| `make analyze` | Static analysis dengan scan-build (Clang) |
| `make valgrind` | Memory leak check dengan valgrind |
| `make metrics` | Tampilkan code metrics |
| `make clean` | Hapus build artifacts |

Prasyarat: GCC, `make`, dan library **ncurses** (untuk TUI).

## Struktur Proyek

Arah dependency (bottom-up):

```
taxonomy -> contract -> capabilities / infrastructure -> agent -> surface -> root (wiring only)
```

```
src/
├── shared/                  <- taxonomy + contract lintas fitur
├── registration/            <- fitur pendaftaran
├── scoring/                 <- fitur pencatatan & skor
├── ranking/                 <- fitur ranking + tie-breaker
├── search/                  <- fitur pencarian peserta
├── recap/                   <- fitur rekapitulasi
├── storage/                 <- fitur penyimpanan data (persistence)
├── export/                  <- fitur ekspor ranking ke TXT
├── sanitizer/               <- fitur input sanitization
├── tui/                     <- infrastructure TUI adapter (ncurses)
├── cli/                     <- surfaces: CLI commands (I/O user)
└── root_cli_main_entry.c   <- titik masuk (wiring semua container)
```

## Testing

```bash
make test       # jalankan semua tests
make valgrind   # memory leak check
```

**113 unit tests** mencakup:
- 23 registration tests
- 20 scoring tests
- 16 ranking tests
- 19 search tests
- 13 recap tests
- 2 storage tests
- 8 sanitizer tests
- 4 integration tests (full pipeline)
- 3 export tests
- 6 logger tests

## Aturan AES

Kode mengikuti aturan AES (detail di `RULES_AES.md`):

| Kelompok | Kode Aturan    | Topik                                                   |
| -------- | -------------- | ------------------------------------------------------- |
| Naming   | AES101, AES102 | Nama file `prefix_concept_suffix`, suffix per layer     |
| Import   | AES201-AES205  | Batas dependency, larangan circular import              |
| Quality  | AES301-AES305  | Batas ukuran file, mandatory definition, bypass comment |
| Role     | AES401-AES406  | Peran tiap layer (taxonomy, contract, capabilities, dst.) |
| Orphan   | AES501-AES506  | Deteksi kode tak terhubung                              |

## Dokumentasi

| Dokumen           | Isi                                                          |
| ----------------- | ------------------------------------------------------------ |
| `PRD.md`          | Requirement, aturan bisnis, state machine, library diizinkan |
| `ARCHITECTURE.md` | Definisi layer, naming, diagram dependency                  |
| `RULES_AES.md`    | Seluruh kode aturan AES + severity + cara perbaikan         |
| `AGENTS.md`       | Ringkasan navigasi untuk developer/AI agent                 |
