#pragma once
#include <cc++/socket.h>
#include <limits>

#include "Clock.h"
#include "ClockPacket.h"
#include "Common.h"

namespace dex {

/**
 * A clock that gets its time over the network from a ClockServer.
 *
 * To get the time, the clock sends a timestamped packet to to the server.
 * The server responds with its current time.  The client then timestamps
 * the response.  The client can then calculate an estimated offset between
 * the two clocks as well as an error bound on this calculation.
 *
 * Because of this, getPhase() should really be used on a ClockClient.
 * However, getValue() can be used.  getValue() calculates the current time
 * by using the HighFrequencyClock as a timekeeper.
 *
 * Calls to ClientClock may take some time since a server response
 * is required.
 */
class ClockClient : public Clock {
   public:
    // Connect to a host and port.
    explicit ClockClient(ost::InetHostAddress addr, int port);

    ~ClockClient()
    {
        delete socket_;
    }

    // Get the ClockServer's "current" time.
    // Slower and less accurate than getPhase().
    // Calls getPhase(HighResolutionClock::instance()).
    // Returns "invalid" on error.
    timestamp_t getValue();

    inline int getTimeout() const
    {
        return timeout_;
    }
    inline void setTimeout(int timeout)
    {
        timeout_ = timeout;
    }

    inline int rtt() const
    {
        return rtt_;
    }

    // If true, getPhase() will send the server ACKNOWLEDGE packets,
    // so it can track a distributed clock's total error bound.
    inline void setAcknowledge(bool acknowledge)
    {
        acknowledge_ = acknowledge;
    }

    // Kill the connected ClockServer.
    void die() const
    {
        (void)sendPacket(ClockPacket(ClockPacket::KILL, 0, 0));
    }

    // Typically 9223372036854775807 usec, or 293,000 years, obviously invalid.
    static constexpr auto invalid = std::numeric_limits<timestamp_t>::max();

   private:
    explicit ClockClient(ClockClient&);
    ClockClient& operator=(ClockClient&);

    int timeout_;  // Timeout (usec).  Sets the max error on phase calculations.
    size_t rtt_;   // The previous call's round trip time (usec).
    unsigned char sequence_;
    bool acknowledge_;
    ost::UDPSocket* socket_;

    bool sendPacket(const ClockPacket&) const;

    // Receives the packet and sets the receipt time via the provided clock.
    ClockPacket receivePacket(Clock&);

    // Returns "invalid" on error.
    timestamp_t getPhase(Clock&, bool acknowledge);

    /**
     * Returns the phase between a local clock and
     * the ClockServer clock.
     * This call is the most accurate in getting timing
     * information from a ClockServer.
     *
     * If set to send acknowledgments, this call will send
     * back information to the ClockServer on the phase
     * difference of the two clocks.
     */
   public:
    inline timestamp_t getPhase(Clock& clock)
    {
        return getPhase(clock, acknowledge_);
    }
};

}  // namespace dex
