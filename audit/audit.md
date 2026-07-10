
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


# Fix AES201: Decouple Surfaces dari Infrastructure TUI

## Masalah

Semua `surfaces_*_command.c` mengimpor `tui/infrastructure_tui_adapter.h` langsung. Ada **~211 pemanggilan** raw ncurses (`attron`, `attroff`, `mvprintw`, `printw`, `refresh()`) tersebar di 6 surfaces file.

## Strategi

Buat lapisan abstraksi `DisplayPort` yang menjembatani surfaces ↔ ncurses, tanpa mengubah logika bisnis surfaces sama sekali.

<pre><div node="[object Object]" class="relative whitespace-pre-wrap word-break-all my-2 rounded-xl bg-muted border"><div class="min-h-7 relative box-border flex flex-row items-center justify-between rounded-t border-b border-border px-2 py-0.5"><div class="font-sans text-sm text-muted-foreground"></div><div class="flex flex-row gap-2 justify-end"><button aria-label="Copy code" class="appearance-none bg-transparent border-0 p-0 cursor-pointer text-secondary-foreground hover:text-foreground transition-colors"><svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 -960 960 960" fill="currentColor" class="h-3.5 w-3.5"><path d="M362.31-260Q332-260 311-281t-21-51.31V-787.69Q290-818 311-839t51.31-21H697.69Q728-860 749-839t21,51.31v455.38Q770-302 749-281t-51.31,21H362.31Zm0-60H697.69q4.62,0 8.46-3.85t3.85-8.46V-787.69q0-4.62-3.85-8.46T697.69-800H362.31q-4.62,0-8.46,3.85T350-787.69v455.38q0,4.62 3.85,8.46t8.46,3.85Zm-140,200Q192-120 171-141t-21-51.31V-707.69h60v515.38q0,4.62 3.85,8.46t8.46,3.85H617.69v60H222.31ZM350-320q0,0 0-3.85t0-8.46V-787.69q0-4.62 0-8.46t0-3.85q0,0 0,3.85t0,8.46v455.38q0,4.62 0,8.46t0,3.85Z"></path></svg></button></div></div><div class="p-3"><div class="w-full h-full text-xs cursor-text"><div class="code-block"><div class="code-line" data-line-number="1" data-line-start="1" data-line-end="1"><div class="line-content"><span class="mtk1">[surfaces_*_command.c]</span></div></div><div class="code-line" data-line-number="2" data-line-start="2" data-line-end="2"><div class="line-content"><span class="mtk1">      ↓ call via struct pointer</span></div></div><div class="code-line" data-line-number="3" data-line-start="3" data-line-end="3"><div class="line-content"><span class="mtk1">[contract_display_port.h]  ← DisplayPort { fn ptrs }</span></div></div><div class="code-line" data-line-number="4" data-line-start="4" data-line-end="4"><div class="line-content"><span class="mtk1">      ↑ implemented by</span></div></div><div class="code-line" data-line-number="5" data-line-start="5" data-line-end="5"><div class="line-content"><span class="mtk1">[infrastructure_tui_adapter.c]</span></div></div><div class="code-line" data-line-number="6" data-line-start="6" data-line-end="6"><div class="line-content"><span class="mtk1">      ↑ wraps</span></div></div><div class="code-line" data-line-number="7" data-line-start="7" data-line-end="7"><div class="line-content"><span class="mtk1">[ncurses]</span></div></div></div></div></div></div></pre>

---

## Proposed Changes

### Taxonomy Layer

#### [NEW] **taxonomy_display_constant.h**

Pindahkan semua `#define COLOR_*` dan `TUI_KEY_*` dari `infrastructure_tui_adapter.h` ke sini. Surfaces hanya `#include` file ini untuk konstanta warna — tanpa menyentuh ncurses.

