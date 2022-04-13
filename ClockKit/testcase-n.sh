#!/bin/bash

# Ensure that one server and n clients run, and print plausible output.

. test-common.sh

if [[ $# -eq 0 ]]; then
  exit 1
fi
n="$1"
if [[ $# -ge 2 ]]; then
  port="$2"
else
  port=4444
fi

conf=$(mktemp /tmp/clockkit.conf.XXX)
srv=$(mktemp /tmp/clockkit.srv.XXX)
clis=''
for i in $(seq "$n"); do
  cli[i]=$(mktemp /tmp/clockkit.cli.XXX)
  clis+="${cli[i]}"
  clis+=' '
done
# Clean up after all possible exits.
trap "rm -f $conf $srv $clis" 0 2 3 15

sed "s/^port:.*/port:$port/" < clockkit.conf > "$conf"
nuke ckserver ckphaselock

./ckserver "$port" > "$srv" &
for i in $(seq "$n"); do
  ./ckphaselock "$conf" > "${cli[i]}" &
done

sleep 4
nuke ckserver ckphaselock

a=$(tail -10 "$srv" | grepregex '<time \d+ +\d+>\s')
if [[ "$a" != "10" ]]; then
  exit 1
fi

for i in $(seq "$n"); do
  b=$(tail -20 "${cli[i]}" | grepregex '<time \d+ +\d+>')
  c=$(tail -20 "${cli[i]}" | grepregex 'offset: [-\d]+')
  if [[ "$b $c" != "10 10" ]]; then
    exit 2
  fi
done
