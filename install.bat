@echo off
REM Build dari source lalu pasang aplikasi di Windows.
REM   install.bat          -> build (make win) + copy ke C:\Program Files\AplikasiPenalti
REM                           (jalankan sebagai Administrator)
REM   install.bat local    -> build + copy ke folder .\bin\ (tanpa akses system)
setlocal

set "TARGET=%~1"

echo === Build (.exe via MinGW) ===
call make win
if errorlevel 1 (
    echo Build gagal. Pastikan MinGW (pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-pdcurses) terpasang. >&2
    pause
    exit /b 1
)

if "%TARGET%"=="local" (
    echo === Pasang ke .\bin (lokal) ===
    if not exist "bin" mkdir bin
    copy /Y aplikasi_perhitungan_penalti.exe bin\ >nul
    echo Selesai. Jalankan: bin\aplikasi_perhitungan_penalti.exe
) else (
    echo === Pasang ke C:\Program Files\AplikasiPenalti (system-wide) ===
    set "DEST=%ProgramFiles%\AplikasiPenalti"
    if not exist "%DEST%" mkdir "%DEST%"
    copy /Y aplikasi_perhitungan_penalti.exe "%DEST%\" >nul
    echo Selesai. Jalankan: "%DEST%\aplikasi_perhitungan_penalti.exe"
)
pause
