#!/usr/bin/env tclsh
load ./clockkit.so clockkit

if {$argc != 1} {
  puts "Usage: $argv0 configfile"
  exit 1
}
ckInitialize [lindex $argv 0]

while {1} {
  if {[ckInSync]} {
    set offset [ckOffset]
    set timestamp [ckTimeAsString]
    puts "offset: $offset\ntime: $timestamp"
  } else {
    puts "offset: OUT OF SYNC"
  }
  after 1000
}
