require './Clock'

# Run at the requested frequency, .freq=, based on some other clock.

class VariableFrequencyClock

  def initialize( master_clock )  
    raise "master_clock missing time" if not master_clock.respond_to? :time
    @slave_freq     = $REFERENCE_FREQ
    @master_clock   = master_clock
    @master_marker  = @master_clock.time
    @slave_marker   = 0
  end
  
  def freq
    @slave_freq
  end

  def time
    master_ticks = @master_clock.time - @master_marker
    slave_ticks = (master_ticks * @slave_freq / $REFERENCE_FREQ).to_i
    @slave_marker + slave_ticks
  end

  def freq=(value)
    # Update the markers.
    master_ticks = @master_clock.time - @master_marker
    slave_ticks = (master_ticks * @slave_freq / $REFERENCE_FREQ).to_i
    @master_marker = @master_clock.time
    @slave_marker += slave_ticks
    
    # Set the frequency.
    @slave_freq = value
  end

  def time=(value)
    @master_marker = @master_clock.time
    @slave_marker = value
  end

  def to_s
    time.to_s
  end

end
