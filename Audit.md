# Audit Laporan Blueprint — C-AES Penalty Shootout (v3)

**Tanggal Audit:** 2026-07-10
**Dokumen Rujukan:** PRD.md, ARCHITECTURE.md, RULES_AES.md
**Blueprint yang Diuji:** GLM, Mimo, Kimi, Qwen, Deepseek

---

## Rubrik Penilaian

| No | Aspek | Bobot | Definisi |
|----|-------|-------|----------|
| 1 | Maintainability | 10 | Keterbacaan kode, organisasi file, konsistensi naming, kemudahan modifikasi |
| 2 | Scalability | 10 | Kemudahan menambah fitur baru, isolasi antar fitur, dependency management |
| 3 | Performance | 10 | Efisiensi algoritma, overhead memory, computational complexity |
| 4 | Functionality | 10 | Kelengkapan fitur sesuai PRD, correctness logika bisnis |
| 5 | Stability | 10 | Error handling, input validation, crash prevention, edge case handling |
| 6 | UI/UX | 10 | Kejelasan output, navigasi menu, feedback ke user, formatting |
| 7 | Error Handling | 10 | Spesifisitas error, propagasi error, pemisahan error per fitur |
| 8 | Traceability | 10 | Komentar yang merujuk ke aturan/dokumen, keterlacakan keputusan desain |
| 9 | PRD Alignment | 10 | Kepatuhan terhadap requirement, flow, state machine, batasan teknis |
| 10 | Architecture Compliance | 10 | Kepatuhan terhadap AES101-506, layer boundary, import rules, role enforcement |

**Skor total: rata-rata 10 aspek (skala 1-10)**

---

## Ringkasan Eksekutif

| Blueprint | Maintainability | Scalability | Performance | Functionality | Stability | UI/UX | Error Handling | Traceability | PRD Alignment | Architecture | **RATA-RATA** |
|-----------|----------------|-------------|-------------|---------------|-----------|-------|----------------|--------------|---------------|--------------|---------------|
| **Deepseek** | 7 | 8 | 7 | 8 | 7 | 7 | 6 | 7 | 8 | 8 | **7.3** |
| **GLM** | 8 | 7 | 7 | 8 | 8 | 7 | 8 | 9 | 8 | 6 | **7.6** |
| **Mimo** | 6 | 7 | 7 | 7 | 6 | 7 | 7 | 6 | 7 | 7 | **6.7** |
| **Qwen** | 5 | 4 | 5 | 6 | 5 | 5 | 4 | 5 | 5 | 6 | **5.0** |
| **Kimi** | 4 | 4 | 6 | 5 | 4 | 5 | 4 | 3 | 5 | 2 | **4.2** |

---

## Audit Detail per Blueprint

---

### 1. GLM.md — Skor: 7.6/10

#### Maintainability: 8/10
- Kode sangat readable dengan komentar di setiap file yang menjelaskan justifikasi AES
- Naming konsisten: `capabilities_registration_name_validator.c`, `agent_scoring_orchestrator.c`
- Setiap fungsi punya satu tanggung jawab
- Struct state (`CompetitionStateVo`) didefinisikan lengkap dan jelas

#### Scalability: 7/10
- Vertical slicing per fitur sudah diterapkan
- Tapi tidak ada port contract — dependency inversion tidak lengkap
- Menambah fitur baru perlu membuat protocol + capabilities + agent + surfaces + root container

#### Performance: 7/10
- Insertion sort O(N²) untuk ranking — acceptable untuk N≤7
- Linear search O(N) untuk pencarian — acceptable
- Tidak ada overhead berlebihan

#### Functionality: 8/10
- Semua 5 fitur terimplementasi: Registration, Scoring, Ranking, Search, Recap
- Flow registrasi sesuai PRD (loop sampai kosong atau kuota tercapai)
- State machine (INIT → REGISTERED → COMPLETED) berfungsi
- Ranking tie-breaker benar (zona 5→4→3→2→1)

#### Stability: 8/10
- Null pointer check di semua fungsi
- Input validation untuk nama (kosong, terlalu panjang, karakter invalid, duplikat)
- Input validation untuk zona (0-5)
- Buffer flush untuk input non-numeric

#### UI/UX: 7/10
- Header aplikasi ditampilkan
- Progress indicator (peserta terdaftar, tendangan selesai)
- ASCII table untuk recap
- "Tekan Enter untuk kembali ke menu"

