#!/usr/bin/env ruby
require './SyncClock'

# Statistics for error bound and phase.

def clear_stats
  $phase_sum = 0.0
  $errbd_sum = 0.0
  $phase_count = 0
  $errbd_count = 0
  $errbd_max = 0.0
end

def update_stats phase, errbd
  return unless errbd
  $phase_sum += phase
  $errbd_sum += errbd
  $phase_count += 1
  $errbd_count += 1
  $errbd_max = errbd if $errbd_max < errbd
end

def print_stats_header
  puts "# min_rtt\tmax_rtt\tavg_skew\tavg_err\tmax_err"
end
def print_stats
  phase_avg = $phase_sum / $phase_count
  errbd_avg = $errbd_sum / $errbd_count
  puts "#{$phase_detector.min_rtt}\t#{$phase_detector.max_rtt}\t#{phase_avg}\t#{errbd_avg}\t#{$errbd_max}"
end

# Low-level simulation

$reference = Clock.new
def run_simulation minutes
  advance_interval = 100000
  (minutes * 60).times {
    ($REFERENCE_FREQ / advance_interval).times {
      Clock.advance advance_interval
      update_stats ($reference.time - $sync.time).abs, $sync.error_bound&.abs
    }
    $sync.update
  }
end

# High-level scenario

print_stats_header
0.step(900,100) { |i|
  $reference = Clock.new
  local      = Clock.new
  local.drift = 0 # up to 200 usec, e.g.
  local.variability = 10
  $phase_detector = PhaseDetector.new $reference
  $sync = SyncClock.new local, $phase_detector
  $phase_detector.min_rtt = i
  $phase_detector.max_rtt = i + 100
  #$phase_detector.failure_rate = 0.0 # probability of failure, 0 to 1

  clear_stats
  run_simulation 10000 # number of minutes
  print_stats
}
