# AES Architecture: Agentic Engineering System — C99 Adaptation

The **Agentic Engineering System (AES)** adalah pola arsitektur berlapis ketat, sangat terdekopling, dan dirancang untuk kejelasan navigasi Developer. Dokumen ini adalah **adaptasi C99** dari AES v3.0 — semua konsep ditulis dalam terminologi bahasa C.

> **Bahasa Target:** C (C99). Tidak ada Rust, Python, atau TypeScript di proyek ini.
> Semua contoh menggunakan ekstensi `.c` dan `.h`.

---

## Terminologi

| Term                        | Definisi                                                                                                     |
| --------------------------- | ------------------------------------------------------------------------------------------------------------ |
| **Workspace**         | Root direktori proyek yang berisi semua konfigurasi dan source.                                              |
| **Feature directory** | Satu subdirektori di dalam`src/` yang mewakili satu fitur vertikal (mis. `registration/`, `scoring/`). |
| **Member**            | Satu file C  yang mewakili satu unit layer dalam satu fitur.                                               |

---

## Pilar Utama dan Filosofi

### 1. Strict Layered Boundary Enforcement

Kode dibagi ke dalam batas horizontal dan vertikal yang jelas. Layer hanya boleh berkomunikasi secara **downward-only** (lapisan di atas mengimpor lapisan di bawah, tidak sebaliknya).

### 2. Capabilities dan Infrastructure sebagai Peer Layer

Tidak seperti arsitektur tiga tier tradisional, **Capabilities** dan **Infrastructure** adalah layer horizontal yang setara (peer):

- Keduanya tidak saling mengimpor.
- Keduanya bergantung ke bawah pada layer **Contract** melalui struct function pointer (port/protocol).

### 3. Dependency Inversion di C

Di C, dependency inversion dilakukan melalui **struct of function pointers** sebagai pengganti interface/trait:

```c
/* contract_scoring_port.h  — outbound interface (diimplementasikan oleh infrastructure) */
typedef struct {
    int (*save_score)(int participant_id, int score);
    int (*load_score)(int participant_id, int *out_score);
} ScoringPort;

/* contract_scoring_protocol.h  — inbound interface (diimplementasikan oleh capabilities) */
typedef struct {
    int (*calculate_total)(const int kicks[7], int *out_total);
} ScoringProtocol;
```

Layer di atasnya (agent/surface) hanya memegang pointer ke struct ini, **bukan** ke implementasi konkretnya. Perakitan (wiring) dilakukan di layer `root_*_container.c`.

### 4. Filosofi Penamaan 3-Struktur (Layer Prefix + Konsep + Role Suffix)

AES mewajibkan konvensi nama file: `[layer]_[konsep]_[suffix].c` atau `[layer]_[konsep1]_[konsep2]_[suffix].c`

1. **Layer (prefix):** Layer arsitektur (`contract_`, `capabilities_`, `taxonomy_`, dst.).
2. **Konsep (tengah):** Satu atau lebih kata yang mendefinisikan konsep inti (`scoring`, `participant_name`, dst.).
3. **Role (suffix):** Peran arsitektur (`_port`, `_protocol`, `_checker`, dst.).

*Contoh:* `contract_scoring_port.h` → layer=contract, konsep=scoring, suffix=port.

File-file diorganisir ke dalam **feature directory** (vertical slicing). Tujuh layer koeksistensi di satu direktori fitur, dibedakan oleh prefix file.

---

## Hierarki Layer (Arah Dependency)

```
taxonomy → contract → capabilities / infrastructure → agent → surfaces → root (wiring only)
```

Arah panah = arah import yang diizinkan.

```
┌─────────────────────────────────────────────────────────┐
│  root_*_entry.c      (titik masuk OS — bootstrap)       │
│  root_*_container.c  (perakitan/wiring — per fitur)     │
├─────────────────────────────────────────────────────────┤
│  surfaces_*_command.c  (CLI commands, user I/O)         │
├─────────────────────────────────────────────────────────┤
│  agent_*_orchestrator.c  (koordinasi pipeline)          │
├─────────────────────────────────────────────────────────┤
│  capabilities_*_checker.c  │  infrastructure_*_adapter.c│
│  (business logic, pure)    │  (I/O sistem, wrapping lib)│
├─────────────────────────────────────────────────────────┤
│  contract_*_port.h / contract_*_protocol.h              │
│  contract_*_aggregate.h  (struct function pointers)     │
├─────────────────────────────────────────────────────────┤
│  taxonomy_*_vo.h / taxonomy_*_entity.h                  │
│  taxonomy_*_error.h / taxonomy_*_constant.h             │
└─────────────────────────────────────────────────────────┘
```

