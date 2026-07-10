#!/usr/bin/env python3
"""Generate Laporan-Project.docx for aplikasi-penalty (matches Project_Task.pdf criteria)."""
from docx import Document
from docx.shared import Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH

doc = Document()

# Base font
style = doc.styles['Normal']
style.font.name = 'Calibri'
style.font.size = Pt(11)

def h1(t):
    p = doc.add_heading(t, level=1)
    return p

def h2(t):
    return doc.add_heading(t, level=2)

def para(t):
    return doc.add_paragraph(t)

def bullet(t):
    return doc.add_paragraph(t, style='List Bullet')

def code(t):
    p = doc.add_paragraph()
    r = p.add_run(t)
    r.font.name = 'Consolas'
    r.font.size = Pt(9)
    return p

# ---- Title ----
title = doc.add_heading('LAPORAN PROJECT', level=0)
sub = doc.add_paragraph('Aplikasi Perhitungan Hasil Lomba Tendangan Penalti')
sub.runs[0].bold = True
sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
meta = doc.add_paragraph('Bahasa C — Arsitektur Hexagonal (Ports & Adapters)')
meta.alignment = WD_ALIGN_PARAGRAPH.CENTER
doc.add_paragraph()

# ---- 1. Analisis Permasalahan (kerangka filsafat masalah, gaya akademis) ----
h1('1. Analisis Permasalahan')
para('Berdasarkan kerangka epistemologis filsafat masalah, suatu kondisi hanya dapat '
     'disebut "masalah" apabila memenuhi tiga prasyarat sekaligus: (a) terdapat subjek '
     'yang memiliki tujuan; (b) terdapat kesenjangan (gap) antara keadaan aktual dan '
     'keadaan yang dikehendaki; serta (c) terdapat hambatan yang tidak dapat diatasi '
     'secara langsung. Dalam laporan ini, tujuan dan norma tidak dibentuk sendiri, '
     'melainkan ditetapkan secara eksplisit oleh dokumen tugas (Project_Task.pdf) — '
     'sehingga masalah diturunkan langsung dari requirement tersebut, bukan dari asumsi.')

h2('1.1 Tujuan (ditetapkan Project_Task.pdf)')
para('Subjek dalam konteks ini adalah penyusun program yang diwajibkan memenuhi '
     'spesifikasi dokumen tugas. Tujuan diambil verbatim dari dokumen tugas, bukan '
     'dirumuskan sendiri:')
bullet('Membangun program bahasa C untuk mengelola hasil lomba tendangan penalti.')
bullet('Jumlah peserta 5-7 orang; setiap peserta 7 tendangan; zona bernilai 0 sampai 5.')
bullet('Mengubah zona menjadi skor; total skor = jumlah seluruh skor tendangan.')
bullet('Menentukan juara dari total skor tertinggi, dengan aturan seri 5 -> 4 -> 3 -> 2 -> 1.')
bullet('Menyimpan data peserta, mencatat hasil tendangan, mencari peserta, menampilkan '
       'rekap, dan mengurutkan ranking.')

h2('1.2 Keadaan Saat Ini dan Keadaan Diinginkan (Norma)')
para('Keadaan saat ini: belum tersedia program yang memenuhi seluruh ketentuan di atas. '
     'Keadaan diinginkan (norma dari PDF): program yang secara utuh memenuhi batas '
     'peserta (5-7), batas tendangan (7), rentang zona (0-5), aturan konversi dan '
     'akumulasi skor, aturan seri bertingkat, serta kelima kemampuan kelola data. '
     'Kesenjangan (gap) antara kedua keadaan inilah yang menjadikan tugas ini sebagai masalah.')

h2('1.3 Hambatan (Diturunkan dari Requirement PDF)')
para('Berikut hambatan yang diturunkan dari requirement Project_Task.pdf, terbagi menjadi '
     'batasan (constraint) dan kebutuhan fungsional yang wajib diimplementasi:')

