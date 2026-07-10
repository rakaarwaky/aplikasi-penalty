import pexpect, sys

BIN = "/home/raka/Study/PengantarCoding/aplikasi-penalty/aplikasi_perhitungan_penalty"
NAMES = ["Budi", "Sari", "Tono", "Wati", "Joko"]
ZONES = [
    [5,5,5,5,5,5,5],
    [4,4,4,4,4,4,4],
    [3,3,3,3,3,3,3],
    [2,2,2,2,2,2,2],
    [1,1,1,1,1,1,1],
]

def main():
    c = pexpect.spawn(BIN, timeout=8, encoding="utf-8")
    c.setwinsize(30, 80)

    # Menu utama
    c.expect("APLIKASI LOMBA TENDANGAN PENALTI")

    # Menu 1: pendaftaran (selected awal = 1)
    c.sendline("")  # Enter buka menu 1
    for n in NAMES:
        c.expect("Nama peserta")
        c.sendline(n)
    c.expect("Nama peserta")
    c.sendline("")            # kosong -> selesai
    c.expect("Total peserta")
    c.sendline("")            # kembali ke menu

    # Menu 2: scoring (turun 1 dari posisi 1)
    c.expect("APLIKASI LOMBA")
    c.send("\x1b[B")          # ArrowDown
    c.sendline("")            # Enter buka scoring
    c.expect("INPUT TENDANGAN DAN SKOR")
    for zones in ZONES:
        for z in zones:
            c.expect("Masukkan zona")
            c.sendline(str(z))
        c.expect("SELESAI")
        c.sendline("")        # selesai per peserta
    c.expect("Semua tendangan selesai")
    c.sendline("")            # kembali ke menu

    # Menu 3: ranking (posisi 2, turun 1 -> 3)
    c.expect("APLIKASI LOMBA")
    c.send("\x1b[B")
    c.sendline("")
    c.expect("RANKING PESERTA")
    screen = c.before + c.after
    print("IN_RANKING: True | HAS_BUDI:", "Budi" in screen, "| HAS_MEDAL:", "*1*" in screen)
    c.sendline("")            # kembali

    # Menu 5: recap (posisi 3, turun 2 -> 5)
    c.expect("APLIKASI LOMBA")
    c.send("\x1b[B"); c.send("\x1b[B")
    c.sendline("")
    c.expect("REKAPITULASI LENGKAP")
    print("IN_RECAP: True")
    c.sendline("")            # kembali

    # Keluar: ESC -> confirm -> y
    c.expect("APLIKASI LOMBA")
    c.send("\x1b")            # ESC -> konfirmasi keluar
    c.expect("Keluar dari aplikasi")
    c.sendline("y")
    c.expect(pexpect.EOF)
    print("EXIT: clean (EOF reached)")
    print("ALL_OK")

if __name__ == "__main__":
    main()
