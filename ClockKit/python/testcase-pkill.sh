#!/bin/bash

if [[ $# -ge 1 ]]; then
  port="$1"
else
  port=4444
fi

conf=$(mktemp /tmp/clockkit.conf.XXX)
srv=$(mktemp /tmp/clockkit.srv.XXX)
cli=$(mktemp /tmp/clockkit.cli.XXX)
# Clean up after all possible exits.
trap "rm -f $conf $srv $cli" 0 2 3 15

sed "s/^port:.*/port:$port/" < ../clockkit.conf > "$conf"
pkill -f "python3 ./ckphaselock.py $conf 0"
pkill -f "ckserver $port" # How to wait for it to die, like killall -w ?  Busywait until pgrep -c reports zero?
export PYTHONUNBUFFERED=TRUE # Show print()s even after pkill.
../ckserver "$port" > "$srv" &
./ckphaselock.py "$conf" 0 > "$cli" &
sleep 2
pkill -f "python3 ./ckphaselock.py $conf 0"
pkill -f "ckserver $port"
sed -i '/EXCEPTION/d' "$cli"
a=$(tail -10 "$srv" | grep -c -P '<time \d+ +\d+>\s')
b=$(tail -20 "$cli" | grep -c -P '<time \d+ +\d+>')
c=$(tail -20 "$cli" | grep -c -P 'offset: [-\d]+')
if [[ "$a $b $c" == "10 10 10" ]]; then
  # Test passed.
  exit 0
fi
exit 1
