from struct import *

responseFormat = '12s 256s 3072s i'
requestFormat = '12s 3072s i'
# packetFormat = '12s 3072s i'
resLen = calcsize(responseFormat)

def leerOutput():
    with open("../OS/bin/output", "rb+", 0) as f:
        packed = f.read(resLen)
    return packed

def escribirInput(estructura):
    with open("../OS/bin/input", "wb+", 0) as f:
        f.write(estructura)

def packPacket(user, secCode, cmd):
    return pack(requestFormat, user, cmd, secCode)

def unpackPacket(packet):
    return unpack(responseFormat, packet)
