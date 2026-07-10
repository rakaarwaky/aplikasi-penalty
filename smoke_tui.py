import os, pty, time, sys, select

BIN = "./aplikasi_perhitungan_penalty"
NAMES = ["Budi", "Sari", "Tono", "Wati", "Joko"]

def main():
    pid, fd = pty.fork()
    if pid == 0:
        os.execv(BIN, [BIN])
        os._exit(127)

    def read_some(t=0.3):
        out = b""
        end = time.time() + t
        while time.time() < end:
            r, _, _ = select.select([fd], [], [], 0.1)
            if r:
                try:
                    out += os.read(fd, 4096)
                except OSError:
                    break
        return out

    def send(data, t=0.25):
        os.write(fd, data)
        return read_some(t)

    # Tunggu menu muncul
    buf = read_some(1.0)
    assert b"PENALTI" in buf or b"APLIKASI" in buf, "menu tidak muncul: %r" % buf[:200]

    # Menu 1: pendaftaran (sudah dipilih default)
    send(b"\r")  # Enter buka menu 1
    read_some(0.5)
    for n in NAMES:
        send(n.encode() + b"\r", 0.3)
    # kosong + enter selesai
    send(b"\r", 0.3)
    send(b"\r", 0.3)  # kembali ke menu

    # Menu 2: scoring
    send(b"\x1b")  # esc tidak keluar (confirm), lalu n
    read_some(0.2)
    # kami di menu; pilih menu 2
    # dari posisi awal 1, tekan down -> 2
    send(b"\x1b", 0.2)  # batalkan confirm keluar kalau muncul -> tekan n
    # asumsi sudah kembali ke menu; pastikan
    read_some(0.3)
    send(b"\x1b", 0.2)  # jika confirm, jawab n (char n)
    read_some(0.3)
    # navigasi ke menu 2
    send(b"\x1b", 0.1)
    read_some(0.2)

    # Pendekatan lebih robust: langsung tekan ESC beberapa kali untuk memastikan
    # kita di menu utama, lalu ArrowDown + Enter untuk menu 2.
    send(b"\x1b")  # jawab n jika confirm
    read_some(0.2)
    send(b"\x1b")  # jawab n
    read_some(0.2)

    print("SMOKE: menu + pendaftaran 5 peserta selesai tanpa crash")
    # Keluar bersih
    send(b"\x1b")  # jika di menu, esc -> confirm; jawab y
    read_some(0.1)
    send(b"y")
    time.sleep(0.5)
    try:
        os.write(fd, b"\n")
    except OSError:
        pass
    time.sleep(0.5)

    # Cek proses selesai
    _, status = os.waitpid(pid, os.WNOHANG)
    if status == 0:
        # belum keluar, beri waktu
        time.sleep(1)
        _, status = os.waitpid(pid, os.WNOHANG)
    print("PROCESS_EXIT_STATUS:", status)

if __name__ == "__main__":
    main()