<pre><div node="[object Object]" class="relative whitespace-pre-wrap word-break-all my-2 rounded-xl bg-muted border"><div class="min-h-7 relative box-border flex flex-row items-center justify-between rounded-t border-b border-border px-2 py-0.5"><div class="font-sans text-sm text-muted-foreground">c</div><div class="flex flex-row gap-2 justify-end"><button aria-label="Copy code" class="appearance-none bg-transparent border-0 p-0 cursor-pointer text-secondary-foreground hover:text-foreground transition-colors"><svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 -960 960 960" fill="currentColor" class="h-3.5 w-3.5"><path d="M362.31-260Q332-260 311-281t-21-51.31V-787.69Q290-818 311-839t51.31-21H697.69Q728-860 749-839t21,51.31v455.38Q770-302 749-281t-51.31,21H362.31Zm0-60H697.69q4.62,0 8.46-3.85t3.85-8.46V-787.69q0-4.62-3.85-8.46T697.69-800H362.31q-4.62,0-8.46,3.85T350-787.69v455.38q0,4.62 3.85,8.46t8.46,3.85Zm-140,200Q192-120 171-141t-21-51.31V-707.69h60v515.38q0,4.62 3.85,8.46t8.46,3.85H617.69v60H222.31ZM350-320q0,0 0-3.85t0-8.46V-787.69q0-4.62 0-8.46t0-3.85q0,0 0,3.85t0,8.46v455.38q0,4.62 0,8.46t0,3.85Z"></path></svg></button></div></div><div class="p-3"><div class="w-full h-full text-xs cursor-text"><div class="code-block"><div class="code-line" data-line-number="1" data-line-start="1" data-line-end="1"><div class="line-content"><span class="mtk18">#define</span><span class="mtk6"> COLOR_DEFAULT   </span><span class="mtk7">0</span></div></div><div class="code-line" data-line-number="2" data-line-start="2" data-line-end="2"><div class="line-content"><span class="mtk18">#define</span><span class="mtk6"> COLOR_TITLE     </span><span class="mtk7">1</span></div></div><div class="code-line" data-line-number="3" data-line-start="3" data-line-end="3"><div class="line-content"><span class="mtk5">// ... dst</span></div></div><div class="code-line" data-line-number="4" data-line-start="4" data-line-end="4"><div class="line-content"><span class="mtk18">#define</span><span class="mtk6"> TUI_KEY_ENTER   </span><span class="mtk7">10</span></div></div><div class="code-line" data-line-number="5" data-line-start="5" data-line-end="5"><div class="line-content"><span class="mtk18">#define</span><span class="mtk6"> TUI_KEY_UP      </span><span class="mtk7">259</span></div></div><div class="code-line" data-line-number="6" data-line-start="6" data-line-end="6"><div class="line-content"><span class="mtk18">#define</span><span class="mtk6"> TUI_KEY_DOWN    </span><span class="mtk7">258</span></div></div><div class="code-line" data-line-number="7" data-line-start="7" data-line-end="7"><div class="line-content"><span class="mtk18">#define</span><span class="mtk6"> TUI_KEY_ESC     </span><span class="mtk7">27</span></div></div></div></div></div></div></pre>

NOTE

`TUI_KEY_UP` dan `TUI_KEY_DOWN` saat ini menggunakan `KEY_UP`/`KEY_DOWN` dari ncurses. Di taxonomy_display_constant.h, kita pakai nilai integer literal agar surfaces tidak perlu include ncurses.

---

### Contract Layer

#### [NEW] **contract_display_port.h**

Struct `DisplayPort` berisi semua function pointer yang dibutuhkan surfaces:

