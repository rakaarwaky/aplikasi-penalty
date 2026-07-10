import os, pty, time, select
BIN = "./aplikasi_perhitungan_penalty"
NAMES = ["Budi", "Sari", "Tono", "Wati", "Joko"]
def rs(fd, t=0.4):
    o = b""
    end = time.time() + t
    while time.time() < end:
        r, _, _ = select.select([fd], [], [], 0.1)
        if r:
            try: o += os.read(fd, 4096)
            except OSError: break
    return o
def wait_for(fd, needle, tries=30):
    for _ in range(tries):
        o = rs(fd, 0.15)
        if needle in o: return o
    return o
pid, fd = pty.fork()
if pid == 0:
    os.environ["TERM"] = "xterm"
    os.execv(BIN, [BIN]); os._exit(127)
time.sleep(0.5); rs(fd, 0.6)
s = lambda d, t=0.4: (os.write(fd, d), rs(fd, t))[1]
s(b"\r", 0.5)
for n in NAMES:
    wait_for(fd, b"Nama peserta"); s(n.encode() + b"\r", 0.4)
wait_for(fd, b"Nama peserta"); o = s(b"\r", 0.5)
print("AFTER EMPTY: reg_still?", b"Nama peserta" in o, "| menu?", b"PENALTI" in o)
o2 = s(b"\r", 0.6)
print("AFTER BACK: menu?", b"PENALTI" in o2, "| reg?", b"Nama peserta" in o2)
print("SNIPPET:", repr(o2[:150]))
_, st = os.waitpid(pid, os.WNOHANG)
