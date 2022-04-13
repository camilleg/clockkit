#!/bin/bash

# Ensure that one server and one client run, and print plausible output.

. test-common.sh

if [[ $# -ge 1 ]]; then
  port="$1"
else
  port=4444
fi

srv=$(mktemp /tmp/clockkit.XXXXXX)
cli=$(mktemp /tmp/clockkit.XXXXXX)
conf=$(mktemp /tmp/clockkit.XXXXXX)
# Clean up after all possible exits.
trap "rm -f $srv $cli $conf" 0 2 3 15

sed "s/^port:.*/port:$port/" < clockkit.conf > "$conf"
nuke ckserver ckphaselock
./ckserver "$port" > "$srv" &
./ckphaselock "$conf" 3 > "$cli"
a=$(tail -10 "$srv" | grepregex '<time \d+ +\d+>\s')
b=$(tail -20 "$cli" | grepregex '<time \d+ +\d+>')
c=$(tail -20 "$cli" | grepregex 'offset: [-\d]+')
if [[ "$a" != "10" ]]; then
  echo "$0: unexpected output from srv:" >&2
  cat "$srv" >&2
  exit 1
fi
if [[ "$b $c" != "10 10" ]]; then
  echo "$0: unexpected output from cli:" >&2
  cat "$cli" >&2
  exit 1
fi

# Parse the end of $srv and $cli.

# The timestamps of cli and srv should be close.
a=$(( $(tail -1 "$srv" | ./parse.rb time) ))
b=$(( $(tail -1 "$cli" | ./parse.rb time) ))
diff=$((a - b))
diffAbs=$(( ${diff#-} )) # Get absolute value by dropping the hyphen.
if [[ $diffAbs -gt 300000 ]]; then
  echo "$0: clock difference $diffAbs exceeds 300 ms" >&2
  exit 1
fi

# offsetMax was already abs()'d.
a=$(( $(tail -1 "$srv" | ./parse.rb offsetMax) ))
if [[ $a -gt 500 ]]; then
  echo "$0: offsetMax $a exceeds 500 μs" >&2
  exit 1
fi

a=$(( $(grep offset "$cli" | tail -1 | ./parse.rb offset) ))
if [[ $a -gt 50 ]]; then
  echo "$0: final offset $a exceeds 50 μs" >&2
  exit 1
fi
