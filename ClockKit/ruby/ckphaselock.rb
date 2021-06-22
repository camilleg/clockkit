#!/usr/bin/env ruby
require './clockkit'

ARGC = ARGV.size
abort "Usage: #$0 configfile seconds" unless ARGC == 2
# 0 seconds means infinity.
sec_remaining = ARGV[1].to_f
terminate = sec_remaining > 0.0
sec_remaining = 1.0 unless terminate

# Trap ^C's SIGINT and pkill's SIGTERM,
# and pass the signal to clockkit.cpp's atexit(), which calls Clockkit.ckTerminate.
Signal.trap('INT' ) { exit }
Signal.trap('TERM') { exit }

Clockkit.ckInitialize ARGV[0]

while sec_remaining > 0.0
  if Clockkit.ckInSync then
    puts "offset: #{Clockkit.ckOffset}\n#{Clockkit.ckTimeAsString}"
  else
    puts "offset: OUT OF SYNC"
  end
  sleep 0.1
  sec_remaining -= 0.1 if terminate
  # STDERR.puts sec_remaining
end

if terminate
  # Also kill ckserver.
  Clockkit.ckKill
else
  Clockkit.ckTerminate
end
