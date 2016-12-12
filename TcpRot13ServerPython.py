#!/usr/bin/python3
# To run this type python [filename]

from socket import *
import codecs

def rot13_service(c):
    while True:
        inbytes == c.recv(1024)
        if inbytes:
            outbytes = codecs.encode(inbytes.decode(), 'rot_13').encode()
            c.send(outbytes)
        else:
            break;

s = socket(AF_INET, SOCK_STREAM)
s.bind(('', 1068))
s.listen(5)
print("listening...")

while True:
    client.addr = s.accept()
    print("connected...")
    rot13_service(client)
    client.close()