<pre><div node="[object Object]" class="relative whitespace-pre-wrap word-break-all my-2 rounded-xl bg-muted border"><div class="min-h-7 relative box-border flex flex-row items-center justify-between rounded-t border-b border-border px-2 py-0.5"><div class="font-sans text-sm text-muted-foreground">c</div><div class="flex flex-row gap-2 justify-end"><button aria-label="Copy code" class="appearance-none bg-transparent border-0 p-0 cursor-pointer text-secondary-foreground hover:text-foreground transition-colors"><svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 -960 960 960" fill="currentColor" class="h-3.5 w-3.5"><path d="M362.31-260Q332-260 311-281t-21-51.31V-787.69Q290-818 311-839t51.31-21H697.69Q728-860 749-839t21,51.31v455.38Q770-302 749-281t-51.31,21H362.31Zm0-60H697.69q4.62,0 8.46-3.85t3.85-8.46V-787.69q0-4.62-3.85-8.46T697.69-800H362.31q-4.62,0-8.46,3.85T350-787.69v455.38q0,4.62 3.85,8.46t8.46,3.85Zm-140,200Q192-120 171-141t-21-51.31V-707.69h60v515.38q0,4.62 3.85,8.46t8.46,3.85H617.69v60H222.31ZM350-320q0,0 0-3.85t0-8.46V-787.69q0-4.62 0-8.46t0-3.85q0,0 0,3.85t0,8.46v455.38q0,4.62 0,8.46t0,3.85Z"></path></svg></button></div></div><div class="p-3"><div class="w-full h-full text-xs cursor-text"><div class="code-block"><div class="code-line" data-line-number="1" data-line-start="1" data-line-end="1"><div class="line-content"><span class="mtk6">typedef</span><span class="mtk1"></span><span class="mtk6">struct</span><span class="mtk1"> {</span></div></div><div class="code-line" data-line-number="2" data-line-start="2" data-line-end="2"><div class="line-content"><span class="mtk5">    /* Lifecycle */</span></div></div><div class="code-line" data-line-number="3" data-line-start="3" data-line-end="3"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">clear)(</span><span class="mtk6">void</span><span class="mtk1">);</span></div></div><div class="code-line" data-line-number="4" data-line-start="4" data-line-end="4"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">screen_refresh)(</span><span class="mtk6">void</span><span class="mtk1">);</span></div></div><div class="code-line" data-line-number="5" data-line-start="5" data-line-end="5"><div class="line-content"><span class="mtk1"></span></div></div><div class="code-line" data-line-number="6" data-line-start="6" data-line-end="6"><div class="line-content"><span class="mtk5">    /* Primitif draw (menggantikan attron+mvprintw+attroff) */</span></div></div><div class="code-line" data-line-number="7" data-line-start="7" data-line-end="7"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">draw_at)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">text);</span></div></div><div class="code-line" data-line-number="8" data-line-start="8" data-line-end="8"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">draw_colored)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">int</span><span class="mtk1"> color, </span><span class="mtk6">int</span><span class="mtk1"> bold, </span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">text);</span></div></div><div class="code-line" data-line-number="9" data-line-start="9" data-line-end="9"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">draw_append)(</span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">text);</span><span class="mtk5">                       /* printw */</span></div></div><div class="code-line" data-line-number="10" data-line-start="10" data-line-end="10"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">draw_append_colored)(</span><span class="mtk6">int</span><span class="mtk1"> color, </span><span class="mtk6">int</span><span class="mtk1"> bold, </span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">text);</span><span class="mtk5"> /* attron+printw+attroff */</span></div></div><div class="code-line" data-line-number="11" data-line-start="11" data-line-end="11"><div class="line-content"><span class="mtk1"></span></div></div><div class="code-line" data-line-number="12" data-line-start="12" data-line-end="12"><div class="line-content"><span class="mtk5">    /* Komponen TUI tingkat tinggi */</span></div></div><div class="code-line" data-line-number="13" data-line-start="13" data-line-end="13"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">print_centered_colored)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">text, </span><span class="mtk6">int</span><span class="mtk1"> color, </span><span class="mtk6">int</span><span class="mtk1"> bold);</span></div></div><div class="code-line" data-line-number="14" data-line-start="14" data-line-end="14"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">print_colored)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">text, </span><span class="mtk6">int</span><span class="mtk1"> color);</span></div></div><div class="code-line" data-line-number="15" data-line-start="15" data-line-end="15"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">box)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">int</span><span class="mtk1"> width, </span><span class="mtk6">int</span><span class="mtk1"> height);</span></div></div><div class="code-line" data-line-number="16" data-line-start="16" data-line-end="16"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">box_double)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">int</span><span class="mtk1"> width, </span><span class="mtk6">int</span><span class="mtk1"> height);</span></div></div><div class="code-line" data-line-number="17" data-line-start="17" data-line-end="17"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">separator)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">int</span><span class="mtk1"> width);</span></div></div><div class="code-line" data-line-number="18" data-line-start="18" data-line-end="18"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">separator_thick)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">int</span><span class="mtk1"> width);</span></div></div><div class="code-line" data-line-number="19" data-line-start="19" data-line-end="19"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">highlight_row)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">int</span><span class="mtk1"> width, </span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">text);</span></div></div><div class="code-line" data-line-number="20" data-line-start="20" data-line-end="20"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">normal_row)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">int</span><span class="mtk1"> width, </span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">text);</span></div></div><div class="code-line" data-line-number="21" data-line-start="21" data-line-end="21"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">footer)(</span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">help);</span></div></div><div class="code-line" data-line-number="22" data-line-start="22" data-line-end="22"><div class="line-content"><span class="mtk1"></span><span class="mtk6">void</span><span class="mtk1"> (</span><span class="mtk3">*</span><span class="mtk1">progress_bar)(</span><span class="mtk6">int</span><span class="mtk1"> row, </span><span class="mtk6">int</span><span class="mtk1"> col, </span><span class="mtk6">int</span><span class="mtk1"> width, </span><span class="mtk6">int</span><span class="mtk1"> percent, </span><span class="mtk6">int</span><span class="mtk1"> color);</span></div></div><div class="code-line" data-line-number="23" data-line-start="23" data-line-end="23"><div class="line-content"><span class="mtk1"></span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">(</span><span class="mtk3">*</span><span class="mtk1">medal)(</span><span class="mtk6">int</span><span class="mtk1"> rank);</span></div></div><div class="code-line" data-line-number="24" data-line-start="24" data-line-end="24"><div class="line-content"><span class="mtk1"></span></div></div><div class="code-line" data-line-number="25" data-line-start="25" data-line-end="25"><div class="line-content"><span class="mtk5">    /* Input */</span></div></div><div class="code-line" data-line-number="26" data-line-start="26" data-line-end="26"><div class="line-content"><span class="mtk1"></span><span class="mtk6">int</span><span class="mtk1">  (</span><span class="mtk3">*</span><span class="mtk1">getch)(</span><span class="mtk6">void</span><span class="mtk1">);</span></div></div><div class="code-line" data-line-number="27" data-line-start="27" data-line-end="27"><div class="line-content"><span class="mtk1"></span><span class="mtk6">int</span><span class="mtk1">  (</span><span class="mtk3">*</span><span class="mtk1">confirm)(</span><span class="mtk6">const</span><span class="mtk1"></span><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk3">*</span><span class="mtk1">prompt);</span></div></div><div class="code-line" data-line-number="28" data-line-start="28" data-line-end="28"><div class="line-content"><span class="mtk1">} DisplayPort;</span></div></div></div></div></div></div></pre>

