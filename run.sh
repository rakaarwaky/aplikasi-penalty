#!/usr/bin/env bash
# Jalankan aplikasi yang SUDAH di-compile (tanpa build ulang).
# Prioritas: AppImage -> binary dist-linux.
set -e

cd "$(dirname "$0")"

APP_DIR="dist-linux"
APPIMAGE="$APP_DIR/AplikasiPenaltyShootout-x86_64.AppImage"
BINARY="$APP_DIR/aplikasi_perhitungan_penalti"

if [ -f "$APPIMAGE" ]; then
    echo "Menjalankan AppImage..."
    chmod +x "$APPIMAGE"
    exec ./"$APPIMAGE"
elif [ -f "$BINARY" ]; then
    echo "Menjalankan binary dist-linux..."
    chmod +x "$BINARY"
    exec ./"$BINARY"
else
    echo "Build belum ada di $APP_DIR/." >&2
    echo "Jalankan './install.sh' (build + pasang) atau './run.sh' setelah 'make'." >&2
    exit 1
fi
