#!/usr/bin/env ruby
require './clockkit'

abort "Usage: #$0 configfile" if ARGV.size != 1
Clockkit.ckInitializeFromConfig ARGV[0]

loop { 
  if Clockkit.ckInSync then
    puts "offset: #{Clockkit.ckOffset}\ntime: #{Clockkit.ckTimeAsString}"
  else
    puts "offset: OUT OF SYNC"
  end
  sleep 1
}
