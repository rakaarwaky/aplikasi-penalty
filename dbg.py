import os, pty, time, select
BIN="./aplikasi_perhitungan_penalty"
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
print("MENU1:",repr(rs(fd,0.6)[:120]))
send=lambda d,t=0.3:(os.write(fd,d),rs(fd,t))[1]
print("after ENTER:",repr(send(b"\r",0.5)[:200]))
print("after name:",repr(send(b"Budi\r",0.4)[:200]))
print("after empty:",repr(send(b"\r",0.4)[:200]))
print("after ENTER back:",repr(send(b"\r",0.4)[:200]))
# now at menu, down+enter
print("after DOWN:",repr(send(b"\x1b[B",0.4)[:200]))
print("after ENTER2:",repr(send(b"\r",0.5)[:200]))
_,st=os.waitpid(pid,os.WNOHANG)
