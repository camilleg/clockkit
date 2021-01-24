require './PhaseDetector'
require './VariableFrequencyClock'

class SyncClock
  def initialize local_clock, phase_detector
    raise 'local_clock lacks time()' unless local_clock.respond_to? :time
    @clock = VariableFrequencyClock.new local_clock
    @phase_detector = phase_detector
    @last_measure = nil
    @discipline_interval = 1000000 # 1 second
    @running_freq = $REFERENCE_FREQ
    @worst_possible_phase_error = nil
    @worst_possible_freq_error = nil
    @FREQ_ADJUST_CONST = 0.2
    @PHASE_ADJUST_CONST = 0.9
  end

  attr_reader :running_freq

  # Called periodically by the thread scheduler.
  # Decide if it is time to discipline the clock.
  def update
    discipline if !@last_measure ||
      @clock.time - @last_measure.local_time > @discipline_interval
  end

  def discipline
    this_measure = @phase_detector.measure @clock
    return unless this_measure
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
    freq_error_bound = (error0 + error1) * $REFERENCE_FREQ / interval
    freq_diff = @clock.freq - freq
    @running_freq += (@FREQ_ADJUST_CONST * freq_diff).to_i
    @worst_possible_freq_error = (@running_freq - freq).abs + freq_error_bound.abs

    phase_freq_adjust = phase1 * $REFERENCE_FREQ / interval
    phase_freq_adjust = (phase_freq_adjust * @PHASE_ADJUST_CONST).to_i

    @clock.freq = @running_freq + phase_freq_adjust
    @worst_possible_phase_error = phase1.abs + error1.abs
    @last_measure = this_measure
  end

  def time
    @clock.time
  end

  # Return the error bound on the time measurement.
  def error_bound
    return nil unless @worst_possible_phase_error && @worst_possible_freq_error && @last_measure

    elapsed = @clock.time - @last_measure.local_time

    # the last known phase offset
    # the last known phase offset error
    # the last known frequency error
    # time since the last frequency measurement
    # clock variability assumption

    freq_error = elapsed * @worst_possible_freq_error / $REFERENCE_FREQ + @worst_possible_phase_error
  end
end
