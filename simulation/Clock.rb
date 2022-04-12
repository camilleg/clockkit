$REFERENCE_FREQ = 1000000

class Clock
  def initialize
    @time = 0
    @variability = 0  # PPM random +- frequency error
    @drift = 0        # PPM steady rate phase drift
    @@clocks << self
  end

  attr_accessor :time, :drift, :variability

  def to_s
    @time.to_s
  end

  def advance usec
    freq = $REFERENCE_FREQ + drift + rand(@variability * 2 + 1) - @variability
    @time += (usec * freq / $REFERENCE_FREQ).to_i
  end

  def self.advance usec
    @@clocks.each { |c| c.advance usec }
  end

  @@clocks = []
end
