#!/usr/bin/env python

import bluetooth

server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )

ports = bluetooth.bluez._get_available_ports( bluetooth.RFCOMM )
print("    ports len %d" % len(ports))
#port = ports[0]
port = bluetooth.PORT_ANY

server_sock.bind(("",port))
server_sock.listen(1)

port = server_sock.getsockname()[1]
print "listening on port %d" % port

##uuid = "1e0ca4ea-299d-4335-93eb-27fcfe7fa848"
##bluetooth.advertise_service( server_sock, "FooBar Service", uuid )

uuid = "00000001-0001-11e8-8001-f4844c401234"
bluetooth.advertise_service( server_sock, "SampleServer",
               service_id = uuid,
               service_classes = [ uuid, bluetooth.SERIAL_PORT_CLASS ],
               profiles = [ bluetooth.SERIAL_PORT_PROFILE ],
#                   protocols = [ OBEX_UUID ]
                )

print "accepting on port %d" % port

client_sock,address = server_sock.accept()
print "Accepted connection from ",address

data = client_sock.recv(1024)
print "received [%s]" % data

client_sock.close()
server_sock.close()


