# PRD: Aplikasi Pengelola Lomba Tendangan Penalti

## 1. Deskripsi Produk

Aplikasi console berbasis bahasa C untuk mengelola pelaksanaan lomba tendangan penalti secara terstruktur. Aplikasi ini menangani pendaftaran peserta, pencatatan hasil tendangan, perhitungan skor otomatis, penentuan peringkat juara dengan aturan tie-breaker, pencarian data peserta, serta pencetakan rekapitulasi hasil lomba.

## 2. Feature Breakdown

### 2.1 Registration - Pendaftaran Peserta

**Deskripsi:** Mengelola input dan penyimpanan data identitas peserta sebelum kompetisi dimulai.
**Aturan Bisnis:**

- Sistem hanya menerima jumlah peserta minimal 5 dan maksimal 7 orang.
- Setiap peserta harus memiliki nama yang unik dan tidak boleh kosong.
- Pendaftaran harus diselesaikan secara penuh sebelum sesi tendangan dimulai.
  **Validasi Nama:**
- Panjang maksimal 30 karakter.
- Hanya menerima huruf alfabet dan spasi, menolak angka dan simbol.
- Uniqueness bersifat case-insensitive sehingga nama Ali dan ali dianggap sama dan akan ditolak.
  **Kebutuhan Data:**
- Input: Nama peserta bertipe string.
- Output: Konfirmasi pendaftaran atau pesan error jika kuota tidak terpenuhi atau nama tidak valid.

### 2.2 Scoring - Pencatatan dan Kalkulasi Tendangan

**Deskripsi:** Merekam hasil tendangan setiap peserta dan menghitung akumulasi skor.
**Aturan Bisnis:**

- Setiap peserta wajib melakukan tepat 7 kali tendangan.
- Input berupa angka zona sasaran, yaitu integer 0 sampai 5.
- Sistem menolak input di luar rentang 0 sampai 5 dan meminta input ulang.
  **Tabel Konversi Zona ke Poin:**
- Zona 0 bernilai 0 poin
- Zona 1 bernilai 1 poin
- Zona 2 bernilai 2 poin
- Zona 3 bernilai 3 poin
- Zona 4 bernilai 4 poin
- Zona 5 bernilai 5 poin
  Sistem mengakumulasikan poin tersebut menjadi total skor per peserta.
  **Kebutuhan Data:**
- Input: ID peserta dan angka zona per tendangan.
- State: Array riwayat 7 tendangan per peserta dan total skor sementara.

### 2.3 Ranking - Penentuan Peringkat dan Tie-Breaker

**Deskripsi:** Mengurutkan peserta dari skor tertinggi ke terendah dengan logika tie-breaker bertingkat.
**Aturan Bisnis:**

- Urutan utama: total skor tertinggi secara descending.
- Aturan tie-breaker jika total skor sama:
  1. Peserta dengan jumlah tendangan masuk zona 5 terbanyak berada di atas.
  2. Jika masih sama, lihat jumlah zona 4 terbanyak.
  3. Jika masih sama, lihat zona 3, lalu zona 2, lalu zona 1.
  4. Jika seluruh frekuensi zona identik, peserta dinyatakan peringkat seri.
     **Kriteria Output Ranking:**
- Menampilkan seluruh peserta dari peringkat tertinggi hingga terendah.
- Penomoran peringkat seri: Jika dua peserta seri di posisi pertama, keduanya mendapat label Peringkat 1, dan peserta berikutnya mendapat label Peringkat 3.
  **Kebutuhan Data:**
- Input: Kumpulan data peserta beserta riwayat 7 zonanya.
- Output: Daftar peserta yang sudah diurutkan beserta atribut peringkat.

### 2.4 Search - Pencarian Data Peserta

**Deskripsi:** Memungkinkan pencarian detail performa peserta tertentu.
**Aturan Bisnis:**

- Pencarian berbasis nama dengan sifat case-insensitive.
- Menampilkan rekapitulasi individu: nama, rincian 7 zona tendangan, total skor.
- Menampilkan pesan khusus jika peserta tidak ditemukan.

### 2.5 Recap - Rekapitulasi Lomba

**Deskripsi:** Menampilkan ringkasan lengkap seluruh hasil kompetisi.
**Aturan Bisnis:**

- Menampilkan tabel lengkap: peringkat, nama, total skor, rincian frekuensi zona.
- Format tampilan rapi menggunakan karakter ASCII untuk border tabel agar mudah dibaca di console.

## 3. Struktur Menu dan Navigasi

Aplikasi memiliki Menu Utama dengan opsi berikut:

0. Keluar
1. Pendaftaran Peserta
2. Input Tendangan dan Skor
3. Tampilkan Ranking
4. Cari Peserta
5. Rekapitulasi Lengkap

**Flow Navigasi:**
Setelah setiap submenu selesai dieksekusi, sistem akan menampilkan pesan tekan Enter untuk melanjutkan dan mengembalikan user ke Menu Utama.

## 4. Urutan Proses dan State Machine

Sistem menerapkan state machine untuk mencegah akses fitur yang belum waktunya:

- **State Init:** Baru dijalankan. Hanya menu 1 yang aktif. Menu lain menampilkan pesan silahkan daftar peserta terlebih dahulu.
- **State Registered:** Peserta sudah terdaftar. Menu 1 dan 2 aktif.
- **State Completed:** Semua peserta telah menyelesaikan 7 tendangan. Seluruh menu aktif.
  Jika user memilih menu Ranking atau Recap sebelum semua peserta selesai melakukan tendangan, sistem menampilkan pesan peringatan dan menolak akses.

## 5. Penanganan Error Global

- **Input Salah Tipe:** Jika user memasukkan huruf pada menu yang membutuhkan angka, sistem akan membersihkan buffer input standar dan meminta ulang tanpa membuat program crash.
- **Validasi Zona:** Menolak angka di luar 0 sampai 5 dan menampilkan pesan zona tidak valid.
- **Interrupt:** Program berjalan secara sinkron dan tidak menyimpan state ke file, sehingga interupsi paksa akan menghanguskan data di memori.

## 6. Batasan Teknis dan Spesifikasi

- **Standar Bahasa:** C99.
- **Platform Target:** Cross-platform terminal Windows dan Linux.
- **Library Standar yang Diizinkan:** stdio.h, stdlib.h, string.h, ctype.h.
- **Library Modern yang Diizinkan:** ncurses.h (TUI rendering, color, window management), panel.h (layered windows via ncurses), form.h (form input via ncurses).
- **Persistensi:** Data hanya disimpan di memori RAM selama program berjalan.

## 7. Skenario Penggunaan

1. User menjalankan aplikasi dan melihat header Aplikasi Lomba Tendangan Penalti.
2. User memilih menu 1, memasukkan data 5 sampai 7 peserta. Sistem memvalidasi nama.
3. User memilih menu 2, memasukkan 7 angka zona tendangan satu per satu untuk setiap peserta. Sistem menolak input invalid.
4. Setelah semua data masuk, user memilih menu 3. Sistem menampilkan daftar juara dengan penomoran seri yang tepat.
5. User dapat memilih menu 4, mengetikkan nama peserta, dan sistem menampilkan detail 7 tendangan beserta total skornya.
6. User memilih menu 5 untuk melihat tabel rekapitulasi berborder ASCII.
7. User memilih menu 0 untuk keluar dari program.
