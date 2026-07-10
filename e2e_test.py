import pexpect, sys, os

BIN = "/home/raka/Study/PengantarCoding/aplikasi-penalty/aplikasi_perhitungan_penalty"
OUT_DIR = "/home/raka/Study/PengantarCoding/aplikasi-penalty"
ZONES = {
    "Budi":  [5,5,5,5,5,5,5],   # 35
    "Sari":  [4,4,4,4,4,4,4],   # 28
    "Tono":  [3,3,3,3,3,3,3],   # 21
    "Wati":  [2,2,2,2,2,2,2],   # 14
    "Joko":  [1,1,1,1,1,1,1],   # 7
}
ORDER = ["Budi","Sari","Tono","Wati","Joko"]

def main():
    c = pexpect.spawn(BIN, timeout=10, encoding="utf-8",
                      codec_errors="replace",
                      env={"TERM":"xterm", "LINES":"30", "COLUMNS":"80"})
    c.setwinsize(30, 80)
    c.logfile = sys.stdout
    fails = []

    def check(cond, msg):
        print(("PASS" if cond else "FAIL") + ": " + msg)
        if not cond: fails.append(msg)

    def menu():
        c.expect("LOMBA TENDANGAN PENALTI", timeout=12)

    # Splash auto-advances -> menu
    menu()
    check(True, "menu muncul setelah splash otomatis")

    # ---- 1) Registration ----
    c.sendline("1")  # shortcut menu 1
    c.expect("Nama peserta #", timeout=10)
    for n in ORDER:
        c.sendline(n)
        c.expect(n + " berhasil terdaftar", timeout=10)
    c.sendline("")                 # empty -> finish
    c.expect("Total peserta", timeout=10)
    c.sendline("")                 # back to menu
    menu()
    check(True, "pendaftaran 5 peserta selesai & kembali ke menu")

    # ---- 2) Scoring ----
    c.sendline("2")
    c.expect("INPUT TENDANGAN DAN SKOR", timeout=10)
    for n in ORDER:
        for z in ZONES[n]:
            c.sendline(str(z))
            c.expect("Zona %d -> %d poin" % (z, z), timeout=10)
        c.expect("SELESAI", timeout=10)
        c.sendline("")             # next participant
    c.expect("SEMUA TENDANGAN SELESAI", timeout=10)
    c.sendline("")                 # back to menu
    menu()
    check(True, "scoring 5x7 tendangan selesai & kembali ke menu")

    # ---- 3) Ranking ----
    c.sendline("3")
    c.expect("RANKING PESERTA", timeout=10)
    idx = c.before.find("JUARA:")
    check(idx != -1 and "Budi" in c.before[idx:idx+40], "juara 1 = Budi (skor tertinggi)")
    c.sendline("")                 # back
    menu()

    # ---- 4) Search found ----
    c.sendline("4")
    c.expect("Nama:", timeout=10)
    c.sendline("Sari")
    c.expect("DITEMUKAN", timeout=10)
    check("Sari" in c.before and "28 poin" in c.before, "search Sari -> total 28 poin")
    c.sendline("")
    menu()

    # ---- 5) Search not found ----
    c.sendline("4")
    c.expect("Nama:", timeout=10)
    c.sendline("Zzz")
    c.expect("TIDAK DITEMUKAN", timeout=10)
    check(True, "search tidak ditemukan handle")
    c.sendline("")
    menu()

    # ---- 6) Recap + export ----
    c.sendline("5")
    c.expect("REKAPITULASI LENGKAP", timeout=10)
    check("Tertinggi: 35" in c.before, "recap tertinggi = 35 (Budi)")
    check("JUARA: Budi" in c.before, "recap juara = Budi")
    c.send("e")                    # export
    c.expect("Berhasil diekspor", timeout=10)
    c.sendline("")
    c.expect("REKAPITULASI LENGKAP")
    c.sendline("")                 # back to menu
    menu()
    export_path = os.path.join(OUT_DIR, "ranking.txt")
    check(os.path.exists(export_path), "file ranking.txt tergenerate")

    # ---- 7) Help screen ----
    c.send("h")
    c.expect("PANDUAN PENGGUNAAN", timeout=10)
    check(True, "layar bantuan (h) muncul")
    c.sendline("")                 # back to menu
    menu()

    # ---- 8) Exit + champion summary ----
    c.sendline("0")
    c.expect("Yakin ingin keluar", timeout=10)
    c.send("y")
    c.expect("JUARA UMUM", timeout=10)
    check(True, "ringkasan juara sebelum keluar muncul")
    c.sendline("")
    c.expect(pexpect.EOF, timeout=10)

    print("\n=== %d FAIL(S) ===" % len(fails))
    for f in fails: print(" - " + f)
    sys.exit(1 if fails else 0)

if __name__ == "__main__":
    main()
