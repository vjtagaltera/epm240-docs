#!/usr/bin/env python

from bluetooth.ble import DiscoveryService

service = DiscoveryService()
devices = service.discover(2)

for address, name in devices.items():
    print("Name: %s, address: %s" % (name, address))

