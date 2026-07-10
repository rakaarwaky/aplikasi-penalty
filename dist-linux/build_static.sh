#!/usr/bin/env bash
# Build aplikasi_perhitungan_penalti dengan ncurses DIBUNGKUS (static) ke dalam binary.
# Hasilnya: user TIDAK perlu ncurses terpasang di mesinnya — hanya butuh glibc
# (sudah ada di semua Linux). Bukan "-static" penuh (glibc static tak ada di Fedora).
#
# Jika libncursesw.a / libtinfow.a belum ada di sistem, script ini otomatis
# download source ncurses, compile static (tanpa C++ binding, kompatibel GCC 16),
# lalu link. Butuh koneksi internet HANYA saat build di mesin developer —
# hasil binary-nya sendiri tak butuh internet di sisi user.
set -e

cd "$(dirname "$0")/.."

NCDIR=""
if find / -name 'libncursesw.a' 2>/dev/null | grep -q .; then
  NCDIR=$(dirname "$(find / -name 'libncursesw.a' 2>/dev/null | head -1)")
  echo "Pakai ncurses static di: $NCDIR"
else
  echo "libncursesw.a tak ditemukan -> download & build dari source..."
  TMP=$(mktemp -d)
  cd "$TMP"
  curl -sL -o nc.tgz https://ftp.gnu.org/gnu/ncurses/ncurses-6.5.tar.gz
  tar xzf nc.tgz
  cd ncurses-6.5
  ./configure --enable-static --disable-shared --without-shared --with-termlib \
    --without-cxx --without-cxx-binding --without-ada --without-progs >/tmp/nc_cfg.log 2>&1
  make -j2 CFLAGS="-O1" >/tmp/nc_mk.log 2>&1
  NCDIR="$TMP/ncurses-6.5/lib"
  NCHDR="$TMP/ncurses-6.5/include"
  cd "$(dirname "$0")/.."
fi

NCHDR="${NCHDR:-$NCDIR/../include}"

gcc -std=c99 -O2 \
  $(find src -name '*.c') \
  -I src -I src/shared -I src/registration -I src/scoring \
  -I src/ranking -I src/search -I src/recap -I src/cli -I src/tui \
  -I "$NCHDR" \
  -Wl,-Bstatic -L"$NCDIR" -lncursesw -ltinfow -Wl,-Bdynamic \
  -lpthread \
  -o aplikasi_perhitungan_penalti

echo "Build selesai: ./aplikasi_perhitungan_penalti"
file aplikasi_perhitungan_penalti
echo "Dependensi (harus TIDAK ada ncurses/tinfo):"
ldd aplikasi_perhitungan_penalti | grep -iE 'ncurses|tinfo' || echo "  -> ncurses dibungkus (OK)"
