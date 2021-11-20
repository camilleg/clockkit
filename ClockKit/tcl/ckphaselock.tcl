#!/usr/bin/env tclsh
load ./clockkit.so clockkit

if {$argc != 2} {
  puts "Usage: $argv0 configfile seconds"
  exit 1
}
# 0 seconds means infinity.
set sec_remaining [lindex $argv 1]
set terminate [expr {$sec_remaining > 0.0}]
if {!$terminate} {
  set sec_remaining 1.0
}

# To trap signals, install the package Expect, and:
#package require Expect
#proc signal_handler {} { exit 0 }
#trap signal_handler SIGINT
#trap signal_handler SIGTERM

ckInitialize [lindex $argv 0]

while {$sec_remaining > 0.0} {
  if {[ckInSync]} {
    set offset [ckOffset]
    set timestamp [ckTimeAsString]
    set timestamp_num [ckTimeAsValue]
    # timestamp_num might be 10 usec later than timestamp.
    puts "offset: $offset\n$timestamp"
  } else {
    puts "offset: OUT OF SYNC"
  }
  # msec
  after 100
  if {$terminate} {
    set sec_remaining [expr {$sec_remaining - 0.1}]
  }
  # puts stderr $sec_remaining
}

if {$terminate} {
  # Also kill ckserver.
  ckKill
} else {
  ckTerminate
}
