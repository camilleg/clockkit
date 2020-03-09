#!/usr/bin/env python3

import time

# This is broken after the move from Python 2 to 3.
from . import clockkit

clockkit.ckInitialize

while 1:
  if clockkit.ckInSync():
    print("offset=", clockkit.ckOffset(), ", timestamp=", clockkit.ckTimeAsString())
  else:
    print("offset: OUT OF SYNC")
  time.sleep(1)
