
Laporan Audit Kepatuhan Arsitektur AES v3.0
Proyek: aplikasi-penalty (C99 Adaptation) Tanggal Audit: 2026-07-10 Basis:
RULES_AES.md
 ×
ARCHITECTURE.md

Ringkasan Eksekutif
Severity	Jumlah Temuan
🔴 CRITICAL	1
🟠 HIGH	3
🟡 MEDIUM	4
🔵 LOW	2
TOTAL	10
🔴 CRITICAL
[AES201] Forbidden Import — Surfaces mengimpor Infrastructure langsung
File: Semua surfaces_*_command.c di /src/cli/ + root_cli_main_entry.c

Seluruh file surfaces layer mengimpor tui/infrastructure_tui_adapter.h secara langsung:

c

// surfaces_ranking_command.c:7
#include "tui/infrastructure_tui_adapter.h"   // ← VIOLATION
Terjadi di:

surfaces_ranking_command.c
surfaces_recap_command.c
surfaces_scoring_command.c
surfaces_registration_command.c
surfaces_search_command.c
surfaces_menu_command.c
root_cli_main_entry.c
Aturan dilanggar: AES201 rule #9 — surfaces(command|controller) dilarang mengimpor infrastructure*.

Dampak: Layer coupling langsung antara UI surface dan ncurses/infrastructure. Jika library TUI diganti (misal dari ncurses ke SDL), seluruh surfaces harus diubah.

Fix yang disarankan: Bungkus semua fungsi TUI (tui_clear, tui_print_centered_colored, dll.) dalam sebuah contract_display_protocol.h, lalu surfaces hanya memanggil melalui protocol tersebut.

🟠 HIGH
[AES401] Taxonomy Role — taxonomy_participant_entity.h menggunakan primitif C langsung
File:
taxonomy_participant_entity.h

c

typedef struct {
    int id;                    // ← primitif langsung di _entity
    ParticipantNameVO name;    // ✅ sudah VO
    int kicks[TOTAL_KICKS];    // ← primitif langsung di _entity
    int total_score;           // ← primitif langsung di _entity
    int zone_freq[MAX_ZONE+1]; // ← primitif langsung di _entity
    int kick_count;            // ← primitif langsung di _entity
} ParticipantEntity;
Aturan dilanggar: AES401 sub-check 2 — _entity tidak boleh menggunakan tipe primitif (int) langsung pada field deklarasi; harus menggunakan taxonomy Value Object.

Catatan: name sudah benar (memakai ParticipantNameVO). Field kicks, total_score, zone_freq, kick_count, dan id seharusnya dibungkus dalam VO seperti KickResultVO, TotalScoreVO, dsb.

[AES405] Agent Role — agent_storage_orchestrator.c hanya mengorkestrasi 1 subsistem
File:
agent_storage_orchestrator.c

c

// Hanya meneruskan ke agg->protocol->save/load — tidak ada koordinasi 2+ subsistem
return agg->protocol->save(filename, state);
Aturan dilanggar: AES405 — orchestrator harus mengoordinasikan minimal 2 subsistem.

Saat ini agent_storage_orchestrator hanya meneruskan panggilan ke 1 protocol (storage). Tidak ada koordinasi antara dua kapabilitas yang berbeda, sehingga fungsi agent ini tidak memiliki nilai tambah orkestrasi.

[AES405] Agent Role — agent_export_orchestrator.c hanya mengorkestrasi 1 subsistem
File:
agent_export_orchestrator.c

Identik dengan storage — hanya meneruskan panggilan ke 1 protocol. Tidak ada koordinasi antar subsistem.

🟡 MEDIUM
[AES503] Capabilities Orphan — sanitizer tidak terhubung ke sistem manapun
Feature: src/sanitizer/

capabilities_sanitizer_validator.c mengimplementasikan SanitizeProtocol, namun:

Tidak ada agent yang menggunakannya melalui protocol
Tidak ada container yang mewiring-nya
Surfaces memanggil sanitizer_validate_string() dan sanitizer_validate_int() secara langsung (bypass DI), bukan melalui contract_sanitize_protocol.h
c

// capabilities_sanitizer_validator.c mengekspos fungsi langsung:
SanitizeError sanitizer_validate_string(const char *input, size_t max_length);
// ← Surface memanggil ini langsung, bukan via struct protocol
Aturan dilanggar: AES503 — capability tidak terhubung ke container manapun, tapi masih reachable via import langsung (bypass DI pattern).

[AES501] Taxonomy Orphan — taxonomy_kick_vo.h tidak diimpor oleh contract apapun
File:
taxonomy_kick_vo.h

