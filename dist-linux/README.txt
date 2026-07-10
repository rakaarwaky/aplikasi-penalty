PENALTY SHOOTOUT — DISTRIBUSI LINUX
===================================

Isi folder ini:
  - aplikasi_perhitungan_penalty        (binary aplikasi, sudah jadi)
  - PenaltyShootout.desktop (launcher: 2x klik -> buka terminal -> jalan)
  - README.txt              (file ini)

CARA PAKAI (user awam):
  1. Extract folder ini ke mana saja.
  2. Klik kanan "PenaltyShootout.desktop" -> "Allow Launching" 
     atau langsung dobel-klik.
  3. Terminal otomatis terbuka dan permainan jalan.
  4. Setelah selesai, tekan Enter untuk menutup terminal.

SYARAT:
  - Sistem Linux (x86_64). Binary ini untuk arsitektur 64-bit.
  - ncurses sudah terpasang di hampir semua distro Linux
    (Ubuntu/Debian/Fedora bawaan) -> TIDAK perlu install apa-apa,
    TIDAK perlu koneksi internet.

CATATAN UNTUK DISTRIBUTOR:
  - Binary ini dibuild di Fedora (glibc modern). Untuk kompatibilitas
    maksimal ke distro lama, rebuild di distro lama/stable.
  - Bila ingin binary 100% tanpa dependensi (self-contained), jalankan
    build_static.sh (butuh ncurses static: compile ncurses dari sumber).
