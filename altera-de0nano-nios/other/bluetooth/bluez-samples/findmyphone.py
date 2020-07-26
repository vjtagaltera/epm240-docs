#!/usr/bin/env python

import bluetooth

target_name = "My Phone"
target_address = None

nearby_devices = bluetooth.discover_devices()

print("  nearby_devices len %d" % len(nearby_devices))
for bdaddr in nearby_devices:
    print("    bdaddr type %s" % str(type(bdaddr)))
    bdtyp = type(bdaddr)
    if bdtyp is str or bdtyp is unicode:
        print("        %s" % bdaddr)
        bdnm = bluetooth.lookup_name( bdaddr )
        print("        %s" % bdnm)

    if target_name == bluetooth.lookup_name( bdaddr ):
        target_address = bdaddr
        break

if target_address is not None:
    print "found target bluetooth device with address ", target_address
else:
    print "could not find target bluetooth device nearby"