para('Batasan (constraint) — pembatas yang melahirkan obstacle bila dilanggar:')
bullet('Masalah 1 - Pembatasan input zona 0-5 (Ketentuan 2 & 4). Tanpa pembatasan, '
       'pengguna dapat memasukkan nilai di luar rentang yang merusak perhitungan, '
       'sehingga melanggar spesifikasi zona.')
bullet('Masalah 2 - Batas jumlah peserta 5-7 (aturan lomba). Data peserta harus '
       'dialokasikan menampung maksimal 7 tanpa meluap, namun tetap menerima minimal 5.')

para('Kebutuhan fungsional — kewajiban spesifikasi yang harus dipenuhi agar tujuan tercapai:')
bullet('Masalah 3 - Konversi zona ke skor dan akumulasi (Ketentuan 3 & 4). Setiap zona '
       'harus dipetakan ke poin, lalu dijumlahkan menjadi total skor tiap peserta.')
bullet('Masalah 4 - Penentuan juara dan aturan seri bertingkat (Ketentuan 5 & 6). '
       'Pengurutan tidak cukup hanya by total skor; diperlukan pemecah seri 5 -> 4 -> 3 '
       '-> 2 -> 1, dan peringkat sama bila seluruh komponen identik.')
bullet('Masalah 5 - Kelola data antar-fitur (simpan, catat, cari, rekap, ranking). '
       'Seluruh fitur harus beroperasi pada satu kesatuan data peserta yang konsisten.')

h2('1.4 Pembedaan Problem yang Dipresentasikan dan Problem Sesungguhnya')
para('Problem yang dipresentasikan (presented problem): kesulitan nyata yang tampak saat '
     'menyusun program — membatasi input, mengurutkan peringkat, menjaga batas data. '
     'Problem sesungguhnya (real problem / akar masalah): ketiadaan lapisan validasi '
     'input, ketiadaan logika pemecah seri bertingkat, dan ketiadaan struktur data '
     'tunggal yang diakses semua fitur. Analisis ini menempatkan gejala di permukaan dan '
     'akar masalah di dasar, sehingga penyelesaian menyasar penyebab, bukan sekadar simptom.')

# ---- 2. Skenario Program ----
h1('2. Skenario Program')
para('Alur penggunaan aplikasi dalam satu sesi:')
bullet('Menu utama menampilkan 6 fitur + 1 fitur simpan/muat, dengan status tiap '
       'fitur (Aktif / Terkunci / Selesai) sesuai tahap lomba.')
bullet('Tahap 1 — Pendaftaran: pengguna memasukkan nama peserta (5–7). Tombol '
       'peserta ke-8 ditolak; nama kosong mengakhiri pendaftaran.')
bullet('Tahap 2 — Input Tendangan & Skor: untuk tiap peserta, masukkan 7 zona (0–5). '
       'Zona divalidasi; total skor diakumulasi otomatis.')
bullet('Tahap 3 — Ranking & Rekap: setelah semua peserta selesai, pengguna dapat '
       'melihat peringkat (dengan aturan seri) dan rekapitulasi lengkap.')
bullet('Fitur Cari Peserta: mencari peserta berdasarkan nama (cocok persis).')
bullet('Fitur Simpan / Muat Data: menyimpan seluruh lomba ke file data_lomba.bin, '
       'memuatnya kembali saat startup, menghapus file, atau me-reset lomba dari memori.')
para('Verifikasi alur penuh (daftar → tendang → ranking → cari → rekap) telah '
     'ditutupi oleh test otomatis test_full_game_via_surfaces.')

# ---- 3. Konstruksi Program ----
h1('3. Konstruksi Program')
para('Arsitektur yang digunakan adalah Hexagonal (Ports & Adapters): aturan bisnis '
     'berada di domain (registration, scoring, ranking, search, recap, storage, '
     'sanitizer), antarmuka (CLI/TUI) hanya berkomunikasi lewat kontrak function '
     'pointer (DisplayPort, StorageProtocol, dsb). Hal ini memungkinkan pengujian '
     'tanpa terminal nyata (fake DisplayPort).')
