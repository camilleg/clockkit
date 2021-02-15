#pragma once
#include <cc++/socket.h>
#include <cc++/thread.h>
#include <map>
#include <string>
#include "Clock.h"

using namespace ost;

namespace dex {

// A network server for a local clock.
// It stores responses from the clients on the state of their synchronization.
// To start the server's thread (run()), call start().
class ClockServer : public Thread {
   private:
    const InetAddress addr_;
    const tpport_t port_;
    Clock& clock_;

    struct Entry {
        timestamp_t time;
        int offset;
        int rtt;
    };
    std::map<std::string, Entry> ackData_;

    bool log_;

    static const timestamp_t SYSTEM_STATE_PURGE_TIME;
    timestamp_t lastUpdate_;

   public:
    // Provide requests for timestamps to clients via a UDP port.
    // To accept connections from clients on *any* local address,
    // set InetAddress to 0.0.0.0.
    ClockServer(InetAddress addr, tpport_t port, Clock& clock);

    void setLogging(bool log)
    {
        log_ = log;
    }

   protected:
    void run();
    void updateEntry(std::string addr, int offset, int rtt);
};

}  // namespace dex
