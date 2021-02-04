#!/bin/bash

srv=$(mktemp /tmp/clockkit.XXXXXX)
cli=$(mktemp /tmp/clockkit.XXXXXX)
# Clean up after all possible exits.
trap "rm -f $srv $cli" 0 2 3 15

killall -q -w ckserver
pkill -f 'ruby ./ckphaselock.rb'
../ckserver 4444 > $srv &
./ckphaselock.rb > $cli &
sleep 3
killall -w ckserver
pkill -f 'ruby ./ckphaselock.rb'
a=$(tail -4 $srv | grep -c -P '<time \d+ \d+>\s')
b=$(tail -4 $cli | grep -c -P 'offset: [-\d]+')
c=$(tail -4 $cli | grep -c -P 'time: <time \d+ \d+>')
if [[ "$a $b $c" == "4 2 2" ]]; then
  # Test passed.
  exit 0
fi
exit 1