---

### Infrastructure Layer

#### [MODIFY] **infrastructure_tui_adapter.h**

* Hapus `#define COLOR_*` dan `#define TUI_KEY_*` (sudah pindah ke taxonomy)
* Tambah `#include "shared/taxonomy_display_constant.h"`
* Tambah `#include "shared/contract_display_port.h"`
* Tambah deklarasi: `DisplayPort tui_display_port_create(void);`

#### [MODIFY] **infrastructure_tui_adapter.c**

* Tambahkan implementasi `draw_at`, `draw_colored`, `draw_append`, `draw_append_colored`
* Tambahkan `tui_display_port_create()` yang mengisi semua function pointer

---

### Root Layer

#### [NEW] **root_display_container.c**

<pre><div node="[object Object]" class="relative whitespace-pre-wrap word-break-all my-2 rounded-xl bg-muted border"><div class="min-h-7 relative box-border flex flex-row items-center justify-between rounded-t border-b border-border px-2 py-0.5"><div class="font-sans text-sm text-muted-foreground">c</div><div class="flex flex-row gap-2 justify-end"><button aria-label="Copy code" class="appearance-none bg-transparent border-0 p-0 cursor-pointer text-secondary-foreground hover:text-foreground transition-colors"><svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 -960 960 960" fill="currentColor" class="h-3.5 w-3.5"><path d="M362.31-260Q332-260 311-281t-21-51.31V-787.69Q290-818 311-839t51.31-21H697.69Q728-860 749-839t21,51.31v455.38Q770-302 749-281t-51.31,21H362.31Zm0-60H697.69q4.62,0 8.46-3.85t3.85-8.46V-787.69q0-4.62-3.85-8.46T697.69-800H362.31q-4.62,0-8.46,3.85T350-787.69v455.38q0,4.62 3.85,8.46t8.46,3.85Zm-140,200Q192-120 171-141t-21-51.31V-707.69h60v515.38q0,4.62 3.85,8.46t8.46,3.85H617.69v60H222.31ZM350-320q0,0 0-3.85t0-8.46V-787.69q0-4.62 0-8.46t0-3.85q0,0 0,3.85t0,8.46v455.38q0,4.62 0,8.46t0,3.85Z"></path></svg></button></div></div><div class="p-3"><div class="w-full h-full text-xs cursor-text"><div class="code-block"><div class="code-line" data-line-number="1" data-line-start="1" data-line-end="1"><div class="line-content"><span class="mtk1">DisplayPort </span><span class="mtk16">root_display_build</span><span class="mtk1">(</span><span class="mtk6">void</span><span class="mtk1">) {</span></div></div><div class="code-line" data-line-number="2" data-line-start="2" data-line-end="2"><div class="line-content"><span class="mtk1"></span><span class="mtk18">return</span><span class="mtk1"></span><span class="mtk16">tui_display_port_create</span><span class="mtk1">();</span></div></div><div class="code-line" data-line-number="3" data-line-start="3" data-line-end="3"><div class="line-content"><span class="mtk1">}</span></div></div></div></div></div></div></pre>

