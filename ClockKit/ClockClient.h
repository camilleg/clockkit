#pragma once
#include <limits>

#include "Clock.h"
#include "ClockPacket.h"

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
 */
class ClockClient : public Clock {
   public:
    // Connect to a host and port.
    explicit ClockClient(const kissnet::endpoint&);

    ~ClockClient()
    {
    }

    // Another client with the same host:port would be dangerous, not useful.
    ClockClient(const ClockClient&) = delete;
    ClockClient& operator=(const ClockClient&) = delete;

    // Get the ClockServer's "current" time.
    // Slower and less accurate than getPhase().
    // Calls getPhase(SystemClock::instance()).
    // Returns "invalid" on error.
    tp getValue();

    int getTimeout() const
    {
        return UsecFromDur(timeout_);
    }
    void setTimeout(int64_t usec)
    {
        if (usec != usecInvalid)
            timeout_ = DurFromUsec(usec);
    }

    dur rtt() const
    {
        return rtt_;
    }

    // If true, when getPhase() is called externally (from PhaseLockedClock.cpp)
    // instead of internally by getValue(),
    // it finishes by sending the server an ACKNOWLEDGE packet,
    // for the server to track the total error bound.
    void setAcknowledge(bool acknowledge)
    {
        acknowledge_ = acknowledge;
    }

    // Phase between a local clock and a ClockServer's clock.
    // Reports the phase to the server, if acknowledge_.
    // The most accurate way to get timing from a ClockServer.
    dur getPhase(Clock& clock)
    {
        return getPhase(clock, acknowledge_);
    }

    // Kill the connected ClockServer.
    void die()
    {
        (void)sendPacket(ClockPacket(ClockPacket::KILL));
    }

   private:
    dur timeout_;  // The max error on phase calculations.
    dur rtt_;      // The previous call's round trip time.
    seqnum sequence_;
    bool acknowledge_;
    kissnet::udp_socket socket_;

    bool sendPacket(const ClockPacket&);

    // Receives the packet and sets the receipt time via the provided clock.
    // On error, the returned packet's type is INVALID.
    ClockPacket receivePacket(Clock&);

    // Returns durInvalid on error.
    dur getPhase(Clock&, bool acknowledge);
};

}  // namespace dex
