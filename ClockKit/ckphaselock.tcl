#!/usr/bin/tclsh

load ./clockkit_tcl.so clockkit

ckInitialize;

while { 1 } {
  if { [ckInSync] } {
    set offset [ckOffset]
    set timestamp [ckTimeAsString]
    puts "offset=$offset timestamp=$timestamp"
  } else {
    puts "offset: OUT OF SYNC"
  }
  after 1000
}
