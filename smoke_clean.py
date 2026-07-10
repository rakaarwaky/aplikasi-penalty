import os, pty, time, select

BIN = "./aplikasi_perhitungan_penalty"
NAMES = ["Budi", "Sari", "Tono", "Wati", "Joko"]
ZONES = [
    [5,5,5,5,5,5,5],
    [4,4,4,4,4,4,4],
    [3,3,3,3,3,3,3],
    [2,2,2,2,2,2,2],
    [1,1,1,1,1,1,1],
]

def read_some(fd, t=0.3):
    out = b""
    end = time.time() + t
    while time.time() < end:
        r, _, _ = select.select([fd], [], [], 0.1)
        if r:
            try: out += os.read(fd, 4096)
            except OSError: break
    return out

def send(fd, data, t=0.2):
    os.write(fd, data)
    return read_some(fd, t)

def down(fd, n=1):
    for _ in range(n):
        send(fd, b"\x1b[B", 0.12)  # ArrowDown

def main():
    pid, fd = pty.fork()
    if pid == 0:
        os.environ["TERM"] = "xterm"
        os.execv(BIN, [BIN]); os._exit(127)

    assert b"PENALTI" in read_some(fd, 1.0)

    # Menu 1 (selected awal = 1)
    send(fd, b"\r", 0.4)
    for n in NAMES:
        send(fd, n.encode() + b"\r", 0.2)
    send(fd, b"\r", 0.2)   # kosong -> selesai
    send(fd, b"\r", 0.2)   # kembali ke menu (selected masih 1)

    # -> Menu 2 (scoring)
    down(fd, 1)
    send(fd, b"\r", 0.4)
    scr = read_some(fd, 0.5)
    in_scoring = b"INPUT TENDANGAN" in scr
    print("IN_SCORING:", in_scoring)
    if in_scoring:
        for zones in ZONES:
            for z in zones:
                send(fd, str(z).encode() + b"\r", 0.08)
            send(fd, b"\r", 0.15)
        send(fd, b"\r", 0.3)  # penutup -> menu

    # -> Menu 3 (ranking): posisi sekarang 2, turun 1 -> 3
    down(fd, 1)
    send(fd, b"\r", 0.4)
    scr = read_some(fd, 0.6)
    in_rank = b"RANKING" in scr
    print("IN_RANKING:", in_rank, "| HAS_BUDI:", b"Budi" in scr, "| HAS_MEDAL:", b"*1*" in scr)
    send(fd, b"\r", 0.3)  # kembali

    # -> Menu 5 (recap): posisi 3, turun 2 -> 5
    down(fd, 2)
    send(fd, b"\r", 0.4)
    scr = read_some(fd, 0.6)
    print("IN_RECAP:", b"REKAPITULASI" in scr)
    send(fd, b"\r", 0.3)

    # Keluar: ESC -> confirm -> 'y' + Enter
    send(fd, b"\x1b", 0.15)
    send(fd, b"y\r", 0.3)
    time.sleep(0.4)
    _, st = os.waitpid(pid, os.WNOHANG)
    if st == 0:
        time.sleep(1); _, st = os.waitpid(pid, os.WNOHANG)
    print("EXIT_STATUS:", st)

if __name__ == "__main__":
    main()
