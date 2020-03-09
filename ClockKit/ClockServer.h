//----------------------------------------------------------------------------//
#ifndef DEX_CLOCK_SERVER_H
#define DEX_CLOCK_SERVER_H
//----------------------------------------------------------------------------//
#include <cc++/thread.h>
#include <cc++/socket.h>
#include "Clock.h"
#include <string>
#include <map>
//----------------------------------------------------------------------------//
using namespace std;
using namespace ost;

namespace dex {
//----------------------------------------------------------------------------//


/**
 * A network server for a local clock.
 * The server accpts and answers requests on a UDP port.
 * It also stores responses from the clients on the state of their
 * synchronization.
 *
 * The server will not start untill it's thread is started.
 * To start up the server, call start().
 */
class ClockServer : public Thread
{
private:
    InetAddress addr_;
    tpport_t port_;
    Clock& clock_;
    
    struct Entry
    {
        timestamp_t time;
        int offset;
        int rtt;
    };
    
    map<string, Entry> ackData_;
    bool log_;
    
    static const timestamp_t SYSTEM_STATE_PURGE_TIME;
    timestamp_t lastUpdate_;

public:
    
    /** 
     * Create a new ClockServer.  The server will bind to the provided
     * InetAddress and port.  To bind to ANY local address, then set the
     * InetAddress to 0.0.0.0 .  The server will then use the provided clock
     * to answer requests for time.
     */
    ClockServer(InetAddress addr, tpport_t port, Clock& clock);

    
    void setLogging(bool log);
    
protected:
    
    /**
     * The thread calls this run method, which is responsible for
     * all network operations.
     */
    void run();
    
    void updateEntry(string addr, int offset, int rtt);
    
};

//----------------------------------------------------------------------------//
} // namespace dex

//----------------------------------------------------------------------------//
#endif //DEX_CLOCK_SERVER_H

