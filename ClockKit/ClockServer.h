#pragma once
#include <cc++/socket.h>  // for ost::lotsastuff
#include <map>
#include <string>

#include "Clock.h"

namespace dex {

// A network server for a local clock.
// It stores responses from the clients on the state of their synchronization.
class ClockServer {
   private:
    const ost::InetAddress addr_;
    const int port_;
    Clock& clock_;

    struct Entry {
        timestamp_t time;    // "Now."
        timestamp_t offset;  // From ClockPacket::getClockOffset().
        timestamp_t rtt;     // From ClockPacket::rtt().
    };
    std::map<std::string, Entry> ackData_;

    bool log_;

    // The most recent time that offsetMax was recalculated.
    timestamp_t tRecalculated_;

   public:
    // Provide requests for timestamps to clients via a UDP port.
    // To accept connections from clients on *any* local address,
    // set InetAddress to 0.0.0.0.
    ClockServer(ost::InetAddress addr, int port, Clock& clock);

    void setLogging(bool log)
    {
        log_ = log;
    }

    void run();

   protected:
    void updateEntry(const std::string& addr, timestamp_t offset, timestamp_t rtt);
};

}  // namespace dex
