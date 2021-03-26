#!/usr/bin/env ruby
require './clockkit'

abort "Usage: #$0 configfile" if ARGV.size != 1
Clockkit.ckInitialize ARGV[0]

loop { 
  if Clockkit.ckInSync then
    puts "offset: #{Clockkit.ckOffset}\n#{Clockkit.ckTimeAsString}"
  else
    puts "offset: OUT OF SYNC"
  end
  sleep 0.1
}
