#!/usr/bin/env tclsh
load ./clockkit.so clockkit
ckInitializeFromConfig "../clockkit.conf"

while { 1 } {
  if { [ckInSync] } {
    set offset [ckOffset]
    set timestamp [ckTimeAsString]
    puts "offset: $offset\ntime: $timestamp"
  } else {
    puts "offset: OUT OF SYNC"
  }
  after 1000
}
