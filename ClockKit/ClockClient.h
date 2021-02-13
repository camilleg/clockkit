#pragma once
#include <cc++/socket.h>
#include "Clock.h"
#include "ClockPacket.h"
#include "Common.h"

using namespace ost;

namespace dex {

/**
 * A clock that gets its time over the network from a ClockServer.
 * This class will throw a ClockException if there are problems communicating
 * with the ClockServer.
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
    ClockClient(InetHostAddress addr, int port);

    ~ClockClient()
    {
        delete socket_;
    }

    // Get the ClockServer's "current" time.
    // Slower and less accurate than getPhase().
    // Calls getPhase(HighResolutionClock::instance()).
    timestamp_t getValue();

    /**
     * Returns the phase between a local clock and
     * the ClockServer clock.
     * This call is the most accurate in getting timing
     * information from a ClockServer.
     * - Throws ClockException on network timeout.
     *
     * If set to send acknowledgments, this call will send
     * back information to the ClockServer on the phase
     * difference of the two clocks.
     */
    timestamp_t getPhase(Clock& c);

    int getTimeout() const
    {
        return timeout_;
    }
    void setTimeout(int timeout)
    {
        timeout_ = timeout;
    }

    int getLastRTT() const
    {
        return lastRTT_;
    }

    // If true, getPhase() will send the server ACKNOWLEDGE packets,
    // so it can track a distributed clock's total error bound.
    void setAcknowledge(bool acknowledge)
    {
        acknowledge_ = acknowledge;
    }

   private:
    ClockClient(ClockClient& c);
    ClockClient& operator=(ClockClient& rhs);

    int timeout_;  // Timeout (usec).  Sets the max error on phase calculations.
    int lastRTT_;  // The last call's round trip time (usec).
    unsigned char sequence_;
    bool acknowledge_;
    UDPSocket* socket_;

    void sendPacket(ClockPacket& packet);

    // Receives the packet and sets the receipt time via the provided clock.
    ClockPacket receivePacket(Clock& clock);

    timestamp_t getPhase(Clock& clock, bool acknowledge);
};

}  // namespace dex
