#!/usr/bin/env ruby
# String parsing, in Ruby because bash is too cumbersome.
abort "usage: #$0 command < line-of-input" if ARGV.size != 1
L = STDIN.gets.chomp.split

case ARGV[0]
when 'time'
  # Filter "<time 1616703831  40503>..." into a usec value 1616703831040503.
  abort "#$0: unexpected input '#{L}'" if L.size < 3
  puts L[1].to_i * 1000000 + L[2].to_i

when 'offset'
  # Filter "offset: 19" into a usec value 19.
  abort "#$0: unexpected input '#{L}'" if L.size < 2
  puts L[1].to_i.abs

when 'offsetMax'
  # Filter "<time 1616776087 109179> offsetMax\t179\t---" into a usec value 179.
  abort "#$0: unexpected input '#{L}'" if L.size < 5
  puts L[4].to_i.abs

else
  abort "#$0: unexpected command '#{ARGV[1]}'"
end
