import os

i = os.open('/dev/tun', os.O_RDWR)

while True:
    pkt = os.read(i, 65536)
    print('recv', pkt.hex())
    pkt = bytearray(pkt)
    pkt[12:16], pkt[16:20] = pkt[16:20], pkt[12:16]
    pkt[20] = 0
    if pkt[22] >= 248:
        print('dropping')
        continue
    pkt[22] += 8
    print('send', pkt.hex())
    os.write(i, pkt)