para('Alur data utama (tanpa diagram bitmap):')
code('main()\n'
     ' └─ CompetitionState state   (satu-satunya wadah data, di-pass via pointer)\n'
     ' ├─ root_*_build()           (rakit tiap aggregate domain + DisplayPort)\n'
     ' ├─ agent_storage_load()     (auto-muat data_lomba.bin bila ada)\n'
     ' └─ cli_surfaces_menu_run()  (event loop menu)\n'
     '       ├─ [1] Registration → agent_registration_orchestrator\n'
     '       ├─ [2] Scoring       → agent_scoring_orchestrator\n'
     '       ├─ [3] Ranking       → capabilities_ranking_calculator\n'
     '       ├─ [4] Search        → capabilities_search_resolver\n'
     '       ├─ [5] Recap         → agent_recap_orchestrator\n'
     '       └─ [6] Storage       → agent_storage_save / load / delete')

# ---- 4. Struktur (struct) ----
h1('4. Struktur (struct)')
para('Struct utama (diekstrak dari source):')
h2('CompetitionState (wadah status lomba)')
code('typedef struct {\n'
     '    ParticipantEntity     participants[MAX_PARTICIPANTS];\n'
     '    int                   participant_count;\n'
     '    CompetitionStateKind  state;   /* INIT | REGISTERED | COMPLETED */\n'
     '} CompetitionState;')
h2('ParticipantEntity (satu peserta)')
code('typedef struct {\n'
     '    ParticipantIdVO   id;          /* nomor urut */\n'
     '    ParticipantNameVO name;        /* nama (dibungkus VO) */\n'
     '    KickVO            kicks[TOTAL_KICKS]; /* 7 hasil tendangan */\n'
     '    TotalScoreVO      total_score; /* akumulasi poin */\n'
     '    ZoneFreqVO        zone_freq;   /* hitungan tiap zona (pemecah seri) */\n'
     '    KickCountVO       kick_count;  /* 0..7 tendangan dilakukan */\n'
     '} ParticipantEntity;')
h2('CompetitionStateKind (enum tahap)')
code('typedef enum {\n'
     '    STATE_INIT = 0,       /* belum ada peserta */\n'
     '    STATE_REGISTERED = 1, /* boleh tendang & cari */\n'
     '    STATE_COMPLETED = 2   /* boleh ranking & recap */\n'
     '} CompetitionStateKind;')

# ---- 5. Konstanta ----
h1('5. Konstanta')
para('Konstanta terpusat di taxonomy_game_constant.h:')
code('MIN_PARTICIPANTS 5     /* peserta minimal */\n'
     'MAX_PARTICIPANTS 7     /* peserta maksimal (batas array) */\n'
     'TOTAL_KICKS      7     /* tendangan per peserta */\n'
     'MIN_ZONE         0     /* zona terendah (miss) */\n'
     'MAX_ZONE         5     /* zona tertinggi (top) */\n'
     'MAX_NAME_LENGTH  30    /* panjang nama maksimal */\n'
     'DEFAULT_STORAGE_FILENAME "data_lomba.bin"')

# ---- 6. Variabel ----
h1('6. Variabel')
para('Aplikasi sengaja TIDAK menggunakan variabel global untuk data lomba. '
     'Variabel utama:')
bullet('CompetitionState state — dideklarasikan di main(), di-pass ke seluruh fitur '
       'via pointer (satu sumber data).')
bullet('Aggregate lokal di main(): reg, sc, rk, sr, rc, st, sn — masing-masing '
       'berisi function pointer ke implementasi domain.')
bullet('DisplayPort dp — antarmuka render, dirakit oleh root_display_build().')
bullet('Variabel lokal di tiap fungsi: loop index (i, z), buffer (buf[128]), '
       'state sementara (RankingEntryVO entries[]).')

