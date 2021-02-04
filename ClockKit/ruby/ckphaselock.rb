#!/usr/bin/env ruby
require './clockkit'
Clockkit.ckInitialize
loop { 
  if Clockkit.ckInSync then
    puts "offset: #{Clockkit.ckOffset}\ntime: #{Clockkit.ckTimeAsString}"
  else
    puts "offset: OUT OF SYNC"
  end
  sleep 1
}