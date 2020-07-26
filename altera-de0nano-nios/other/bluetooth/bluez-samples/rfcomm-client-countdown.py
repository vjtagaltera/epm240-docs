#!/usr/bin/env python


import bluetooth
import time


bd_addr = "00:70:00:00:00:D0" # BCM4356 37.4MHz AMPAK AP6356-0055

port = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))

for n in range(10,0,-1):
    print("  countdown %d" % n)
    time.sleep(10)
print("say hello")

sock.send("hello!!")

sock.close()


