import os, pty, time, select

BIN = "./aplikasi_perhitungan_penalty"
NAMES = ["Budi", "Sari", "Tono", "Wati", "Joko"]
# zona tiap peserta (7 angka 0-5) -> agar ada juara jelas
ZONES = [
    [5,5,5,5,5,5,5],  # Budi 35
    [4,4,4,4,4,4,4],  # Sari 28
    [3,3,3,3,3,3,3],  # Tono 21
    [2,2,2,2,2,2,2],  # Wati 14
    [1,1,1,1,1,1,1],  # Joko 7
]

def read_some(fd, t=0.3):
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

def send(fd, data, t=0.25):
    os.write(fd, data)
    return read_some(fd, t)

def main():
    pid, fd = pty.fork()
    if pid == 0:
        os.execv(BIN, [BIN])
        os._exit(127)

    buf = read_some(fd, 1.0)
    assert b"PENALTI" in buf, "menu tidak muncul"

    # --- Menu 1: pendaftaran ---
    send(fd, b"\r", 0.4)
    for n in NAMES:
        send(fd, n.encode() + b"\r", 0.25)
    send(fd, b"\r", 0.25)  # kosong = selesai
    send(fd, b"\r", 0.25)  # kembali ke menu

    # --- Menu 2: scoring ---
    # pastikan di menu utama dulu
    send(fd, b"\x1b", 0.15)  # esc (jika confirm keluar -> n)
    send(fd, b"n", 0.15)
    read_some(fd, 0.2)
    # navigasi: dari posisi mana pun, tekan ESC tidak ubah; pakai pendekatan:
    # keluar ke menu lalu ArrowDown 1x -> menu 2
    # posisi default menu 1; tekan down -> 2
    send(fd, b"\x1b", 0.1)  # batalkan kalau confirm
    send(fd, b"n", 0.1)
    read_some(fd, 0.2)
    send(fd, b"\x1b", 0.1); send(fd, b"n", 0.1)
    read_some(fd, 0.2)

    # Sekarang asumsikan di menu utama dg seleksi di menu 1.
    # Turun 1 -> menu 2, Enter
    send(fd, b"\x1b", 0.05)
    send(fd, b"n", 0.05)
    send(fd, b"\x1b", 0.05)
    send(fd, b"n", 0.05)
    read_some(fd, 0.2)

    # Buka menu 2
    send(fd, b"\x1b", 0.05); send(fd, b"n", 0.05)
    read_some(fd, 0.2)
    send(fd, b"\x1b", 0.05); send(fd, b"n", 0.05)
    read_some(fd, 0.2)

    # Masuk ke scoring dengan Enter pada menu 2
    send(fd, b"\r", 0.4)
    scr = read_some(fd, 0.5)
    in_scoring = b"INPUT TENDANGAN" in scr
    print("IN_SCORING:", in_scoring)

    if in_scoring:
        for zones in ZONES:
            for z in zones:
                send(fd, str(z).encode() + b"\r", 0.12)
            send(fd, b"\r", 0.2)  # selesai per peserta
        send(fd, b"\r", 0.3)  # penutup -> kembali ke menu

    # --- Menu 3: ranking ---
    # dari menu utama posisi menu 2, turun 1 -> menu 3
    send(fd, b"\x1b", 0.05); send(fd, b"n", 0.05)
    send(fd, b"\x1b", 0.05); send(fd, b"n", 0.05)
    read_some(fd, 0.2)
    send(fd, b"\x1b", 0.05); send(fd, b"n", 0.05)
    read_some(fd, 0.2)
    send(fd, b"\x1b", 0.05); send(fd, b"n", 0.05)
    read_some(fd, 0.2)
    # buka menu 3
    send(fd, b"\r", 0.4)
    scr = read_some(fd, 0.6)
    has_rank = b"RANKING" in scr
    print("IN_RANKING:", has_rank)
    # cek medali/pemenang muncul
    print("HAS_BUDI:", b"Budi" in scr)
    send(fd, b"\r", 0.3)  # kembali

    # Keluar
    send(fd, b"\x1b", 0.1); send(fd, b"y", 0.2)
    time.sleep(0.4)
    send(fd, b"\n", 0.3)
    time.sleep(0.4)
    _, status = os.waitpid(pid, os.WNOHANG)
    if status == 0:
        time.sleep(1)
        _, status = os.waitpid(pid, os.WNOHANG)
    print("PROCESS_EXIT_STATUS:", status)

if __name__ == "__main__":
    main()
