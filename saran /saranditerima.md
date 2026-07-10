# Saran UI/UX yang DITERIMA (Belum Diterapkan)

Dari 4 file saran (1.md - 4.md), berikut saran yang **belum diterapkan** dan layak diimplementasikan:

---

## 1. Konfirmasi Aksi Berbahaya (Destructive Action Confirmation)

**Sumber:** 1.md §6, 2.md §6, 3.md §2

Aksi seperti keluar program atau reset data harus minta konfirmasi user.

```
Apakah Anda yakin ingin keluar? (y/n):
```

**Status saat ini:** Belum ada konfirmasi saat keluar (ESC/0 langsung exit).

---

## 2. Input Format & Contoh (Input Guidance)

**Sumber:** 1.md §2, 2.md §2

Tampilkan contoh format input yang benar saat meminta input dari user.

```
Nama peserta (max 30, contoh: "Budi Santoso"):
```

**Status saat ini:** Prompt sudah ada tapi belum ada contoh format.

---

## 3. Error Message Lebih Spesifik + Recovery

**Sumber:** 1.md §2, 2.md §3

Saat input salah, beri tahu APA yang salah dan BAGAIMANA cara memperbaiki.

```
"Zona harus 0-5. Anda memasukkan '7'. Silakan coba lagi."
```

**Status saat ini:** Pesan error sudah jelas, tapi belum ada info nilai yang dimasukkan user.

---

## 4. Tabel dengan Box-Drawing Characters

**Sumber:** 1.md §4, 3.md §1

Gunakan karakter box-drawing (┌─┬─┐) untuk tabel yang lebih rapi dan profesional.

```
┌─────┬──────────────┬──────┐
│ No  │ Nama         │ Skor │
├─────┼──────────────┼──────┤
│  1  │ Budi         │   12 │
└─────┴──────────────┴──────┘
```

**Status saat ini:** Ranking dan rekap pakai garis ACS (│, ─, ├), tapi tabel pencarian belum.

---

## 5. Keyboard Shortcuts Tambahan

**Sumber:** 1.md §7, 2.md §7

Tekan `q` untuk keluar dari mana saja, `b` untuk kembali, dll.
**Status saat ini:** Hanya navigasi panah + Enter + ESC + '0'.

---

## 6. Auto-Save / Persistensi Data

**Sumber:** 2.md §7, 4.md §4

Data otomatis tersimpan ke file saat program ditutup atau crash.
**Status saat ini:** Fitur storage/load sudah ada di codebase tapi belum terkoneksi ke UI.

---

## 7. Fuzzy Search (Lebih Toleran)

**Sumber:** 2.md §9, 4.md §4

Selain case-insensitive, bisa cari dengan parsial dan tampilkan saran jika tidak ketemu.

```
Tidak ada peserta 'Budi'. Mungkin maksud Anda: 'Budi Santoso', 'Budiman'?
```

**Status saat ini:** Search sudah case-insensitive tapi belum fuzzy/suggestive.

---

## 8. Tie-Breaker Transparan

**Sumber:** 2.md §8, 3.md §3, 4.md §5

Saat ranking seri, tampilkan ALASAN mengapa posisi A di atas B.

```
Seri! Budi & Andi sama-sama 12 poin.
Budi juara karena: zona 5 lebih banyak (3 vs 1)
```

**Status saat ini:** Tie-breaker sudah dihitung (zone_freq) tapi penjelasan belum ditampilkan.

---

## 9. Loading/Progress Indicator untuk Proses Berat

**Sumber:** 1.md §3, 4.md §2

Tampilkan progress saat menghitung peringkat atau ekspor data.
**Status saat ini:** Proses ranking/recap instan (data kecil), tapi baik untuk scalable.

---

## 10. Opsi Export/Print Hasil

**Sumber:** 2.md §10, 3.md §3, 4.md §5

Rekapitulasi bisa di-export ke file .txt atau .csv untuk dicetak.
**Status saat ini:** Fitur export sudah ada di codebase (infrastructure_export_adapter) tapi belum terkoneksi ke menu UI.

---

## 11. Breadcrumb / Path Navigasi

**Sumber:** 4.md §1

Tampilkan posisi user saat ini di layar.

```
Menu Utama > Pendaftaran
```

**Status saat ini:** Judul layar sudah ada tapi belum ada breadcrumb path.

---

## 12. Menu Layout Grid (2 Kolom)

**Sumber:** 4.md §1 (Contoh benchmark)

Menu utama bisa ditampilkan dalam format grid 2 kolom agar lebih compact.
**Status saat ini:** Menu masih single column.

---

*Dokumen ini dihasilkan dari analisis file saran/1.md - saran/4.md*
*Tanggal: 10 Juli 2026*
