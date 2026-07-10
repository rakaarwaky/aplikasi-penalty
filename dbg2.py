import os, pty, time, select
BIN="./aplikasi_perhitungan_penalty"
NAMES=["Budi","Sari","Tono","Wati","Joko"]
def rs(fd,t=0.4):
    o=b"";end=time.time()+t
    while time.time()<end:
        r,_,_=select.select([fd],[],[],0.1)
        if r:
            try:o+=os.read(fd,4096)
            except OSError:break
    return o
pid,fd=pty.fork()
if pid==0:
    os.environ["TERM"]="xterm";os.execv(BIN,[BIN]);os._exit(127)
time.sleep(0.5)
rs(fd,0.6)
s=lambda d,t=0.3:(os.write(fd,d),rs(fd,t))[1]
for n in NAMES: s(n.encode()+b"\r",0.25)
print("after 5 names, screen tail:",repr(s(b"\r",0.4)[-120:]))  # empty -> break loop
print("after getch-back:",repr(s(b"\r",0.5)[-160:]))  # should be menu
print("after DOWN:",repr(s(b"\x1b[B",0.4)[:60]))
print("after ENTER:",repr(s(b"\r",0.5)[:80]))
_,st=os.waitpid(pid,os.WNOHANG)
