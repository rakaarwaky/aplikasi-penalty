# Saran UI/UX yang DITERIMA (Valid & Belum Dikerjakan)

Dokumen ini berisi saran yang layak diimplementasikan. Saran yang sudah ada di
working tree / sudah dikerjakan hari ini DITANDINGI sebagai REDUNDAN dan tidak
dimasukkan agar tidak ada pekerjaan ganda.

---

## A. Navigasi & Feedback (dari saran/1.md)

### A1. Sinkronkan footer navigasi dengan kode  [VALID]
Footer menu menulis "Navigasi: [v/^]" tapi `cli_surfaces_menu_run` hanya menangani
panah (KEY_UP/KEY_DOWN). Pengguna yang menekan 'v'/'^' tidak mendapat respons.
- **Fix:** Ubah footer jadi "Navigasi: [^/v] (panah)" di `surfaces_menu_command.c`
  (paling konsisten lintas terminal), atau tambahkan case 'v'/'^' di switch.
- **File:** `src/cli/surfaces_menu_command.c`

### A2. Konfirmasi keluar (ESC / pilihan 0)  [VALID — sudah direncanakan, belum pasang]
ESC dan pilihan 0 langsung keluar tanpa konfirmasi → risiko sesi terputus tak sengaja.
- **Fix:** Pasang `tui_confirm("Keluar dari aplikasi?")` pada ESC dan case '0'.
- **File:** `src/cli/surfaces_menu_command.c` (helper `tui_confirm` sudah ada di adapter)

### A3. Error scoring tanpa clear layar  [VALID]
Di scoring, zona salah memicu `tui_clear()` lalu gambar ulang → banyak kedip.
- **Fix:** Hapus `tui_clear()` di cabang error; cukup perbarui baris feedback
  via `draw_scoring_screen(part, kick_num, msg, 1)`.
- **File:** `src/cli/surfaces_scoring_command.c`

### A4. Highlight baris terpilih lebih kontras  [VALID — kecil]
`tui_highlight_row` pakai `COLOR_HIGHLIGHT` tanpa bold → di terminal 16-warna
sulit dibedakan.
- **Fix:** Tambah `A_BOLD` di `tui_highlight_row`.
- **File:** `src/tui/infrastructure_tui_adapter.c`

---

## B. Visualisasi Data (dari saran/2.md)

### B1. Bar chart skor di layar ranking  [VALID]
Ranking masih tabel angka polos. Bandingkan skor langsung dengan bar.
- **Fix:** Setelah kolom skor, panggil `tui_progress_bar(row, col, 20,
  score*100/max_score, COLOR_SUCCESS)`.
- **File:** `src/cli/surfaces_ranking_command.c`

### B2. Warna zona di riwayat ranking/recap  [VALID]
Riwayat zona di scoring sudah diwarnai, tapi frekuensi zona di ranking/recap
masih putih polos.
- **Fix:** Cetak zona 5 dengan `COLOR_SUCCESS|A_BOLD`, zona 0 dengan
  `COLOR_ERROR`, sisanya `COLOR_MENU`.
- **File:** `src/cli/surfaces_ranking_command.c`, `src/cli/surfaces_recap_command.c`

### B3. Zebra striping tabel recap  [VALID]
Tabel recap (hingga 7 baris) sulit dibaca mata.
- **Fix:** Beri latar `COLOR_DIM` pada baris genap, atau gandakan `tui_separator`
  antar peserta.
- **File:** `src/cli/surfaces_recap_command.c`

---

## C. Onboarding & Aksesibilitas (dari saran/3.md)

### C1. Layar "Cara Bermain"  [VALID]
Pengguna baru tidak tahu aturan (zona=poin, 7 tendangan, 5-7 peserta).
- **Fix:** Tombol 'h' di menu utama membuka layar bantuan (aturan + navigasi)
  pakai `tui_box` + teks. Bisa juga ditampilkan sekali setelah splash.
- **File:** `src/cli/surfaces_menu_command.c` (fungsi bantu baru)

### C2. Label "minimal 5 peserta" permanen di pendaftaran  [VALID]
User baru tidak tahu kuota minimal sampai error muncul.
- **Fix:** Tampilkan "Minimal 5, maksimal 7 peserta" permanen di layar
  pendaftaran (progress bar kuota sudah ada).
- **File:** `src/cli/surfaces_registration_command.c`

### C3. Shortcut angka 1-5 di menu  [VALID]
Navigasi lebih cepat dengan menekan angka langsung.
- **Fix:** Tambah case '1'..'5' di switch menu yang set `selected` lalu jalankan
  aksi sama seperti ENTER.
- **File:** `src/cli/surfaces_menu_command.c`

---

## D. Polish & Robustness (dari saran/4.md)

### D1. Error lebih menonjol (bold)  [VALID]
Pesan error pakai `COLOR_ERROR` saja; kasus fatal kurang menonjol.
- **Fix:** Di `show_error` (registration) dan pesan error scoring, gunakan
  `COLOR_PAIR(COLOR_ERROR) | A_BOLD`.
- **File:** `src/cli/surfaces_registration_command.c`, `src/cli/surfaces_scoring_command.c`

### D2. Ringkasan juara sebelum keluar  [VALID]
Setelah `tui_end()`, layar ncurses hilang; user kehilangan hasil terakhir.
- **Fix:** Sebelum `tui_end()`, tampilkan juara 1 di layar ncurses, tunggu Enter,
  baru akhiri.
- **File:** `src/root_cli_main_entry.c`

### D3. Animasi transisi (nice-to-have, low priority)  [VALID — optional]
Pindah layar langsung `tui_clear()`. Bisa lebih halus.
- **Fix:** Opsional — `tui_box_double` atau `napms(50)` saat transisi. Hindari
  terlalu lambat agar tidak mengganggu.
- **File:** semua `surfaces_*_command.c`

---

## YANG DITOLAK / REDUNDAN (sudah ada, jangan dikerjakan lagi)

- Splash screen animasi + loading           → SUDAH di `tui_splash()`
- Status bar fase lomba di menu             → SUDAH di `draw_menu`
- Progress bar kuota & tendangan global      → SUDAH di menu & registration
- Footer bantuan konsisten                   → SUDAH di `tui_footer()`
- Medali juara 1/2/3 di ranking & recap      → SUDAH di `tui_medal()`
- Panel "Juara 1" di menu utama              → SUDAH dikerjakan sesi ini
- Ringkasan statistik di recap               → SUDAH dikerjakan sesi ini
- Fix splash Enter bocor ke menu             → SUDAH di-fix (`flushinp()`) sesi ini
- Footer separator COLOR_DIM konsisten       → SUDAH dirapikan sesi ini
