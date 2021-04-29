#!/usr/bin/env python3
import sys
import time
import _clockkit

if len(sys.argv) != 2:
    sys.exit("Usage: %s configfile" % sys.argv[0])
_clockkit.ckInitialize(sys.argv[1])

while 1:
    if _clockkit.ckInSync():
        print("offset:", _clockkit.ckOffset(), "\n", _clockkit.ckTimeAsString())
    else:
        print("offset: OUT OF SYNC")
    time.sleep(0.1)

_clockkit.ckTerminate()
