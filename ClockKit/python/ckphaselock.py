#!/usr/bin/env python3
import signal
import sys
import time

import _clockkit

argc = len(sys.argv)
if argc != 3:
    sys.exit("Usage: %s configfile seconds" % sys.argv[0])
# 0 seconds means infinity.
sec_remaining = float(sys.argv[2])
terminate = sec_remaining > 0.0
if not terminate:
    sec_remaining = 1.0

_clockkit.ckInitialize(sys.argv[1])

# Trap ^C's SIGINT and pkill's SIGTERM,
# and pass the signal to clockkit.cpp's atexit(), which calls Clockkit.ckTerminate.
def signal_handler(sig, frame):
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

sec_remaining = float(sys.argv[2]) if terminate else 1.0
while sec_remaining > 0.0:
    if _clockkit.ckInSync():
        print("offset:", _clockkit.ckOffset(), "\n", _clockkit.ckTimeAsString())
    else:
        print("offset: OUT OF SYNC")
    time.sleep(0.1)
    if terminate:
        sec_remaining -= 0.1
    # print(sec_remaining, file=sys.stderr)

_clockkit.ckTerminate()
