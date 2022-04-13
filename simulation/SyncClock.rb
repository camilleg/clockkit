require './PhaseDetector'
require './VariableFrequencyClock'

class SyncClock
  def initialize local_clock, phase_detector
    raise 'local_clock lacks time()' unless local_clock.respond_to? :time

    # clock1 disciplines frequency, clock2 phase.
    @clock1 = VariableFrequencyClock.new local_clock
    @clock2 = VariableFrequencyClock.new @clock1
    @phase_detector = phase_detector
    @clock1_measure = nil
    @clock2_measure = nil

    @frequency_discipline_interval = 30000000 # 30 seconds
    @phase_discipline_interval = 1000000 # 1 second
    @CLOCK_VARIABILITY_MAXIMUM = 100 # ppm, local clock drift between updates
    @last_freq_error = nil
  end

  # Called periodically by the thread scheduler.
  # Decide if it is time to discipline either clock.
  def update
    if @clock1_measure.nil?
      discipline_freq
    else
      elapsed = @clock1.time - @clock1_measure.local_time
      discipline_freq if elapsed > @frequency_discipline_interval
    end
    if @clock2_measure.nil?
      discipline_phase
    else
      elapsed = @clock2.time - @clock2_measure.local_time
      discipline_phase if elapsed > @phase_discipline_interval
    end
  end

  def discipline_freq
    measure = @phase_detector.measure(@clock1)
    return if measure.nil?
    if @clock1_measure.nil?
      @clock1_measure = measure
      return
    end

    time0 = @clock1_measure.local_time
    time1 = measure.local_time
    phase0 = @clock1_measure.phase
    phase1 = measure.phase
    error0 = @clock1_measure.error_bound
    error1 = measure.error_bound

    interval = (time1 + phase1) - (time0 + phase0)
    freq = (phase0 - phase1) * $REFERENCE_FREQ / interval + $REFERENCE_FREQ
    freq_error_bound = (error0 + error1) * $REFERENCE_FREQ / interval
    freq_diff = $REFERENCE_FREQ - freq
    @last_freq_error = freq_diff.abs + freq_error_bound.abs

    if freq_diff > 0 && freq_diff > freq_error_bound
      conservative_freq_diff = freq_diff - freq_error_bound
    elsif freq_diff < 0 && freq_diff < -freq_error_bound
      conservative_freq_diff = freq_diff + freq_error_bound
    else
      conservative_freq_diff = 0
    end

    @clock1.freq = @clock1.freq + conservative_freq_diff
    @clock1_measure = measure
  end

  def discipline_phase
    measure = @phase_detector.measure(@clock2)
    if measure.nil?
      @clock2.freq = $REFERENCE_FREQ
      return
    end
    if @clock2_measure.nil?
      @clock2_measure = measure
      return
    end

    phase1 = measure.phase
    error1 = measure.error_bound

    if phase1 > 0 && phase1 > error1
      conservative_phase = phase1 - error1
    elsif phase1 < 0 && phase1 < -error1
      conservative_phase = phase1 + error1
    else
      conservative_phase = 0
    end

    @clock2.freq = (conservative_phase * $REFERENCE_FREQ / @phase_discipline_interval.to_f).to_i + $REFERENCE_FREQ
    @clock2_measure = measure
  end

  def time
    @clock2.time
  end

  # Return the error bound on the time measurement.
  def error_bound
    return nil if @clock2_measure.nil? || @last_freq_error.nil?
    interval = @clock2.time - @clock2_measure.local_time
    phase_error = @clock2_measure.phase.abs + @clock2_measure.error_bound.abs
    freq_error = @last_freq_error + @CLOCK_VARIABILITY_MAXIMUM
    (phase_error + freq_error * (interval.to_f / $REFERENCE_FREQ)).to_i
  end
end
