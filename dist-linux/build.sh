#!/usr/bin/env bash
# Build aplikasi_perhitungan_penalti untuk distribusi Linux (dynamic, ncurses shared).
# ncurses sudah ada di hampir semua distro Linux -> user tak perlu install apa-apa.
set -e

cd "$(dirname "$0")/.."

gcc -std=c99 -O2 \
  $(find src -name '*.c') \
  -I src -I src/shared -I src/registration -I src/scoring \
  -I src/ranking -I src/search -I src/recap -I src/cli -I src/tui \
  $(pkg-config --libs ncurses) \
  -o aplikasi_perhitungan_penalti

echo "Build selesai: ./aplikasi_perhitungan_penalti"
echo "Cek dependensi:"
ldd aplikasi_perhitungan_penalti | grep -i ncurses || echo "(ncurses ter-link static? bukan)"
