# Simulate a networked phase detector between two clocks.
# Phase is defined as (reference - local).

class PhaseDetector
  def initialize remote_clock
    raise 'remote_clock lacks time()' unless remote_clock.respond_to? :time
    @remote_clock = remote_clock
    @min_rtt = 100      # usec (0.1 msec)
    @max_rtt = 1000     # usec (1.0 msec)
    @failure_rate = 0.0 # phase detector failure rate (1.0 = 100%)
  end

  # RTT means round trip time.
  attr_accessor :min_rtt, :max_rtt, :failure_rate

  # Make a measurement and set @phase and @error_bound.
  # This measurement is VERY jittery, assuming no
  # symmetric communication delays (i.e., worst case).
  # Return nil when measurement fails.
  def measure local_clock
    return nil if rand < @failure_rate
    rtt = rand(@max_rtt - @min_rtt) + @min_rtt
    error_bound = (rtt/2).to_i
    error = rand(error_bound * 2 + 1) - error_bound
    phase = @remote_clock.time - local_clock.time + error
    PhaseMeasurement.new phase, error_bound, local_clock.time
  end
end

class PhaseMeasurement
  def initialize phase, error_bound, local_time
    @phase = phase
    @error_bound = error_bound
    @local_time = local_time
  end

  attr_reader :phase, :error_bound, :local_time
end