grep -rn 'taxonomy_kick_vo' → hanya ditemukan di file itu sendiri
KickVO terdefinisi namun tidak digunakan oleh contract apapun. Padahal scoring menggunakan ZoneVO untuk input tendangan — KickVO tampaknya adalah artefak yang belum digunakan atau sudah usang.

Aturan dilanggar: AES501 — taxonomy VO tidak diimpor oleh contract manapun.

[AES406] Surface Role — surfaces_recap_command.c mengimpor module.export.h langsung
File:
surfaces_recap_command.c

c

#include "export/module.export.h"   // ← surfaces import modul non-aggregate
Surfaces seharusnya hanya mengimpor taxonomy_ dan contract_aggregate. Mengimpor module.export.h yang berisi root_export_build() berarti surface melakukan wiring sendiri — peran yang seharusnya ada di root_*_container.c.

[AES506] Surface Orphan — surfaces_menu_command.c tidak diimpor secara eksplisit
File:
surfaces_menu_command.c

cli_surfaces_menu_run() dipanggil dari root_cli_main_entry.c melalui module.cli.h, tapi module.cli.h tidak secara eksplisit #include "cli/surfaces_menu_command.h" — fungsinya hanya ada di module.cli.h sebagai deklarasi forward. Ini adalah pola "hidden linkage" yang tidak transparan.

🔵 LOW
[AES302] File Minimum Limit — capabilities_scoring_zone_validator.c hanya 18 baris
File:
capabilities_scoring_zone_validator.c
 (18 baris)

Di bawah ambang minimum 5 baris yang diizinkan — namun hanya 18 baris total. File ini sangat tipis dan mungkin bisa digabung dengan capabilities_scoring_score_calculator.c.

NOTE

Batas minimum 5 baris terpenuhi (18 > 5), tapi rule AES302 biasanya diinvestigasi untuk file sangat kecil karena potensi "split tidak perlu".

[AES501] Taxonomy Orphan — taxonomy_logger.h / taxonomy_logger.c tidak diimpor contract apapun
File:
taxonomy_logger.h
,
taxonomy_logger.c

Logger terdefinisi tapi tidak ada contract, capabilities, infrastructure, atau surfaces yang mengimpornya. Logger adalah cross-cutting concern yang tidak dihubungkan ke dependency graph.

📐 Klarifikasi Pola Arsitektur Contract (C99)
Berikut pola yang benar dan disengaja dalam proyek ini — bukan pelanggaran:

Contract Suffix	Implementor	Caller	Peran
_port	infrastructure_*	agent_* (via struct)	Outbound I/O — infra mengisi function pointer-nya
_protocol	capabilities_*	agent_* (via struct)	Inbound logic — capabilities mengisi function pointer-nya
_aggregate	agent_* (mengimplementasikan fungsi yang di-expose)	surfaces_* (memanggil melalui aggregate)	Facade domain — agent menyediakan, surface mengonsumsi
Perbedaan kritis _aggregate:

Implement (agent): agent mengisi/mengeksekusi logika yang dijanjikan aggregate — mis. agent_ranking_compute() adalah implementasi dari RankingAggregate.protocol->compute_ranking
Call (surface): surface menerima aggregate sebagai parameter dan hanya memanggil fungsi melalui struct pointer — surface tidak boleh tahu siapa implementornya
Aspek	Status
Naming convention (prefix_concept_suffix) semua file	✅ Patuh
Suffix per layer (AES102) — semua suffix valid	✅ Patuh
taxonomy_ hanya mengimpor sesama taxonomy_	✅ Patuh
contract_ mengimpor taxonomy_ dan sesama contract_	✅ Patuh
capabilities_ tidak mengimpor infrastructure_	✅ Patuh
root_*_container.c melakukan wiring yang tepat	✅ Patuh
root_cli_main_entry.c memanggil semua container	✅ Patuh
Tidak ada printf di capabilities layer	✅ Patuh
Tidak ada fopen/I/O di capabilities layer	✅ Patuh
Tidak ada circular import	✅ Patuh
Semua file di bawah 1000 baris (AES301)	✅ Patuh
taxonomy_constant hanya berisi #define	✅ Patuh
agent_recap_orchestrator mengoordinasikan 2 subsistem	✅ Patuh
Prioritas Perbaikan

1. AES201 (CRITICAL)  → Buat contract_display_protocol.h; surfaces tidak boleh sentuh ncurses langsung
2. AES406 (MEDIUM)    → Pindahkan root_export_build() dari surfaces_recap ke root_cli_main_entry
3. AES501 (LOW)       → Hapus taxonomy_kick_vo.h atau hubungkan ke scoring contract
4. AES501 (LOW)       → Hubungkan taxonomy_logger ke sistem (via contract atau hapus)
