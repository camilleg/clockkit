#!/usr/bin/env ruby

require './clockkit'

Clockkit.ckInitialize

loop { 
  if Clockkit.ckInSync then
    puts "offset=" + Clockkit.ckOffset.to_s + ", timestamp=" + Clockkit.ckTimeAsString
  else
    puts "offset: OUT OF SYNC"
  end
  sleep 1
}
