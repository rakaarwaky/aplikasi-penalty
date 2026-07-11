@echo off
REM Jalankan aplikasi yang SUDAH di-compile (tanpa build ulang).
REM Prioritas: exe dari dist-windows.
setlocal

set "EXE=dist-windows\aplikasi_perhitungan_penalti.exe"

if exist "%EXE%" (
    echo Menjalankan %EXE%...
    "%EXE%"
) else (
    echo Build belum ada di dist-windows\. >&2
    echo Jalankan install.bat (build + pasang) terlebih dahulu. >&2
    pause
    exit /b 1
)
