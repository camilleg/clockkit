#!/bin/bash
# Ensure that one server and one client run, and print plausible output.
# Fancier tests would run more clients, parse the outputs,
# and measure how close they are.

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

sed "s/^port:.*/port:$port/" < clockkit.conf > $conf
killall -q -w ckserver ckphaselock
./ckserver $port > $srv &
./ckphaselock $conf 3 > $cli
a=$(tail -10 $srv | grep -c -P '<time \d+ +\d+>\s')
b=$(tail -20 $cli | grep -c -P '<time \d+ +\d+>')
c=$(tail -20 $cli | grep -c -P 'offset: [-\d]+')
if [[ "$a $b $c" != "10 10 10" ]]; then
  echo "$0: unexpected outputs" >&2
  exit 1
fi

# Parse the end of $srv and $cli.

# The timestamps of cli and srv should be close.
a=$(( $(tail -1 $srv | ./parse.rb time) ))
b=$(( $(tail -1 $cli | ./parse.rb time) ))
diff=$(( $a - $b ))
diffAbs=$(( ${diff#-} )) # Get absolute value by dropping the hyphen.
if [[ $diffAbs -gt 300000 ]]; then
  echo "$0: clocks too different" >&2
  exit 1
fi

# offsetMax (already abs()'d) should be small.
a=$(( $(tail -1 $srv | ./parse.rb offsetMax) ))
if [[ $a -gt 500 ]]; then
  echo "$0: offsetMax too large" >&2
  exit 1
fi

# Final offset should be small.
a=$(( $(grep offset $cli | tail -1 | ./parse.rb offset) ))
if [[ $a -gt 50 ]]; then
  echo "$0: final offset too large" >&2
  exit 1
fi
