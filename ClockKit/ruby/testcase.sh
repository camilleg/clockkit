#!/bin/bash

if [[ $# -ge 1 ]]; then
  port=$1
else
  port=4444
fi

srv=$(mktemp /tmp/clockkit.XXXXXX)
cli=$(mktemp /tmp/clockkit.XXXXXX)
conf=$(mktemp /tmp/clockkit.XXXXXX)
# Clean up after all possible exits.
trap "rm -f $srv $cli $conf" 0 2 3 15

sed "s/^port:.*/port:$port/" < ../clockkit.conf > $conf
pkill -f 'ruby ./ckphaselock.rb'
pkill -f "ckserver $port"
../ckserver $port > $srv &
./ckphaselock.rb $conf > $cli &
sleep 2
pkill -f 'ruby ./ckphaselock.rb'
pkill -f "ckserver $port"
a=$(tail -10 $srv | grep -c -P '<time \d+ \d+>\s')
b=$(tail -20 $cli | grep -c -P 'offset: [-\d]+')
c=$(tail -20 $cli | grep -c -P 'time: <time \d+ \d+>')
if [[ "$a $b $c" == "10 10 10" ]]; then
  # Test passed.
  exit 0
fi
exit 1
