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
time.sleep(0.5); rs(fd,0.6)
s=lambda d,t=0.5:(os.write(fd,d),rs(fd,t))[1]
for n in NAMES:
    out=s(n.encode()+b"\r",0.5)
    print("NAME",n,"->", "OK" if b"[OK]" in out else ("FAIL:"+repr(out[-80:])))
# sekarang empty enter
out=s(b"\r",0.5)
print("EMPTY ENTER ->", "back-to-menu?" , b"PENALTI" in out or b"APLIKASI" in out, "|", repr(out[-100:]))
_,st=os.waitpid(pid,os.WNOHANG)
