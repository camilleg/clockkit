#!/usr/bin/env ruby
require './clockkit'

abort "Usage: #$0 configfile" unless ARGV.size == 1
Clockkit.ckInitialize ARGV[0]

# Trap ^C's SIGINT and pkill's SIGTERM,
# and pass the signal to clockkit.cpp's atexit(), which calls Clockkit.ckTerminate.
Signal.trap('INT' ) { exit }
Signal.trap('TERM') { exit }

loop { 
  if Clockkit.ckInSync then
    puts "offset: #{Clockkit.ckOffset}\n#{Clockkit.ckTimeAsString}"
  else
    puts "offset: OUT OF SYNC"
  end
  sleep 0.1
}

Clockkit.ckTerminate
