#!/usr/bin/env python

import sys
import bluetooth

uuid = "00000001-0001-11e8-8001-f4844c401234"

service_matches = bluetooth.find_service( uuid = uuid )

if len(service_matches) == 0:
    print "couldn't find the FooBar service"
    sys.exit(0)

print("    service_matches len %d" % len(service_matches))
for i,x in enumerate(service_matches):
  first_match = service_matches[i]
  port = first_match["port"]
  name = first_match["name"]
  host = first_match["host"]

  print "connecting to i %d \"%s\" on host %s at port %s" % (i, name, host, port)

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((host, port))
sock.send("hello!!")
sock.close()


