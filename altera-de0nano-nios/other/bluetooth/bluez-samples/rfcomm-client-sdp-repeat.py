#!/usr/bin/env python
# tested on python 2.7.17 PyBluez 0.23

import sys
import bluetooth
import socket
import fcntl, os
import errno
import time


def recv(sock, timeout):
    rb, rt = None, None
    loopcnt = 0
    tm1 = time.time()
    while True:
        loopcnt += 1
        try:
            msg = sock.recv(4096)
        except Exception as e:
            err = e.args[0]
            if err == errno.EAGAIN or err == errno.EWOULDBLOCK:
                tm2 = time.time()
                if (tm2 - tm1) > timeout:
                    rt = tm2 - tm1
                    break
                time.sleep(0.003)
                continue
            else:
                # a "real" error occurred
                print e
                tm3 = time.time()
                rt = tm3 - tm1
                break
        else:
            # ok
            tm4 = time.time()
            rb, rt = msg, tm4 - tm1
            break
    return rb, rt, loopcnt  # return value, time, loops


uuid = "00000001-0001-11e8-8001-f4844c401234"

tm_discovery_0 = time.time()
service_matches = bluetooth.find_service( uuid = uuid )
tm_discovery_1 = time.time()
print("    discovery in %.3f sec" % ( tm_discovery_1 - tm_discovery_0 ))

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
fcntl.fcntl(sock, fcntl.F_SETFL, os.O_NONBLOCK)

user_data_base = " " * 4000
send_max = 8
poll_extra_max = send_max * 4 # about: len(user_data_base)/1000 * send_max
total_data = 0
tm00 = time.time()
for n in range(send_max):
    user_data = "hello!! %d %s" % (n, user_data_base)
    sock.send(user_data)
    total_data += len(user_data)

    rb, rt, rl = recv(sock, 0.5)
    print("    recv len %d [%s]  in %.3f sec  %d loops" % ( len(rb), repr(rb), rt, rl))

last_cost = 0.0
for n in range(poll_extra_max):
    rb, rt, rl = recv(sock, 0.5)
    if rb is None:
        print("    recv [None]  in %.3f sec  %d loops" % ( rt, rl))
        last_cost = rt
        break
    print("    recv len %d [%s]  in %.3f sec  %d loops" % ( len(rb), repr(rb), rt, rl))

tm99 = time.time()

sock.close()

total_cost = tm99 - tm00 - last_cost
bps = total_data * 8.0 / total_cost
print("  total data %d  cost %.3f  bps %.3f" % ( total_data, total_cost, bps ))