---

## Spesifikasi Layer dan Prefix File

Layer diidentifikasi via **file prefix**, bukan direktori.

| Layer Prefix        | Allowed Suffixes                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     | Allowed#include                                          | Peran Semantik                                                                                                                                                       |
| :------------------ | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :------------------------------------------------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `taxonomy_`       | `_vo`, `_entity`, `_event`, `_error`, `_constant`, `_utility`, `_helper`                                                                                                                                                                                                                                                                                                                                                                                                                               | `taxonomy_` saja                                       | Pure domain models: struct data, enum error, konstanta kompilasi. Tidak ada logika.                                                                                  |
| `contract_`       | `_port`, `_protocol`, `_aggregate`                                                                                                                                                                                                                                                                                                                                                                                                                                                                             | `taxonomy_`, `contract_`                             | Struct function pointer sebagai interface. Port = outbound (infra). Protocol = inbound (capabilities). Aggregate = facade komposit.                                  |
| `capabilities_`   | `_analyzer`, `_checker`, `_processor`, `_evaluator`, `_resolver`, `_validator`, `_formatter`, `_executor`, `_transformer`, `_calculator`, `_builder`, `_compiler`, `_aggregator`, `_classifier`, `_extractor`, `_reporter`, `_mapper`, `_filter`, `_collector`, `_comparator`, `_scorer`, `_inspector`, `_reviewer`, `_assessor`, `_auditor`                                                                                                                       | `taxonomy_`, `contract_`                             | Business logic dan kalkulasi domain. Murni — tidak tahu filesystem, terminal, atau library eksternal.                                                               |
| `infrastructure_` | `_adapter`, `_provider`, `_scanner`, `_client`, `_schemas`, `_lifespan`, `_wrapper`, `_tracer`, `_tracker`, `_variants`, `_detector`, `_patterns`, `_system`, `_repository`, `_cache`, `_loader`, `_writer`, `_reader`, `_driver`, `_connector`, `_gateway`, `_serializer`, `_encoder`, `_decoder`, `_fetcher`, `_watcher`, `_indexer`, `_dispatcher`, `_recorder`, `_proxy`, `_publisher`, `_subscriber`, `_listener`, `_poller`, `_streamer` | `taxonomy_`, `contract_`                             | Implementasi teknis: wrapping library C (ncurses, file I/O), pemanggilan system call. Wajib implementasi`_port`. Tidak ada terminal display — itu tugas surfaces. |
| `agent_`          | `_orchestrator`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    | `taxonomy_`, `contract_`                             | Mengkoordinasikan capabilities dan infrastructure melalui struct function pointer. Mengeksekusi pipeline/workflow.                                                   |
| `surfaces_`       | `_command`, `_controller`, `_page`, `_view`, `_component`, `_router`, `_layout`, `_hook`, `_store`, `_action`, `_screen`                                                                                                                                                                                                                                                                                                                                                                       | Bervariasi per sub-tipe surfaces (lihat detail di bawah) | Entry point aplikasi: CLI commands, display output ke terminal, navigasi menu.                                                                                       |
| `root_`           | `_container`, `_entry`                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           | Semua layer                                              | Bootstrap: perakitan DI (wiring struct function pointer), inisialisasi state, main loop. Tidak ada business logic.                                                   |

---

## Spesifikasi Detail per Layer

### 1. Taxonomy (`taxonomy_` prefix)

Pure domain models. Tidak ada fungsi bisnis, tidak ada I/O.

- **Value Object (`_vo`):** Struct immutable. Boleh gunakan tipe primitif C (`int`, `char[]`) secara langsung. *Contoh: `taxonomy_participant_vo.h`*
- **Entity (`_entity`):** Struct dengan identitas unik. Field harus gunakan VO, bukan primitif langsung. *Contoh: `taxonomy_participant_entity.h`*
- **Error (`_error`):** `enum` untuk error domain. *Contoh: `taxonomy_scoring_error.h`*
- **Constant (`_constant`):** Hanya `#define` atau `const` — tidak boleh ada fungsi. *Contoh: `taxonomy_game_constant.h`*
- **Event (`_event`):** Struct snapshot kejadian domain. *Contoh: `taxonomy_kick_recorded_event.h`*

### 2. Contract (`contract_` prefix)

