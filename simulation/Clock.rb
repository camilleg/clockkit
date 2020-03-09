$REFERENCE_FREQ = 1000000

class Clock

private

  @@clocks = []
  
public

  def initialize
    @time = 0
    @variability = 0  # PPM random +/- frequency error
    @drift = 0        # PPM steady rate phase drift
    @@clocks << self
  end

  attr_accessor :time, :drift, :variability

  def to_s()
    @time.to_s
  end
  
  def Clock.advance_all(usecs)
    @@clocks.each { |c| c.advance(usecs) }
  end

  def advance(usecs)
    freq = $REFERENCE_FREQ + drift;
    freq += rand(@variability * 2 + 1) - @variability
    @time += (usecs * freq / $REFERENCE_FREQ).to_i
  end

end
