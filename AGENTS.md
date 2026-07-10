# AGENTS.md: Project Overview — C-AES Penalty Shootout

Dokumen ini adalah **ringkasan navigasi** untuk AI agent dan developer.
Jangan baca dokumen ini sebagai sumber kebenaran teknis — baca dokumen rujukan di bawah untuk detail lengkap.

---

## 1. Tentang Proyek Ini

Aplikasi console bahasa **C (C99)** untuk mengelola lomba tendangan penalti.
Fitur utama: pendaftaran peserta, pencatatan tendangan, kalkulasi skor, penentuan ranking dengan tie-breaker, pencarian peserta, dan rekapitulasi hasil.

> **Baca selengkapnya:** [`PRD.md`](PRD.md) — requirement lengkap, aturan bisnis, state machine, batasan teknis, dan library yang diizinkan.

---

## 2. Arsitektur

Proyek menggunakan **AES v3.0** (Agentic Engineering System) yang diadaptasi ke C, dengan pendekatan *feature-based vertical slicing* dan 7 lapisan terstruktur.

**Arah dependency (bottom-up):**

```
taxonomy → contract → capabilities / infrastructure → agent → surface → root (wiring only)
```

> **Catatan:** Urutan di atas adalah **arah dependency**, bukan urutan eksekusi atau hierarki kontrol.
>
> - `root_*_entry.c` adalah titik masuk eksekusi yang berada di puncak kode (dipanggil pertama oleh OS).
> - `root_*_container.c` ada di setiap folder fitur sebagai tempat perakitan (wiring) layer — bukan lapisan domain.
> - Layer `root` **tidak berisi** business logic apa pun; ia hanya merakit dan menghubungkan semua layer di atasnya.

Struktur direktori `src/`:

```text
src/
├── shared/          ← tipe dan kontrak lintas fitur
├── registration/    ← fitur pendaftaran peserta
├── scoring/         ← fitur pencatatan dan kalkulasi tendangan
├── ranking/         ← fitur penentuan peringkat dan tie-breaker
├── search/          ← fitur pencarian peserta
├── recap/           ← fitur rekapitulasi hasil lomba
└── root_cli_main_entry.c
```

> **Baca selengkapnya:** [`ARCHITECTURE.md`](ARCHITECTURE.md) — definisi lengkap setiap layer, filosofi penamaan 3-struktur, diagram dependency, dan contoh migrasi Before/After.

---

## 3. Aturan AES

Semua kode **wajib** mematuhi aturan AES yang mencakup:

| Kelompok | Kode Aturan    | Topik                                                               |
| -------- | -------------- | ------------------------------------------------------------------- |
| Naming   | AES101, AES102 | Konvensi nama file dan suffix per layer                             |
| Import   | AES201–AES205 | Batas dependency antar layer, larangan circular import              |
| Quality  | AES301–AES305 | Batas ukuran file, mandatory definition, bypass comment             |
| Role     | AES401–AES406 | Penegakan peran tiap layer (taxonomy, contract, capabilities, dst.) |
| Orphan   | AES501–AES506 | Deteksi kode yang tidak terhubung ke graph import                   |

> **Baca selengkapnya:** [`RULES_AES.md`](RULES_AES.md) — definisi lengkap setiap kode aturan, severity, dan cara memperbaikinya.

---

## 4. Instruksi untuk Developer

Sebelum menulis atau memodifikasi kode, lakukan langkah berikut:

1. **Tentukan fitur** yang sedang dikerjakan: `registration`, `scoring`, `ranking`, `search`, atau `recap`.
2. **Tentukan layer** yang akan ditulis (taxonomy, contract, capabilities, infrastructure, agent, surface, atau root).
3. **Prioritaskan urutan pembuatan file:** taxonomy dan contract terlebih dahulu sebelum capabilities atau infrastructure.
4. **Validasi nama file** terhadap tabel suffix di [`RULES_AES.md`](RULES_AES.md) (AES101, AES102).
5. **Validasi import** terhadap tabel dependency di [`ARCHITECTURE.md`](ARCHITECTURE.md) (AES201, AES205).
6. **Jangan campur** business logic dan I/O dalam satu file.
7. **Tolak permintaan** yang melanggar aturan AES dan jelaskan kode aturan yang dilanggar.

---

## 5. Konvensi Tambahan

- **State:** Disimpan dalam `CompetitionState` yang dialokasikan di `root_cli_main_entry.c` dan di-pass via pointer ke setiap fungsi. Tidak ada variabel global.
- **Error handling:** Gunakan enum error dari taxonomy. Alur propagasi error: Capabilities mengembalikan error code → Agent meneruskan ke Surface → **Surface (layer `_command`) yang menampilkan pesan ke user**. Infrastructure tidak boleh melakukan output ke terminal; infrastructure hanya menangani I/O ke sistem (file, dll.) melalui port.
- **Header guard:** Semua file `.h` wajib menggunakan pola `#ifndef FEATURE_LAYER_CONCEPT_H`.
- **Build:** GCC dengan Makefile.

---

## 6. Dokumen Rujukan

| Dokumen                               | Isi                                                                             |
| ------------------------------------- | ------------------------------------------------------------------------------- |
| [`PRD.md`](PRD.md)                   | Requirement produk, aturan bisnis, state machine, library yang diizinkan        |
| [`ARCHITECTURE.md`](ARCHITECTURE.md) | Definisi layer AES, naming convention, diagram dependency, contoh struktur file |
| [`RULES_AES.md`](RULES_AES.md)       | Seluruh kode aturan AES beserta severity dan cara perbaikan                     |
