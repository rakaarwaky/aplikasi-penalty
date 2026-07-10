# Aplikasi Perhitungan Penalti

Aplikasi **console berbasis TUI** (Terminal User Interface) bahasa **C (C99)** untuk
mengelola lomba tendangan penalti. Dibangun dengan arsitektur **AES v3.0** (Agentic
Engineering System) menggunakan pendekatan *feature-based vertical slicing* dan
7 lapisan terstruktur.

---

## Daftar Isi

- [Fitur](#fitur)
- [Cara Install Prasyarat](#cara-install-prasyarat)
- [Cara Build &amp; Run](#cara-build--run)
- [Cara Penggunaan](#cara-penggunaan)
- [Aturan Bisnis](#aturan-bisnis)
- [State Machine](#state-machine)
- [Struktur Proyek](#struktur-proyek)
- [Makefile Targets](#makefile-targets)
- [Testing](#testing)
- [Aturan AES](#aturan-aes)
- [Dokumentasi](#dokumentasi)
- [License](#license)

---

## Fitur

| Fitur                            | Deskripsi                                                              |
| -------------------------------- | ---------------------------------------------------------------------- |
| **Pendaftaran Peserta**    | 5-7 peserta, nama unik (case-insensitive), validasi karakter           |
| **Input Tendangan & Skor** | Setiap peserta 7 tendangan, zona 0-5 (zona == poin)                    |
| **Ranking**                | Urut skor tertinggi, tie-breaker frekuensi zona 5 ke 1, peringkat seri |
| **Pencarian Peserta**      | Cari by nama, tampilkan detail tendangan & zona                        |
| **Rekapitulasi**           | Tabel lengkap ranking + frekuensi zona tiap peserta                    |
| **Penyimpanan Data**       | Simpan/load state ke file binary                                       |
| **Export TXT**             | Ekspor ranking ke file teks yang rapi                                  |
| **Input Sanitization**     | Validasi input string dan integer                                      |
| **Logging**                | Sistem logging berlevel (DEBUG, INFO, WARN, ERROR, FATAL)              |

---

## Cara Install Prasyarat

### Linux (Fedora/RHEL)

```bash
# Compiler dan build tool
sudo dnf install -y gcc make

# Library TUI
sudo dnf install -y ncurses-devel

# Tools opsional (untuk development)
sudo dnf install -y cppcheck clang-analyzer valgrind clang-format
```

### Linux (Ubuntu/Debian)

```bash
# Compiler dan build tool
sudo apt-get install -y gcc make

# Library TUI
sudo apt-get install -y libncurses-dev

# Tools opsional (untuk development)
sudo apt-get install -y cppcheck clang valgrind clang-format
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install ncurses (biasanya sudah ada)
brew install ncurses
```

### Windows (Native)

```bash
# Install MinGW (MSYS2)
# Download dari https://msys2.org/, lalu install

# Buka MSYS2 MinGW 64-bit, jalankan:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make

# Install PDCurses (TUI library untuk Windows)
pacman -S mingw-w64-x86_64-pdcurses
```

Build dengan MinGW:

```bash
# Di MSYS2 MinGW 64-bit
make
make run
```

> **Catatan:** Kode sudah mendukung native Windows dengan `#ifdef _WIN32` untuk PDCurses.

### Windows (Cross-compile dari Linux)

```bash
# Install MinGW cross-compiler
sudo dnf install -y mingw64-gcc mingw64-pdcurses

# Build .exe dari Linux
make win
```

Hasil: `aplikasi_perhitungan_penalti.exe`

---

## Cara Build & Run

```bash
# Build
make

# Build lalu jalankan
make run

# Jalankan 113 unit tests
make test

# Hapus build artifacts
make clean
```

Binary yang dihasilkan: `aplikasi_perhitungan_penalti`

---

## Cara Penggunaan

### Menu Utama

Setelah menjalankan aplikasi, user akan melihat menu utama:

```
========================================
  APLIKASI PENALTI SHOOTOUT
========================================

1. Pendaftaran Peserta
2. Input Tendangan dan Skor
3. Tampilkan Ranking
4. Cari Peserta
5. Rekapitulasi Lengkap
0. Keluar

Pilihan:
```

### Alur Penggunaan

1. **Pendaftaran (Menu 1)**

   - Masukkan nama peserta satu per satu (5-7 orang)
   - Nama hanya boleh huruf dan spasi, maksimal 30 karakter
   - Nama tidak boleh duplikat (case-insensitive)
2. **Input Tendangan (Menu 2)**

   - Pilih peserta yang akan melakukan tendangan
   - Masukkan 7 angka zona (0-5) untuk setiap peserta
   - Zona = poin (zona 5 = 5 poin, zona 0 = 0 poin)
3. **Lihat Ranking (Menu 3)**

   - Tampil setelah semua peserta selesai 7 tendangan
   - Menampilkan peringkat dengan tie-breaker
4. **Cari Peserta (Menu 4)**

   - Ketik nama peserta (case-insensitive)
   - Tampil detail: nama, 7 zona, total skor
5. **Rekapitulasi (Menu 5)**

   - Tabel lengkap: peringkat, nama, skor, frekuensi zona
6. **Keluar (Menu 0)**

   - Keluar dari program

---

## Aturan Bisnis

### Pendaftaran Peserta

| Aturan              | Detail                                  |
| ------------------- | --------------------------------------- |
| Jumlah peserta      | Minimal 5, maksimal 7                   |
| Nama peserta        | Unik (case-insensitive), tidak kosong   |
| Validasi nama       | Hanya huruf dan spasi, maks 30 karakter |
| Contoh nama sah     | "Budi Santoso", "Ali", "A"              |
| Contoh nama ditolak | "Budi123", "Ali@gmail.com", ""          |

### Tendangan dan Skor

| Aturan           | Detail                         |
| ---------------- | ------------------------------ |
| Jumlah tendangan | Tepat 7 kali per peserta       |
| Input zona       | Integer 0-5                    |
| Konversi poin    | Zona == Poin (zona 5 = 5 poin) |
| Total skor       | Penjumlahan semua zona         |

### Tabel Konversi Zona ke Poin

| Zona | Poin | Keterangan       |
| ---- | ---- | ---------------- |
| 0    | 0    | Miss             |
| 1    | 1    | Gol mudah        |
| 2    | 2    | Gol sedang       |
| 3    | 3    | Gol sulit        |
| 4    | 4    | Gol sangat sulit |
| 5    | 5    | Gol sempurna     |

### Ranking dan Tie-Breaker

| Urutan | Kriteria                                 |
| ------ | ---------------------------------------- |
| 1      | Total skor tertinggi                     |
| 2      | Jika seri: jumlah zona 5 terbanyak       |
| 3      | Jika masih seri: jumlah zona 4 terbanyak |
| 4      | Jika masih seri: jumlah zona 3 terbanyak |
| 5      | Jika masih seri: jumlah zona 2 terbanyak |
| 6      | Jika masih seri: jumlah zona 1 terbanyak |
| 7      | Jika semua sama: peringkat seri          |

**Contoh penomoran seri:**

- 2 peserta seri di posisi 1 → keduanya dapat peringkat 1
- Peserta berikutnya → peringkat 3 (bukan 2)

---

## State Machine

```
┌─────────┐    >=5 peserta    ┌────────────┐    semua selesai    ┌───────────┐
│  INIT   │ ───────────────> │ REGISTERED │ ──────────────────> │ COMPLETED │
│         │                  │            │                     │           │
│ Menu 1  │                  │ Menu 1,2   │                     │ Menu 1-5  │
│ aktif   │                  │ aktif      │                     │ aktif     │
└─────────┘                  └────────────┘                     └───────────┘
```

| State      | Menu Aktif                    | Keterangan                      |
| ---------- | ----------------------------- | ------------------------------- |
| INIT       | 1 (Pendaftaran)               | Belum ada peserta               |
| REGISTERED | 1, 2 (Pendaftaran, Tendangan) | Peserta sudah didaftarkan       |
| COMPLETED  | 1-5 (Semua menu)              | Semua peserta selesai tendangan |

---

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
├── cli/                     <- surfaces: TUI commands (I/O user via ncurses)
└── root_cli_main_entry.c   <- titik masuk (wiring semua container)

tests/
├── test_main.c              <- test runner
├── test_registration.c      <- 23 tests
├── test_scoring.c           <- 20 tests
├── test_ranking.c           <- 16 tests
├── test_search.c            <- 19 tests
├── test_recap.c             <- 13 tests
├── test_infrastructure.c    <- 2 tests (storage)
├── test_sanitizer.c         <- 8 tests
├── test_export.c            <- 3 tests
├── test_logger.c            <- 6 tests
└── test_integration.c       <- 4 tests (full pipeline)
```

---

## Makefile Targets

| Target | Deskripsi |
|--------|-----------|
| `make` | Build binary `aplikasi_perhitungan_penalti` |
| `make run` | Build lalu jalankan |
| `make test` | Jalankan 113 unit tests |
| `make win` | Build `.exe` untuk Windows (MinGW + PDCurses) |
| `make lint` | Static analysis dengan cppcheck |
| `make format` | Format code dengan clang-format |
| `make analyze` | Static analysis dengan scan-build (Clang) |
| `make valgrind` | Memory leak check dengan valgrind |
| `make metrics` | Tampilkan code metrics |
| `make install` | Install ke `/usr/local/bin` |
| `make uninstall` | Hapus dari `/usr/local/bin` |
| `make clean` | Hapus build artifacts |

---

## Testing

### Menjalankan Tests

```bash
make test       # jalankan semua 113 tests
make valgrind   # memory leak check
```

### Ringkasan Tests

| Module          | Jumlah Tests  | Keterangan                                 |
| --------------- | ------------- | ------------------------------------------ |
| Registration    | 23            | Validasi nama, append, duplikat, kapasitas |
| Scoring         | 20            | Validasi zona, record kick, state          |
| Ranking         | 16            | Urutan, tie-breaker, seri                  |
| Search          | 19            | Case-insensitive, not found, agent         |
| Recap           | 13            | Detail, agent, null checks                 |
| Storage         | 2             | Save/load roundtrip                        |
| Sanitizer       | 8             | String/int validation                      |
| Export          | 3             | TXT export, null/empty checks              |
| Logger          | 6             | Log levels, filtering                      |
| Integration     | 4             | Full pipeline, edge cases                  |
| **Total** | **113** |                                            |

### Validasi Kualitas

| Validasi              | Hasil              |
| --------------------- | ------------------ |
| Build                 | 0 warning, 0 error |
| Clang Static Analyzer | 0 bug              |
| Valgrind              | 0 errors, 0 leaks  |

---

## Aturan AES

Kode mengikuti aturan AES (detail di `RULES_AES.md`):

| Kelompok | Kode Aturan    | Topik                                                     |
| -------- | -------------- | --------------------------------------------------------- |
| Naming   | AES101, AES102 | Nama file`prefix_concept_suffix`, suffix per layer      |
| Import   | AES201-AES205  | Batas dependency, larangan circular import                |
| Quality  | AES301-AES305  | Batas ukuran file, mandatory definition, bypass comment   |
| Role     | AES401-AES406  | Peran tiap layer (taxonomy, contract, capabilities, dst.) |
| Orphan   | AES501-AES506  | Deteksi kode tak terhubung                                |

### Layer Arsitektur

| Layer          | Prefix              | Peran                                  |
| -------------- | ------------------- | -------------------------------------- |
| Taxonomy       | `taxonomy_`       | Domain models: struct, enum, konstanta |
| Contract       | `contract_`       | Interface: struct function pointers    |
| Capabilities   | `capabilities_`   | Business logic murni (tanpa I/O)       |
| Infrastructure | `infrastructure_` | I/O implementation (file, ncurses)     |
| Agent          | `agent_`          | Orchestration pipeline                 |
| Surfaces       | `surfaces_`       | TUI commands, user I/O (ncurses)      |
| Root           | `root_`           | Wiring/perakitan (tanpa logic)         |

---

## Dokumentasi

| Dokumen             | Isi                                                          |
| ------------------- | ------------------------------------------------------------ |
| `README.md`       | Dokumentasi lengkap (file ini)                               |
| `PRD.md`          | Requirement, aturan bisnis, state machine, library diizinkan |
| `ARCHITECTURE.md` | Definisi layer, naming, diagram dependency                   |
| `RULES_AES.md`    | Seluruh kode aturan AES + severity + cara perbaikan          |
| `AGENTS.md`       | Ringkasan navigasi untuk developer/AI agent                  |

---

## Statistik Proyek

| Metric       | Jumlah |
| ------------ | ------ |
| Source files | 33     |
| Header files | 37     |
| Source lines | ~1,790 |
| Header lines | ~855   |
| Test files   | 11     |
| Test lines   | ~1,807 |
| Total tests  | 113    |