#### [MODIFY] **root_cli_main_entry.c**

* Tambah `DisplayPort dp = root_display_build();`
* Pass `&dp` ke `cli_surfaces_menu_run()`

---

### Surfaces Layer

#### [MODIFY] **module.cli.h**

* Hapus `#include "tui/infrastructure_tui_adapter.h"`
* Tambah `#include "shared/contract_display_port.h"` dan `#include "shared/taxonomy_display_constant.h"`
* Tambah `DisplayPort *dp` ke semua signature fungsi surfaces

#### [MODIFY] Semua `surfaces_*_command.c` (6 file)

Ganti pattern raw ncurses:

<pre><div node="[object Object]" class="relative whitespace-pre-wrap word-break-all my-2 rounded-xl bg-muted border"><div class="min-h-7 relative box-border flex flex-row items-center justify-between rounded-t border-b border-border px-2 py-0.5"><div class="font-sans text-sm text-muted-foreground">c</div><div class="flex flex-row gap-2 justify-end"><button aria-label="Copy code" class="appearance-none bg-transparent border-0 p-0 cursor-pointer text-secondary-foreground hover:text-foreground transition-colors"><svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 -960 960 960" fill="currentColor" class="h-3.5 w-3.5"><path d="M362.31-260Q332-260 311-281t-21-51.31V-787.69Q290-818 311-839t51.31-21H697.69Q728-860 749-839t21,51.31v455.38Q770-302 749-281t-51.31,21H362.31Zm0-60H697.69q4.62,0 8.46-3.85t3.85-8.46V-787.69q0-4.62-3.85-8.46T697.69-800H362.31q-4.62,0-8.46,3.85T350-787.69v455.38q0,4.62 3.85,8.46t8.46,3.85Zm-140,200Q192-120 171-141t-21-51.31V-707.69h60v515.38q0,4.62 3.85,8.46t8.46,3.85H617.69v60H222.31ZM350-320q0,0 0-3.85t0-8.46V-787.69q0-4.62 0-8.46t0-3.85q0,0 0,3.85t0,8.46v455.38q0,4.62 0,8.46t0,3.85Z"></path></svg></button></div></div><div class="p-3"><div class="w-full h-full text-xs cursor-text"><div class="code-block"><div class="code-line" data-line-number="1" data-line-start="1" data-line-end="1"><div class="line-content"><span class="mtk5">// BEFORE</span></div></div><div class="code-line" data-line-number="2" data-line-start="2" data-line-end="2"><div class="line-content"><span class="mtk16">attron</span><span class="mtk1">(</span><span class="mtk16">COLOR_PAIR</span><span class="mtk1">(COLOR_GOLD) </span><span class="mtk3">|</span><span class="mtk1"> A_BOLD);</span></div></div><div class="code-line" data-line-number="3" data-line-start="3" data-line-end="3"><div class="line-content"><span class="mtk16">mvprintw</span><span class="mtk1">(row, col, </span><span class="mtk12">"teks </span><span class="mtk10">%s</span><span class="mtk12">"</span><span class="mtk1">, nama);</span></div></div><div class="code-line" data-line-number="4" data-line-start="4" data-line-end="4"><div class="line-content"><span class="mtk16">attroff</span><span class="mtk1">(</span><span class="mtk16">COLOR_PAIR</span><span class="mtk1">(COLOR_GOLD) </span><span class="mtk3">|</span><span class="mtk1"> A_BOLD);</span></div></div><div class="code-line" data-line-number="5" data-line-start="5" data-line-end="5"><div class="line-content"><span class="mtk1"></span></div></div><div class="code-line" data-line-number="6" data-line-start="6" data-line-end="6"><div class="line-content"><span class="mtk5">// AFTER</span></div></div><div class="code-line" data-line-number="7" data-line-start="7" data-line-end="7"><div class="line-content"><span class="mtk6">char</span><span class="mtk1"></span><span class="mtk10">buf</span><span class="mtk1">[</span><span class="mtk7">128</span><span class="mtk1">];</span></div></div><div class="code-line" data-line-number="8" data-line-start="8" data-line-end="8"><div class="line-content"><span class="mtk16">snprintf</span><span class="mtk1">(buf, </span><span class="mtk6">sizeof</span><span class="mtk1"></span><span class="mtk10">buf</span><span class="mtk1">, </span><span class="mtk12">"teks </span><span class="mtk10">%s</span><span class="mtk12">"</span><span class="mtk1">, nama);</span></div></div><div class="code-line" data-line-number="9" data-line-start="9" data-line-end="9"><div class="line-content"><span class="mtk1">dp</span><span class="mtk3">-></span><span class="mtk16">draw_colored</span><span class="mtk1">(row, col, COLOR_GOLD, </span><span class="mtk7">1</span><span class="mtk1">, buf);</span></div></div></div></div></div></div></pre>

