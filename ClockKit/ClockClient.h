
//----------------------------------------------------------------------------//
#ifndef DEX_CLOCK_CLIENT_H
#define DEX_CLOCK_CLIENT_H
//----------------------------------------------------------------------------//
#include <cc++/socket.h>
#include "Common.h"
#include "Clock.h"
#include "ClockPacket.h"
//----------------------------------------------------------------------------//
using namespace std;
using namespace ost;

namespace dex {
//----------------------------------------------------------------------------//

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
class ClockClient : public Clock
{
public:
    
    /**
     * Creates a client bound to localhost and an open port.
     * - The bind port is chosen by starting at 5000 and moving up.
     * - Default timeout is 1000 usec (1 msec).
     * - By default, the client does not send acknowledgments.
     *
     * @param addr internt address of the ClockClient.
     * @param port port the ClockClient is listening on.
     */
    ClockClient(InetHostAddress addr, int port);

    /**
     * Destructs the Client and closes the network socket.
     */
    ~ClockClient();

    // not all that accurate and takes longer
    // calls getPhase(HighResolutionClock::instance());
    /**
     * Gets the "current" time on the ClockServer.
     * This call is not all that accurate and can take quite
     * a long time.  Instead, use getPhase().
     * - Throws ClockException on network timeout.
     */
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
	
    /**
     * Gets the timeout value for calls on this clock.
     * If network operations take longer than this, then
     * a ClockException will be thrown.
     * The timeout value determines the maximum amount
     * of error on phase calculations.
     * @return timeout value in microseconds (usec).
     */
    int getTimeout();
    
    /**
     * Sets the timeout value for calls on this clock.
     * If network operations take longer than this, then
     * a ClockException will be thrown.
     * The timeout value determines the maximum amount
     * of error on phase calculations.
     * @param timeout timeout value in microseconds (usec).
     */ 
     void setTimeout(int timeout);
    
    /**
     * Gets the round-trip-time from the last call on this clock.
     * @return Last round-trip-time in microseconds (usec).
     */
    int getLastRTT();
        
    /**
     * Sets whether the ClockClient will send ACKNOWLEDGE packets
     * to the server on a getPhase() call.
     * This allows the ClockServer to keep track of the total
     * error bound of a distributed clock.
     */
    void setAcknowledge(bool acknowledge);
    
private:	
	ClockClient(ClockClient& c);
	ClockClient& operator=(ClockClient& rhs);

    int timeout_;
    unsigned char sequence_;
    int lastRTT_;
    bool acknowledge_;
    UDPSocket* socket_;


    void sendPacket(ClockPacket& packet);
    
    // receives the packet and set's the receipt time via the provided clock
    ClockPacket receivePacket(Clock& clock);

    timestamp_t getPhase(Clock& clock, bool acknowledge);


}; // class ClockClient

//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_CLOCK_CLIENT_H
//----------------------------------------------------------------------------//

