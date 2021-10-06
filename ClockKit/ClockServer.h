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
    static constexpr auto zero = std::chrono::seconds(0);  // aka 0s

    struct Entry {
        tp time;     // "Now."
        dur offset;  // From ClockPacket::getClockOffset().
        dur rtt;     // From ClockPacket::rtt().
        Entry(tp a, dur b, dur c)
            : time(a)
            , offset(b)
            , rtt(c)
        {
        }
        Entry()
            : time(zero)
            , offset(zero)
            , rtt(zero)
        {
        }
    };
    std::map<std::string, Entry> ackData_;

    bool log_;

    // When offsetMax was most recently recalculated.
    tp tRecalculated_;

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
    void updateEntry(const std::string& addr, dur offset, dur rtt);
};

}  // namespace dex