# ---- 7. Fungsi ----
h1('7. Daftar Fungsi')
para('Fungsi domain & infrastruktur utama:')
bullet('root_registration_build / agent_registration_append (pendaftaran peserta)')
bullet('capabilities_scoring_validate_zone / capabilities_scoring_record_kick (input tendangan & skor)')
bullet('capabilities_ranking_compute (urutkan + aturan seri zona 5→4→3→2→1)')
bullet('capabilities_search_resolver (cari peserta)')
bullet('agent_recap_orchestrator / capabilities_recap_formatter (rekapitulasi)')
bullet('agent_storage_save / agent_storage_load / agent_storage_delete (simpan/muat/hapus file)')
bullet('sanitizer_validate_int / sanitizer_validate_string (validasi input)')
bullet('cli_surfaces_menu_run + cli_surfaces_*_execute (navigasi & layar tiap fitur)')
bullet('root_display_build (rakit DisplayPort / antarmuka ncurses)')
bullet('cli_surfaces_storage_execute (fitur Simpan/Muat/Reset baru)')

# ---- 8. Kode Sumber ----
h1('8. Kode Sumber (Script Program)')
para('Kode sumber lengkap terdapat di folder src/ (41 file .c/.h) dengan struktur:')
bullet('src/shared/        — konstanta, struct (VO), enum, kontrak antarmuka')
bullet('src/registration/  — pendaftaran peserta')
bullet('src/scoring/       — validasi & pencatatan zona → skor')
bullet('src/ranking/       — peringkat + aturan seri')
bullet('src/search/        — pencarian peserta')
bullet('src/recap/         — rekapitulasi')
bullet('src/storage/       — simpan/muat/hapus file')
bullet('src/sanitizer/     — validasi input')
bullet('src/cli/           — layar menu & tiap fitur (command + page)')
bullet('src/tui/           — adaptor ncurses (DisplayPort)')
bullet('root_cli_main_entry.c — titik masuk program')
para('Cuplikan fungsi inti — aturan seri (ranking):')
code('static int compare_entries(const void *a, const void *b) {\n'
     '    const RankingEntryVO *x = a, *y = b;\n'
     '    if (x->total_score != y->total_score)\n'
     '        return y->total_score - x->total_score;\n'
     '    for (int z = MAX_ZONE; z >= 1; z--)        /* 5,4,3,2,1 */\n'
     '        if (x->zone_freq[z] != y->zone_freq[z])\n'
     '            return y->zone_freq[z] - x->zone_freq[z];\n'
     '    return 0;                                  /* seri sempurna */\n'
     '}')
para('Kompilasi & pengujian: make (build) dan make test (semua test lolos).')

# ---- Header identitas: HANYA halaman pertama ----
# Tidak dibentuk sendiri; diambil dari identitas penyusun (template tugas).
from docx.oxml.ns import qn

sec = doc.sections[0]
sectPr = sec._sectPr

# Aktifkan "Different First Page"
titlePg = sectPr.find(qn('w:titlePg'))
if titlePg is None:
    titlePg = sectPr.makeelement(qn('w:titlePg'), {})
    sectPr.append(titlePg)
titlePg.set(qn('w:val'), 'true')

# Isi first-page header dengan identitas
first_hdr = sec.first_page_header
first_hdr.is_linked_to_previous = False
# Hapus isi lama (jika ada)
for p in list(first_hdr.paragraphs):
    p._p.getparent().remove(p._p)

identity = [
    'Nama        : Raka Arwaky',
    'Nim         : 22342030',
    'Mata Kuliah : Pengantar Coding',
    'Sesi        : 863',
]
for line in identity:
    hp = first_hdr.add_paragraph()
    hp.add_run(line)

# Pastikan tidak ada default header (biar halaman 2+ kosong)
sec.header.is_linked_to_previous = True

doc.save('Laporan-Project.docx')
print('SAVED Laporan-Project.docx')
