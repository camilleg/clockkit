#!/usr/bin/env python3
import time
import _clockkit
_clockkit.ckInitialize
while 1:
    if _clockkit.ckInSync():
        print("offset:", _clockkit.ckOffset(), "\ntime:", _clockkit.ckTimeAsString())
    else:
        print("offset: OUT OF SYNC")
    time.sleep(1)
