#!/usr/bin/env bash
# Build dari source lalu pasang aplikasi.
#   ./install.sh          -> build + pasang ke /usr/local/bin (perlu sudo)
#   ./install.sh --local  -> build + salin ke ./bin/ (tanpa akses system)
set -e

cd "$(dirname "$0")"

LOCAL=0
if [ "$1" = "--local" ]; then
    LOCAL=1
fi

echo "=== Build ==="
make clean
make

if [ "$LOCAL" -eq 1 ]; then
    echo "=== Pasang ke ./bin (lokal) ==="
    mkdir -p bin
    cp aplikasi_perhitungan_penalti bin/
    chmod +x bin/aplikasi_perhitungan_penalti
    echo "Selesai. Jalankan: ./bin/aplikasi_perhitungan_penalti"
else
    echo "=== Pasang ke /usr/local/bin (system-wide) ==="
    if [ "$(id -u)" -ne 0 ]; then
        echo "(perlu sudo)"
        sudo make install
    else
        make install
    fi
    echo "Selesai. Jalankan: aplikasi_perhitungan_penalti"
fi
