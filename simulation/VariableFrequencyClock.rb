require './Clock'

# Run at the requested frequency, .freq=, based on some other clock.

class VariableFrequencyClock
  def initialize master_clock
    raise 'master_clock lacks time' unless master_clock.respond_to? :time
    @slave_freq    = $REFERENCE_FREQ
    @master_clock  = master_clock
    @master_marker = @master_clock.time
    @slave_marker  = 0
  end

  def time= value
    @master_marker = @master_clock.time
    @slave_marker = value
  end

  def time
    master_ticks = @master_clock.time - @master_marker
    slave_ticks = (master_ticks * @slave_freq / $REFERENCE_FREQ).to_i
    @slave_marker + slave_ticks
  end

  def to_s
    time.to_s
  end

  def freq= value
    # Update the markers.
    @slave_marker = time
    @master_marker = @master_clock.time
    # Set the frequency.
    @slave_freq = value
  end

  def freq
    @slave_freq
  end
end
