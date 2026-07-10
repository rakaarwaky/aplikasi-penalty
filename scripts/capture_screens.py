#!/usr/bin/env python3
"""Generate all screen states by calling app API directly via subprocess test helper."""

import subprocess, os, time

BASE = "/home/raka/Study/PengantarCoding/aplikasi-penalty"
SCREENSHOTS = f"{BASE}/screenshots"
TMUX = "p"

def run(cmd):
    r = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    return r.stdout.strip()

def capture(name):
    """Capture tmux pane to .txt file with ANSI colors."""
    out = f"{SCREENSHOTS}/{name}.txt"
    subprocess.run(f"tmux capture-pane -t {TMUX} -p -e > {out}", shell=True)
    print(f"  captured: {name}.txt")

def press(key, wait=0.5):
    subprocess.run(f"tmux send-keys -t {TMUX} '{key}'", shell=True)
    time.sleep(wait)

def enter(wait=0.5):
    press("Enter", wait)

def start_app():
    subprocess.run("tmux kill-server 2>/dev/null", shell=True)
    time.sleep(0.3)
    subprocess.run(f"tmux new-session -d -s {TMUX} -x 80 -y 24", shell=True)
    time.sleep(0.3)
    subprocess.run(f"tmux send-keys -t {TMUX} 'cd {BASE} && ./aplikasi_perhitungan_penalti' Enter", shell=True)
    time.sleep(1.5)

def wait_for(text, timeout=10):
    """Wait until text appears in tmux pane."""
    start = time.time()
    while time.time() - start < timeout:
        out = run(f"tmux capture-pane -t {TMUX} -p")
        if text in out:
            return True
        time.sleep(0.3)
    return False

def input_zone(zone):
    """Input a zone number in scoring mode."""
    press(str(zone), 0.3)
    enter(1.2)

def input_name(name):
    """Input participant name in registration."""
    press(name, 0.3)
    enter(0.8)

if __name__ == "__main__":
    os.makedirs(SCREENSHOTS, exist_ok=True)

    print("Starting app...")
    start_app()

    # 1. Menu utama
    wait_for("APLIKASI PERHITUNGAN")
    capture("01_menu_utama")

    # 2. Masuk pendaftaran
    enter(1)
    wait_for("PENDAFTARAN")
    capture("02_pendaftaran_awal")

    # 3. Daftar 5 peserta
    for name in ["Budi", "Sari", "Tono", "Wati", "Joko"]:
        input_name(name)
    capture("03_pendaftaran_5peserta")

    # Selesai daftar
    press("", 0.3)
    enter(1.5)
    wait_for("APLIKASI PERHITUNGAN")
    capture("04_menu_registered")

    # 4. Masuk scoring
    press("2", 1.5)
    wait_for("Masukkan zona")
    capture("05_scoring_awal")

    # 5. Input semua scoring
    kicks = {
        "Budi": [5, 5, 3, 0, 2, 4, 5],
        "Sari": [4, 4, 4, 3, 3, 2, 1],
        "Tono": [3, 3, 3, 2, 2, 1, 0],
        "Wati": [2, 2, 2, 1, 1, 0, 0],
        "Joko": [1, 1, 1, 0, 0, 0, 0],
    }

    for name, zones in kicks.items():
        for z in zones:
            input_zone(z)
        # Tunggu selesai participant
        time.sleep(0.5)
        if wait_for("SELESAI", 3):
            enter(1.5)  # Lanjut ke peserta berikutnya
        elif wait_for("Semua tendangan selesai", 3):
            enter(1.5)  # Kembali ke menu
            break

    # Tunggu menu
    wait_for("APLIKASI PERHITUNGAN")
    capture("07_menu_completed")

    # 6. Ranking
    press("3", 1.5)
    wait_for("RANKING")
    time.sleep(0.5)
    capture("08_ranking")
    enter(1)

    # 7. Search
    wait_for("APLIKASI PERHITUNGAN")
    press("4", 1.5)
    wait_for("CARI PESERTA")
    capture("09_search_input")

    # Cari Budi
    press("Budi", 0.3)
    enter(1.5)
    wait_for("DITEMUKAN")
    capture("10_search_found")
    enter(1)

    # 8. Recap
    wait_for("APLIKASI PERHITUNGAN")
    press("5", 1.5)
    wait_for("REKAPITULASI")
    time.sleep(0.5)
    capture("11_recap")
    enter(1)

    # 9. Storage
    wait_for("APLIKASI PERHITUNGAN")
    press("6", 1.5)
    wait_for("SIMPAN")
    capture("12_storage")

    # 10. Help
    press("0", 0.5)
    press("y", 0.5)
    time.sleep(1)
    # Restart for help
    start_app()
    wait_for("APLIKASI PERHITUNGAN")
    press("h", 1.5)
    wait_for("PANDUAN")
    capture("13_help")

    print("\nAll screenshots captured!")
