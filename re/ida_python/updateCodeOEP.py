from struct import * 
#change OEP
exe=open('shakedown_bak.exe', 'r+b')
exe.seek(0x128)
exe.write(pack('L', 0x45007))

#overwrite
mem=open('m.txt', 'rb')
ctn=mem.read(0x1c00)
mem.close()
exe.seek(0x19000)
exe.write(ctn)
exe.close()

