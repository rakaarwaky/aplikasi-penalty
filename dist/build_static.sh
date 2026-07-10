#!/usr/bin/env bash
# Build penalty_shootout BENAR-BENAR self-contained (static, tanpa dependensi runtime).
# PRASYARAT (sekali saja): sudo apt install libncurses-dev  (memberi libncurses.a)
# Setelah dev package ada, binary ini jalan di distro Linux x86_64 mana pun
# TANPA perlu ncurses terpasang di mesin user.
set -e

cd "$(dirname "$0")/.."

# Cari library statis ncurses + tinfo (lokasi bisa beda tiap distro)
NC=$(find / -name 'libncurses.a' 2>/dev/null | head -1)
TI=$(find / -name 'libtinfo.a' 2>/dev/null | head -1)
if [ -z "$NC" ]; then
  echo "ERROR: libncurses.a tak ditemukan."
  echo "Jalankan dulu: sudo apt install libncurses-dev"
  exit 1
fi

gcc -std=c99 -O2 -static \
  $(find src -name '*.c') \
  -I src -I src/shared -I src/registration -I src/scoring \
  -I src/ranking -I src/search -I src/recap -I src/cli -I src/tui \
  "$NC" "${TI:+$TI}" -lpthread \
  -o penalty_shootout

echo "Build statis selesai: ./penalty_shootout"
file penalty_shootout
ldd penalty_shootout || echo "(not a dynamic executable -> sukses statis)"