#### Error Handling: 8/10
- `CompetitionError` enum dengan 15+ error code spesifik
- Error dikelompokkan per fitur (registration, scoring, ranking, search, recap)
- Surface menerjemahkan error code ke pesan user-friendly
- Tidak ada error handling di capabilities atau infrastructure

#### Traceability: 9/10
- Komentar di setiap file merujuk ke aturan AES (AES201 #6, AES401, AES403, dst.)
- Komentar merujuk ke bagian PRD (PRD 2.1, PRD 2.2, PRD 2.3, dst.)
- Justifikasi keputusan desain dicatat (misal: duplikasi comparator untuk compliance AES201 #6)

#### PRD Alignment: 8/10
- Min 5, Max 7 peserta ✅
- Nama unik case-insensitive ✅
- 7 tendangan per peserta ✅
- Zona 0-5, poin = zona ✅
- State machine ✅
- Ranking descending + tie-breaker ✅
- Rank seri (1,1,3) ✅
- Search case-insensitive ✅
- Recap ASCII table ✅
- Flow registrasi loop ✅

#### Architecture Compliance: 6/10
- **AES101 Naming**: ✅ Semua file `prefix_concept_suffix`
- **AES102 Suffix**: ✅ Suffix sesuai layer
- **AES201 Import**: ⚠️ Tidak ada port — protocol saja. Dependency inversion tidak lengkap
- **AES401 Taxonomy**: ✅ VO pakai primitif (benar), entity pakai VO (benar)
- **AES402 Contract**: ✅ Protocol pakai VO/state types
- **AES403 Capabilities**: ✅ Implementasi protocol
- **AES404 Infrastructure**: ❌ Tidak ada infrastructure adapter — tidak ada port implementation
- **AES405 Agent**: ✅ Koordinasi via protocol (tapi hanya 1 subsystem, bukan 2)
- **AES406 Surface**: ⚠️ Surfaces lakukan I/O langsung (scanf/getchar) — seharusnya delegate ke infrastructure

---

### 2. Deepseek.md — Skor: 7.3/10

#### Maintainability: 7/10
- Kode terstruktur dengan baik, 69 file
- Tapi static singleton pattern (`static ScoringProtocol protocol;`) membingungkan
- Duplikasi ranking logic antara `capabilities_ranking_evaluator.c` dan `capabilities_recap_formatter.c`
- Komentar cukup baik tapi tidak sedalam GLM

#### Scalability: 8/10
- Protocol + Port separation memudahkan pergantian implementation
- Vertical slicing per fitur
- Root container per fitur
- Infrastructure adapter bisa diganti tanpa mengubah capabilities

#### Performance: 7/10
- qsort O(N log N) untuk ranking — lebih baik dari insertion sort
- Linear search O(N) untuk pencarian
- Tidak ada overhead signifikan

#### Functionality: 8/10
- Semua 5 fitur terimplementasi
- Flow registrasi sesuai PRD (loop sampai SELESAI)
- State machine berfungsi
- Ranking + tie-breaker benar

#### Stability: 7/10
- Null pointer check
- Input validation untuk nama dan zona
- Buffer flush
- Tapi tidak ada validasi untuk edge case seperti input kosong di search

#### UI/UX: 7/10
- ASCII table untuk ranking dan recap
- Header aplikasi
- Progress indicator
- Menu navigation

#### Error Handling: 6/10
- **Satu error enum untuk semua fitur** (`ScoringError`) — ini masalah. Registration error, scoring error, ranking error, search error, recap error semuanya pakai `ScoringError`
- Error code kurang spesifik per fitur
- Tapi propagasi error sudah benar (capabilities → agent → surface)

#### Traceability: 7/10
- AES compliance checklist di akhir file
- Komentar per file menjelaskan peran layer
- Tapi tidak merujuk ke bagian PRD spesifik

#### PRD Alignment: 8/10
- Semua requirement terpenuhi
- Flow registrasi benar
- State machine benar

#### Architecture Compliance: 8/10
- **AES101 Naming**: ✅
- **AES102 Suffix**: ✅
- **AES201 Import**: ✅ Capabilities import taxonomy+contract, Infrastructure import taxonomy+contract
- **AES401 Taxonomy**: ✅ VO pakai primitif, entity pakai VO
- **AES402 Contract**: ⚠️ Registration protocol pakai VO (benar), tapi scoring protocol pakai `int zone` (melanggar — seharusnya `ZoneVO`)
- **AES403 Capabilities**: ✅ Implementasi protocol
- **AES404 Infrastructure**: ✅ Implementasi port
- **AES405 Agent**: ✅ Koordinasi 2 subsystem (protocol + port)
- **AES406 Surface**: ✅ Surface hanya delegasi ke agent

---

### 3. Mimo.md — Skor: 6.7/10

#### Maintainability: 6/10
- Banyak definisi hilang (taxonomy_participant_vo.h, taxonomy_competition_entity.h, taxonomy_competition_state_vo.h, taxonomy_competition_error.h, taxonomy_string_utility.h)
- File dimulai dari "You're right — I was in the middle of building..." — incomplete
- `OperationResultVO` (wrapper `int value`) menambah kompleksitas tanpanilai
- `ci_strcmp` digunakan tapi tidak didefinisikan

#### Scalability: 7/10
- Protocol + Port separation
- Vertical slicing per fitur
- Tapi global variable di recap adapter mengurangi isolasi

#### Performance: 7/10
- Bubble sort O(N²) — acceptable
- Linear search O(N)
- Tidak ada overhead signifikan

#### Functionality: 7/10
- Flow registrasi benar (loop sampai 'selesai')
- Semua fitur ada
- Tapi banyak kode tidak ditampilkan — tidak bisa diverifikasi

#### Stability: 6/10
- Global variable `static RankingAggregate *g_ranking_aggregate` — thread-unsafe, error-prone
- Missing definitions membuat tidak bisa compile
- Error handling ada tapi tidak bisa diverifikasi sepenuhnya

#### UI/UX: 7/10
- Menu [terkunci] indicator
- ASCII table untuk recap
- Progress indicator

#### Error Handling: 7/10
- Error enum per fitur (`RegistrationError`, `ScoringError`, `RankingError`, `SearchError`, `RecapError`)
- Lebih baik dari Deepseek yang pakai satu enum untuk semua

#### Traceability: 6/10
- Komentar ada tapi tidak sedalam GLM
- File tree lengkap

#### PRD Alignment: 7/10
- Flow registrasi benar
- Semua fitur ada
- Tapi kode tidak lengkap — tidak bisa verifikasi semua requirement

#### Architecture Compliance: 7/10
- **AES101 Naming**: ✅
- **AES102 Suffix**: ✅
- **AES201 Import**: ⚠️ Global variable di recap adapter
- **AES401 Taxonomy**: ✅ VO + Entity pattern
- **AES402 Contract**: ✅ Protocol pakai VO types
- **AES403 Capabilities**: ✅ Implementasi protocol
- **AES404 Infrastructure**: ✅ Implementasi port
- **AES405 Agent**: ✅ Koordinasi 2 subsystem
- **AES406 Surface**: ⚠️ Surfaces lakukan scanf langsung

---

### 4. Qwen.md — Skor: 5.0/10

#### Maintainability: 5/10
- **6 VO wrapper** (`NameVO`, `IdVO`, `IndexVO`, `ScoreVO`, `ZoneVO`, `CountVO`) — ini **benar sesuai AES401** karena entity harus pakai VO, bukan primitif
- Tapi `IndexVO`, `ScoreVO`, `ZoneVO`, `CountVO` hanya wrap `int` — setiap kali akses harus pakai `.value`, mengurangi keterbacaan
- `CommonError` hanya 2 value (`COMMON_SUCCESS`, `COMMON_ERR_UNKNOWN`) — error handling sangat generik
- `void* context` di `GameAggregate` — tidak type-safe

#### Scalability: 4/10
- **Shared `contract_game_aggregate.h`** — satu aggregate untuk semua fitur, melanggar vertical slicing
- **Agent directory terpisah** (`src/agent/`) — agent harusnya per-fitur
- **Surfaces directory terpisah** (`src/surfaces/`) — surfaces harusnya per-fitur
- Menambah fitur baru harus modifikasi `GameAggregate` dan `agent_game_orchestrator.c`

#### Performance: 5/10
- `malloc` untuk protocol structs — overhead tidak perlu untuk struct kecil
- `void* context` add indirection overhead
- Bubble sort O(N²) — acceptable

#### Functionality: 6/10
- Semua 5 fitur ada
- **Flow registrasi salah**: Input jumlah di awal (`"Masukkan jumlah peserta (5-7): "`), bukan loop input nama
- Ranking + tie-breaker benar
- Search case-insensitive benar

#### Stability: 5/10
- `destroy_agent_context` memfree protocol structs — ownership tidak jelas (protocol di-alloc di container, freed di agent)
- `malloc` tanpa null check di beberapa tempat
- Error handling sangat generik (`CommonError`)

#### UI/UX: 5/10
- Menu dasar tanpa indicator state
- Ranking output sederhana (tanpa ASCII table)
- Recap punya ASCII table

#### Error Handling: 4/10
- `CommonError` hanya `COMMON_SUCCESS` dan `COMMON_ERR_UNKNOWN` — **sangat generik**
- Tidak ada error spesifik per fitur (registration, scoring, ranking, search, recap)
- Semua error di-derive dari `CommonError`

#### Traceability: 5/10
- Komentar minim
- Tidak ada rujukan ke aturan AES atau bagian PRD

#### PRD Alignment: 5/10
- Flow registrasi **tidak sesuai PRD** — PRD mengharapkan loop input nama, bukan input jumlah di awal
- Min 5, Max 7 peserta: ada tapi tidak diterapkan dengan benar
- Fitur lain ada

#### Architecture Compliance: 6/10
- **AES101 Naming**: ✅
- **AES102 Suffix**: ✅
- **AES201 Import**: ⚠️ Shared aggregate di `src/shared/` — seharusnya per-fitur
- **AES401 Taxonomy**: ✅ VO files pakai primitif (benar), entity pakai VO (benar)
- **AES402 Contract**: ⚠️ Registration pakai `NameVO` (benar), tapi scoring pakai `int zone` (melanggar — seharusnya `ZoneVO`)
- **AES403 Capabilities**: ✅ Implementasi protocol
- **AES404 Infrastructure**: ❌ **Tidak ada infrastructure adapter** — tidak ada port implementation
- **AES405 Agent**: ⚠️ Agent ada tapi centralized, bukan per-fitur
- **AES406 Surface**: ✅ Surface hanya delegasi

---

### 5. Kimi.md — Skor: 4.2/10

#### Maintainability: 4/10
- Banyak pelanggaran arsitektur — kode sulit dimodifikasi tanpa melanggar aturan
- `taxonomy_*.c` files kosong (melanggar AES302)
- `extern` declaration di capabilities (melanggar AES201)
- `strcasecmp` non-standard (tidak ada di C99)

#### Scalability: 4/10
- Agent melakukan terlalu banyak (I/O + logic)
- Infrastructure melakukan display
- Sulit menambah fitur tanpa melanggar batasan layer

#### Performance: 6/10
- Algoritma sesuai kebutuhan
- Tidak ada overhead berlebihan

#### Functionality: 5/10
- Semua fitur ada
- Flow registrasi salah (input jumlah di awal)
- Ranking benar
- Search benar

#### Stability: 4/10
- Agent lakukan printf — bisa crash jika stdout bermasalah
- `extern` declaration — linking error risk
- Missing `taxonomy_competition_state_entity.h`

#### UI/UX: 5/10
- Menu dasar
- Output cukup jelas

#### Error Handling: 4/10
- Error handling tidak konsisten antar fitur
- Agent menampilkan error langsung (bukan delegate ke surface)

#### Traceability: 3/10
- Sangat minim komentar
- Tidak ada rujukan ke aturan AES

#### PRD Alignment: 5/10
- Flow registrasi salah (input jumlah di awal)
- Fitur lain ada

#### Architecture Compliance: 2/10
- **AES101 Naming**: ⚠️ Beberapa file tidak sesuai
- **AES102 Suffix**: ⚠️ Beberapa suffix tidak sesuai
- **AES201 Import**: ❌ Agent impor infrastructure langsung
- **AES401 Taxonomy**: ⚠️ Entity files kosong
- **AES402 Contract**: ⚠️ Protocol tidak konsisten
- **AES403 Capabilities**: ⚠️ `extern` declaration
- **AES404 Infrastructure**: ❌ Infrastructure lakukan display (printf)
- **AES405 Agent**: ❌ Agent lakukan printf, impor infrastructure
- **AES406 Surface**: ❌ Surface tidak ada (agent langsung I/O)

---

## Perbandingan per Aspek

### 1. Maintainability

| Blueprint | Skor | Kekuatan | Kelemahan |
|-----------|------|----------|-----------|
| GLM | 8 | Komentar traceability terbaik, kode readable | Tidak ada port contract |
| Deepseek | 7 | Struktur konsisten | Static singleton, duplikasi ranking |
| Mimo | 6 | Protocol+Port ada | Banyak definisi hilang |
| Qwen | 5 | VO pattern benar per AES401 | `void*`, `CommonError` generik |
| Kimi | 4 | Semua fitur ada | Banyak pelanggaran boundary |

### 2. Scalability

| Blueprint | Skor | Kekuatan | Kelemahan |
|-----------|------|----------|-----------|
| Deepseek | 8 | Protocol+Port, root container per fitur | Static singleton |
| Mimo | 7 | Protocol+Port, vertical slicing | Global variable di recap |
| GLM | 7 | Vertical slicing | Tanpa port, dependency inversion tidak lengkap |
| Qwen | 4 | Vertical slicing ada | Shared aggregate, agent centralized |
| Kimi | 4 | Vertical slicing ada | Agent lakukan I/O |

### 3. Performance

| Blueprint | Skor | Algoritma | Overhead |
|-----------|------|-----------|----------|
| Deepseek | 7 | qsort O(N log N) | Static singleton |
| GLM | 7 | Insertion sort O(N²) | Minimal |
| Mimo | 7 | Bubble sort O(N²) | Minimal |
| Qwen | 5 | Bubble sort O(N²) | `malloc`, `void*` |
| Kimi | 6 | qsort O(N log N) | Minimal |

### 4. Functionality

| Blueprint | Skor | Flow Registrasi | Fitur Lengkap |
|-----------|------|-----------------|---------------|
| Deepseek | 8 | ✅ Loop sampai SELESAI | ✅ |
| GLM | 8 | ✅ Loop sampai kosong | ✅ |
| Mimo | 7 | ✅ Loop sampai selesai | ✅ |
| Qwen | 6 | ❌ Input jumlah di awal | ✅ |
| Kimi | 5 | ❌ Input jumlah di awal | ✅ |

### 5. Stability

| Blueprint | Skor | Error Check | Input Validation | Crash Prevention |
|-----------|------|-------------|------------------|------------------|
| GLM | 8 | ✅ Null check | ✅ Lengkap | ✅ Buffer flush |
| Deepseek | 7 | ✅ Null check | ✅ Cukup | ✅ |
| Mimo | 6 | ⚠️ Tidak lengkap | ⚠️ Tidak bisa verifikasi | ⚠️ Global variable |
| Qwen | 5 | ⚠️ Minimal | ⚠️ Minimal | ❌ `malloc` tanpa null check |
| Kimi | 4 | ⚠️ Minimal | ⚠️ Minimal | ❌ `extern` risk |

### 6. UI/UX

| Blueprint | Skor | Menu | Output | Formatting |
|-----------|------|------|--------|------------|
| GLM | 7 | ✅ State indicator | ✅ Progress | ✅ ASCII table |
| Deepseek | 7 | ✅ | ✅ | ✅ ASCII table |
| Mimo | 7 | ✅ [terkunci] | ✅ Progress | ✅ ASCII table |
| Qwen | 5 | ⚠️ Dasar | ⚠️ Sederhana | ✅ ASCII table |
| Kimi | 5 | ⚠️ Dasar | ✅ | ⚠️ |

### 7. Error Handling

| Blueprint | Skor | Error Enum | Spesifisitas | Propagasi |
|-----------|------|------------|--------------|-----------|
| GLM | 8 | 15+ code, per fitur | ✅ Spesifik | ✅ Capabilities→Agent→Surface |
| Mimo | 7 | Per fitur | ✅ Spesifik | ✅ |
| Deepseek | 6 | 1 ScoringError untuk semua | ⚠️ Generik | ✅ |
| Qwen | 4 | CommonError (2 value) | ❌ Sangat generik | ⚠️ |
| Kimi | 4 | Tidak konsisten | ⚠️ | ❌ Agent tampilkan langsung |

### 8. Traceability

| Blueprint | Skor | Komentar AES | Rujukan PRD | Decision Log |
|-----------|------|-------------|-------------|--------------|
| GLM | 9 | ✅ Setiap file | ✅ Bagian spesifik | ✅ Justifikasi duplikasi |
| Deepseek | 7 | ✅ AES checklist | ⚠️ | ⚠️ |
| Mimo | 6 | ⚠️ | ⚠️ | ⚠️ |
| Qwen | 5 | ⚠️ | ❌ | ❌ |
| Kimi | 3 | ❌ | ❌ | ❌ |

### 9. PRD Alignment

| Blueprint | Skor | Flow | State Machine | Requirement |
|-----------|------|------|---------------|-------------|
| Deepseek | 8 | ✅ | ✅ | ✅ Semua |
| GLM | 8 | ✅ | ✅ | ✅ Semua |
| Mimo | 7 | ✅ | ✅ | ⚠️ Kode tidak lengkap |
| Qwen | 5 | ❌ Input jumlah | ✅ | ⚠️ |
| Kimi | 5 | ❌ Input jumlah | ✅ | ⚠️ |

### 10. Architecture Compliance

| Aturan | Deepseek | GLM | Mimo | Qwen | Kimi |
|--------|----------|-----|------|------|------|
| AES101 Naming | ✅ | ✅ | ✅ | ✅ | ⚠️ |
| AES102 Suffix | ✅ | ✅ | ✅ | ✅ | ⚠️ |
| AES201 Import | ✅ | ⚠️ | ⚠️ | ⚠️ | ❌ |
| AES401 Taxonomy | ✅ | ✅ | ✅ | ✅ | ⚠️ |
| AES402 Contract | ⚠️ | ✅ | ✅ | ⚠️ | ⚠️ |
| AES403 Capabilities | ✅ | ✅ | ✅ | ✅ | ⚠️ |
| AES404 Infrastructure | ✅ | ❌ | ✅ | ❌ | ❌ |
| AES405 Agent | ✅ | ✅ | ✅ | ⚠️ | ❌ |
| AES406 Surface | ✅ | ⚠️ | ⚠️ | ✅ | ❌ |

**Skor Architecture Compliance:**
- Deepseek: 8/10 (AES402 minor: scoring pakai `int zone`)
- Mimo: 7/10 (AES201 global variable, AES406 surfaces lakukan scanf)
- Qwen: 6/10 (AES402 minor, AES404 missing infrastructure, shared aggregate)
- GLM: 6/10 (AES404 missing infrastructure, AES406 surfaces lakukan I/O)
- Kimi: 2/10 (Banyak pelanggaran critical)

---

## Rekomendasi Final

### Peringkat

| # | Blueprint | Skor | Rekomendasi |
|---|-----------|------|-------------|
| 1 | **GLM** | 7.6 | Kode paling readable, traceability terbaik. Perlu tambah port contract dan infrastructure layer |
| 2 | **Deepseek** | 7.3 | Architecture compliance terbaik (protocol+port). Perlu fix error enum per fitur dan static singleton |
| 3 | **Mimo** | 6.7 | Protocol+Port ada. Perlu lengkapi definisi yang hilang, hapus global variable |
| 4 | **Qwen** | 5.0 | VO pattern benar per AES401. Perlu tambah infrastructure layer, fix flow registrasi, buat error enum per fitur |
| 5 | **Kimi** | 4.2 | Banyak pelanggaran arsitektur. Perlu refactor signifikan |

### Perbaikan yang Diperlukan per Blueprint

**GLM:**
1. Tambah `contract_*_port.h` dan `infrastructure_*_adapter.c` untuk setiap fitur
2. Agent harus koordinasi 2 subsystem (protocol + port)
3. Pindahkan I/O dari surfaces ke infrastructure

**Deepseek:**
1. Pisahkan error enum per fitur (jangan pakai `ScoringError` untuk semua)
2. Fix static singleton pattern — gunakan stack allocation atau parameter passing
3. Fix agent printf di ranking — delegate ke port

**Mimo:**
1. Lengkapi semua definisi yang hilang
2. Hapus global variable `static RankingAggregate *g_ranking_aggregate`
3. Pindahkan scanf dari surfaces ke infrastructure

**Qwen:**
1. Tambah `infrastructure_*_adapter.c` untuk setiap fitur (implement port)
2. Pindahkan aggregate dari `src/shared/` ke per-fitur
3. Pindahkan agent dari `src/agent/` ke per-fitur
4. Buat error enum per fitur (RegistrationError, ScoringError, dst.)
5. Fix flow registrasi: loop input nama, bukan input jumlah di awal
6. Fix AES402: scoring protocol harus pakai `ZoneVO`, bukan `int zone`

**Kimi:**
1. Hapus printf dari agent layer
2. Hapus import infrastructure dari agent
3. Pindahkan display dari infrastructure ke surfaces
4. Fix flow registrasi
5. Lengkapi semua definisi yang hilang
6. Hapus `extern` declaration dari capabilities
