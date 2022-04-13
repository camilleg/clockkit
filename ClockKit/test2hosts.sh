#!/bin/bash

# Ensure that one server and one client run, on different hosts, and print plausible output.
#
# This script runs the server (ckserver) on a remote host specified in a config file
# (named config-remote.sh, if called by "make test-remote"),
# and the client (ckphaselock) on localhost.
# It parses their timestamps, and warns if they differ too much.
# After a few seconds, it kills both the server and the client.
#
# For a simpler example of syncing two hosts,
# in README.md see the section "To sync host B to host A."

. test-common.sh

if [[ $# -ne 1 ]]; then
  echo "usage: $0 <configfile.sh>" >&2
  exit 1
fi
if [ ! -s "$1" ]; then
  echo "$0: Please copy config-remote.example.sh to $1 and edit it."
  exit 1
fi
. "$1"

srv=$(mktemp /tmp/clockkit.XXXXXX)
cli=$(mktemp /tmp/clockkit.XXXXXX)
conf=$(mktemp /tmp/clockkit.XXXXXX)
# Clean up after all possible exits.
trap "rm -f $srv $cli $conf" 0 2 3 15

sed -e "s/^server:.*/server:$host/" \
    -e "s/^port:.*/port:$port/" \
    -e "s/^timeout:.*/timeout:${timeout_msec}000/" \
    < clockkit.conf > "$conf"
nuke ckphaselock
ssh="ssh -p $sshport -o ConnectTimeout=2 $host" # Abort after 2 seconds.
./ckphaselock $conf 0 > /dev/null # Kill any ckserver already listening to $host:$port.
$ssh "$dirRemote/ckserver" "$address" "$port" > "$srv" &
sleep 1.8 # Reduce timeouts during startup.
./ckphaselock "$conf" 5 > "$cli"
# If firewalls block packets, then ckserver hears nothing.
a=$(tail -10 "$srv" | grepregex '<time \d+ +\d+>\s')
b=$(tail -20 "$cli" | grepregex '<time \d+ +\d+>')
c=$(tail -20 "$cli" | grepregex 'offset: [-\d]+')
if [[ "$a $b $c" != "10 10 10" ]]; then
  echo "$0 $1: unexpected outputs" >&2
  # Save the output, to debug.
  cp "$srv" /tmp/clockkit-failed-srv.txt
  cp "$cli" /tmp/clockkit-failed-cli.txt
  # c can fail to be 10, when $cli has
  #   offset: invalid
  #   <time 2077252342 775807>
  exit 1
fi

# Parse the end of $srv and $cli.

# The timestamps of cli and srv should be close.
a=$(( $(tail -1 "$srv" | ./parse.rb time) ))
b=$(( $(tail -1 "$cli" | ./parse.rb time) ))
diff=$((a - b))
diffAbs=$(( ${diff#-} )) # Get absolute value by dropping the hyphen.
if [[ $diffAbs -gt 300000 ]]; then
  echo "$0 $1: clocks too different" >&2
  exit 1
fi

# The biggest offset (already abs()'d) should be small.
a=$(( $(tail -1 "$srv" | ./parse.rb offsetMax) ))
if [[ $a -gt 50000 ]]; then
  echo "$0 $1: offsetMax $a too large" >&2
  exit 1
fi

# The final offset's abs() should be small.
a=$(( $(grep offset "$cli" | tail -1 | ./parse.rb offset) ))
if [[ $a -gt 100000 ]]; then
  echo "$0 $1: final offset $a too large" >&2
  exit 1
fi
