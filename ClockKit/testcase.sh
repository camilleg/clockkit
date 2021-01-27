#!/bin/bash

# Ensure that one server and one client run, and print plausible output.
# Fancier tests would run more clients, parse the outputs,
# and measure how close they are.

cli=$(mktemp /tmp/clockkit.XXXXXX)
srv=$(mktemp /tmp/clockkit.XXXXXX)
# Clean up after all possible exits.
trap "rm -f $srv $cli" 0 2 3 15

killall -q -w ckserver ckphaselock
./ckserver 4444 > $srv &
./ckphaselock > $cli &
sleep 3
killall -q -w ckserver ckphaselock
a=$(tail -4 $srv | grep -c -P '<time \d+ \d+>\s')
b=$(tail -4 $cli | grep -c -P 'offset: [-\d]+')
c=$(tail -4 $cli | grep -c -P 'time: <time \d+ \d+>')
if [[ "$a $b $c" == "4 2 2" ]]; then
  # Test passed.
  exit 0
fi
exit 1
