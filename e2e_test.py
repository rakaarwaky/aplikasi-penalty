import pexpect, sys, time, os

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
    c = pexpect.spawn(BIN, timeout=8, encoding="utf-8",
                      codec_errors="replace",
                      env={"TERM":"xterm", "LINES":"30", "COLUMNS":"80"})
    c.setwinsize(30, 80)
    c.logfile = sys.stdout
    fails = []

    def check(cond, msg):
        print(("PASS" if cond else "FAIL") + ": " + msg)
        if not cond: fails.append(msg)

    # Splash auto-advances; menu appears
    c.expect("LOMBA TENDANGAN PENALTI", timeout=10)
    check(True, "menu muncul setelah splash otomatis")

    # ---- Registration: 5 peserta ----
    for n in ORDER:
        c.expect("Nama peserta #", timeout=8)
        c.sendline(n)
        c.expect(n + " berhasil terdaftar", timeout=8)
    # empty to finish
    c.expect("Nama peserta #", timeout=8)
    c.sendline("")
    c.expect("Total peserta", timeout=8)
    c.sendline("")  # back to menu
    c.expect("LOMBA TENDANGAN PENALTI", timeout=8)
    check(True, "pendaftaran 5 peserta selesai")

    # ---- Scoring: open menu 2 (Input Tendangan) ----
    c.send("\x1b[B")  # down to 2
    c.sendline("")
    c.expect("INPUT TENDANGAN DAN SKOR", timeout=8)
    for n in ORDER:
        for z in ZONES[n]:
            c.sendline(str(z))
            c.expect("Zona %d -> %d poin" % (z, z), timeout=8)
        c.expect("SELESAI", timeout=8)
        c.sendline("")  # next participant
    c.expect("SEMUA TENDANGAN SELESAI", timeout=8)
    c.sendline("")  # back to menu
    c.expect("LOMBA TENDANGAN PENALTI", timeout=8)
    check(True, "scoring 5 peserta x 7 tendangan selesai")

    # ---- Ranking: open menu 3 ----
    c.send("\x1b[B"); c.send("\x1b[B")  # to 3
    c.sendline("")
    c.expect("RANKING PESERTA", timeout=8)
    # Winner should be Budi
    idx = c.before.find("JUARA:")
    check(idx != -1 and "Budi" in c.before[idx:idx+40], "juara 1 = Budi (skor tertinggi)")
    c.sendline("")  # back
    c.expect("LOMBA TENDANGAN PENALTI", timeout=8)

    # ---- Search: open menu 4, search 'Sari' ----
    c.send("\x1b[B"); c.send("\x1b[B"); c.send("\x1b[B")  # to 4
    c.sendline("")
    c.expect("CARI PESERTA", timeout=8)
    c.expect("Nama:", timeout=8)
    c.sendline("Sari")
    c.expect("DITEMUKAN", timeout=8)
    check("Sari" in c.before and "28 poin" in c.before, "search Sari -> total 28 poin")
    c.sendline("")  # back
    c.expect("LOMBA TENDANGAN PENALTI", timeout=8)

    # search not found
    c.send("\x1b[B"); c.send("\x1b[B"); c.send("\x1b[B")
    c.sendline("")
    c.expect("CARI PESERTA")
    c.expect("Nama:")
    c.sendline("Zzz")
    c.expect("TIDAK DITEMUKAN", timeout=8)
    check(True, "search tidak ditemukan handle")
    c.sendline("")
    c.expect("LOMBA TENDANGAN PENALTI", timeout=8)

    # ---- Recap: open menu 5, then export ----
    c.send("\x1b[B"); c.send("\x1b[B"); c.send("\x1b[B"); c.send("\x1b[B")  # to 5
    c.sendline("")
    c.expect("REKAPITULASI LENGKAP", timeout=8)
    check("Tertinggi: 35" in c.before, "recap tertinggi = 35 (Budi)")
    check("JUARA: Budi" in c.before, "recap juara = Budi")
    c.send("e")  # export
    c.expect("Berhasil diekspor", timeout=8)
    c.sendline("")
    c.expect("REKAPITULASI LENGKAP")
    c.sendline("")  # back
    c.expect("LOMBA TENDANGAN PENALTI", timeout=8)

    # export file exists?
    export_path = os.path.join(OUT_DIR, "ranking.txt")
    check(os.path.exists(export_path), "file ranking.txt tergenerate")

    # ---- Exit: menu 0 ----
    c.send("\x1b[B"); c.send("\x1b[B"); c.send("\x1b[B"); c.send("\x1b[B"); c.send("\x1b[B")  # to 0
    c.sendline("")
    c.expect("Yakin ingin keluar", timeout=8)
    c.send("y")
    # champion summary
    c.expect("JUARA UMUM", timeout=8)
    check(True, "ringkasan juara sebelum keluar muncul")
    c.sendline("")
    c.expect(pexpect.EOF, timeout=8)

    print("\n=== %d FAIL(S) ===" % len(fails))
    for f in fails: print(" - " + f)
    sys.exit(1 if fails else 0)

if __name__ == "__main__":
    main()
