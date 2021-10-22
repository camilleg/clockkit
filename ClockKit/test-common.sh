#!/bin/bash

# Utilities for test*.sh.

case "$(uname -s)" in
  Linux)
    # Linux or WSL.  WSL separately may need uname -r.
    nuke() { killall -q -w "$@" ; }
    grepregex() { grep -c -P "$@" ; }
    ;;
  Darwin)
    # MacOS.
    nuke() { killall "$@" 2> /dev/null || true ; }
    grepregex() { grep -c -E "$@" ; }
    ;;
  CYGWIN*|MINGW32*|MSYS*|MINGW*)
    # Windows.
    nuke() { echo "Tests NYI in Windows" ; }
    grepregex() { echo "Tests NYI in Windows" ; }
    ;;
  *)
    # Unrecognized.  See https://en.wikipedia.org/wiki/Uname '-s'.
    nuke() { echo "Tests NYI" ; }
    grepregex() { echo "Tests NYI" ; }
    ;;
esac