Dan `refresh()` → `dp->screen_refresh()`.

---

## Open Questions

IMPORTANT

`tui_init()`, `tui_end()`, dan `tui_splash()` **tidak** masuk ke DisplayPort karena dipanggil dari `root_cli_main_entry.c` (layer root boleh import semua layer). Apakah ini sesuai ekspektasi?

IMPORTANT

`surfaces_menu_command.c` menggunakan `attron`/`mvprintw` sebanyak  **39 kali** . Konversi ke `dp->draw_colored()` memerlukan ~39 `snprintf` tambahan. Apakah ada concern performa atau readability?

## Verification Plan

### Build Check

<pre><div node="[object Object]" class="relative whitespace-pre-wrap word-break-all my-2 rounded-xl bg-muted border"><div class="min-h-7 relative box-border flex flex-row items-center justify-between rounded-t border-b border-border px-2 py-0.5"><div class="font-sans text-sm text-muted-foreground">bash</div><div class="flex flex-row gap-2 justify-end"><button aria-label="Copy code" class="appearance-none bg-transparent border-0 p-0 cursor-pointer text-secondary-foreground hover:text-foreground transition-colors"><svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 -960 960 960" fill="currentColor" class="h-3.5 w-3.5"><path d="M362.31-260Q332-260 311-281t-21-51.31V-787.69Q290-818 311-839t51.31-21H697.69Q728-860 749-839t21,51.31v455.38Q770-302 749-281t-51.31,21H362.31Zm0-60H697.69q4.62,0 8.46-3.85t3.85-8.46V-787.69q0-4.62-3.85-8.46T697.69-800H362.31q-4.62,0-8.46,3.85T350-787.69v455.38q0,4.62 3.85,8.46t8.46,3.85Zm-140,200Q192-120 171-141t-21-51.31V-707.69h60v515.38q0,4.62 3.85,8.46t8.46,3.85H617.69v60H222.31ZM350-320q0,0 0-3.85t0-8.46V-787.69q0-4.62 0-8.46t0-3.85q0,0 0,3.85t0,8.46v455.38q0,4.62 0,8.46t0,3.85Z"></path></svg></button></div></div><div class="p-3"><div class="w-full h-full text-xs cursor-text"><div class="code-block"><div class="code-line" data-line-number="1" data-line-start="1" data-line-end="1"><div class="line-content"><span class="mtk16">make</span><span class="mtk1"></span><span class="mtk12">clean</span><span class="mtk1"> && </span><span class="mtk16">make</span></div></div></div></div></div></div></pre>

### Manual Verification

* Jalankan program, pastikan semua layar tampil normal
* Cek tidak ada `#include "tui/infrastructure_tui_adapter.h"` di surfaces
* Cek `grep -r 'infrastructure_tui_adapter' src/cli/` → empty
