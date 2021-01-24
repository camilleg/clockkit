require './PhaseDetector'
require './VariableFrequencyClock'

class SyncClock

  @@POLL_INTERVAL_MULTIPLIER = 2**6
  @@TIME_CONSTANT = 1
  @@COMPLIANCE_MAX = 2**4

  def initialize local_clock, phase_detector
    @clock = VariableFrequencyClock.new local_clock
    @phase_detector = phase_detector
    @time_constant = @@TIME_CONSTANT
    @update_interval = @@POLL_INTERVAL_MULTIPLIER
    @compliance = @@COMPLIANCE_MAX
    @frequency_error = 0
    @phase_error = 0
    @last_measure = nil
  end

  # Return the requested time before next update.
  def update
    this_measure = @phase_detector.measure @clock
    return @update_interval unless this_measure
    unless @last_measure
      @last_measure = this_measure
      return @update_interval
    end
    interval = (this_measure.local_time + this_measure.phase) -
               (@last_measure.local_time + @last_measure.phase)
    next_frequency_error = @frequency_error + (interval * this_measure.phase / @time_constant**2)
    next_phase_error = this_measure.phase / @time_constant**2
    puts "#{next_frequency_error}\t#{next_phase_error}"
  end

  def discipline
    this_measure = @phase_detector.measure @clock
    unless this_measure
      @clock.freq = @running_freq
      return
    end
    unless @last_measure
      @last_measure = this_measure
      return
    end

    time0  = @last_measure.local_time
    time1  =  this_measure.local_time
    phase0 = @last_measure.phase
    phase1 =  this_measure.phase
    error0 = @last_measure.error_bound
    error1 =  this_measure.error_bound

    interval = (time1 + phase1) - (time0 + phase0)
    freq = (phase0 - phase1) * $REFERENCE_FREQ / interval + $REFERENCE_FREQ
    freq_diff = $REFERENCE_FREQ - freq

    @running_freq += (@FREQ_ADJUST_CONST * freq_diff).to_i

    phase_freq_adjust = phase1 * $REFERENCE_FREQ / interval
    phase_freq_adjust = (phase_freq_adjust * @PHASE_ADJUST_CONST).to_i
    @clock.freq = @running_freq + phase_freq_adjust

    @last_measure = this_measure
  end

  def time
    @clock.time
  end

  # Return the error bound on the time measurement.
  def error_bound
    # the last known phase offset
    # the last known phase offset error
    # the last known frequency error
    # time since the last frequency measurement
    # clock variability assumption
  end
end
