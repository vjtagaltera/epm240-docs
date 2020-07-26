#!/usr/bin/env python


import bluetooth


bd_addr = "00:70:00:00:00:D0" # BCM4356 37.4MHz AMPAK AP6356-0055

port = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))

sock.send("hello!!")

sock.close()