Interface definitions — *apa* yang bisa dilakukan, tanpa *bagaimana*.

Di C, interface direpresentasikan sebagai **struct of function pointers**:

- **Port (`_port`):** Outbound interface, diimplementasikan oleh infrastructure. *Contoh: `contract_scoring_port.h`*
- **Protocol (`_protocol`):** Inbound interface, diimplementasikan oleh capabilities. *Contoh: `contract_scoring_protocol.h`*
- **Aggregate (`_aggregate`):** Facade yang mengomposis beberapa port/protocol. Digunakan oleh surfaces sebagai satu-satunya pintu masuk ke domain. *Contoh: `contract_service_aggregate.h`*

### 3. Capabilities (`capabilities_` prefix)

Use-case logic. **Sepenuhnya agnostik dari I/O** — tidak memanggil `printf`, tidak membuka file.

Mengimplementasikan fungsi yang terdaftar dalam `contract_*_protocol.h` dan mengisi struct function pointer-nya.

### 4. Infrastructure (`infrastructure_` prefix)

Implementasi teknis dan wrapping library eksternal.

- Mengimplementasikan fungsi yang terdaftar dalam `contract_*_port.h`.
- Boleh memanggil `fopen`, `ncurses`, `scanf`, I/O sistem — **tapi tidak untuk display pesan ke user**.
- Display pesan error dan output ke terminal adalah tanggung jawab **surfaces layer**.

### 5. Agent (`agent_` prefix)

Orkestrasi dan eksekusi pipeline.

- Memegang pointer ke `contract_*_aggregate.h` (atau langsung ke protocol/port struct).
- Mengkoordinasikan minimal 2 subsistem (capabilities + infrastructure) melalui contract.
- Tidak ada `printf`, tidak ada logika bisnis inlined.

### 6. Surfaces (`surfaces_` prefix)

Entry point aplikasi dan user-facing I/O.

- **Smart Surfaces (`_command`/`_controller`):** Menerima input user, memanggil agent melalui aggregate contract, menampilkan hasil/pesan error ke terminal. Hanya import `taxonomy_` dan `contract_aggregate_`.
- **Passive Surfaces (`_component`/`_view`/`_layout`):** Hanya rendering/formatting output. Tidak ada logika bisnis. Hanya import `taxonomy_`.

### 7. Root (`root_` prefix)

Wiring layer. **Tidak ada di hierarki domain** — root adalah lapisan perakitan di luar domain.

- **Container (`_container`):** Factory function per fitur. Menginstansiasi struct function pointer dari infrastructure dan capabilities, lalu menyusunnya menjadi aggregate contract. *Contoh: `root_scoring_container.c`*
- **Entry (`_entry`):** Titik masuk binary. Memanggil semua `_container`, membuat `CompetitionState`, memulai main loop. *Contoh: `root_cli_main_entry.c`*

---

## Contoh: Struktur File Fitur `scoring/`

```
src/scoring/
├── taxonomy_kick_vo.h                  ← struct Kick (zona, poin)
├── taxonomy_scoring_error.h            ← enum ScoringError
├── taxonomy_game_constant.h            ← MAX_KICKS, MAX_ZONE
├── contract_scoring_protocol.h         ← struct ScoringProtocol (fn ptr: calculate_total)
├── contract_scoring_port.h             ← struct ScoringPort (fn ptr: save/load — jika butuh)
├── contract_scoring_aggregate.h        ← struct ScoringAggregate (gabungan protocol+port)
├── capabilities_score_calculator.h     ← implementasi ScoringProtocol
├── capabilities_score_calculator.c
├── infrastructure_score_adapter.h      ← implementasi ScoringPort (jika butuh persistensi)
├── infrastructure_score_adapter.c
├── agent_scoring_orchestrator.h        ← koordinasi pipeline scoring
├── agent_scoring_orchestrator.c
├── surfaces_scoring_command.h          ← CLI: input tendangan, display hasil, display error
├── surfaces_scoring_command.c
└── root_scoring_container.c            ← wiring: buat ScoringAggregate dari konkret
```

---

## Lihat Juga

| Dokumen                     | Peran                                                          |
| --------------------------- | -------------------------------------------------------------- |
| [PRD.md](PRD.md)             | Konteks strategis — visi produk, aturan bisnis, state machine |
| [RULES_AES.md](RULES_AES.md) | Aturan enforcement untuk naming, import, dan role boundary     |
| [AGENTS.md](AGENTS.md)       | Konvensi operasional — struktur direktori, instruksi AI agent |
