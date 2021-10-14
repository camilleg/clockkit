#include "ClockServer.h"

#include <cmath>
#include <iostream>

#include "ClockPacket.h"

using namespace std;
using namespace std::chrono;

namespace dex {

ClockServer::ClockServer(ost::InetAddress addr, int port, Clock& clock)
    : addr_(addr)
    , port_(port)
    , clock_(clock)
    , ackData_{map<string, Entry>()}
    , log_(false)
    , tRecalculated_(clock_.getValue())
{
}

void ClockServer::run()
{
    if (log_)
        cout << "time                     host    \toffset\tround-trip-time" << endl;
    constexpr auto length = ClockPacket::PACKET_LENGTH;
    std::byte buffer[length];
    ost::UDPSocket socket(addr_, port_);

    while (socket.isPending(ost::Socket::pendingInput, TIMEOUT_INF)) {
        const auto now = clock_.getValue();  // Before anything else.
        if (now == tpInvalid) {
            // Very unlikely, for std::chrono::system_clock::now().
            cerr << "ClockServer's clock is corrupt.\n";
            continue;
        }
        const ost::InetAddress peer = socket.getPeer();  // also sets up the socket to send back to the sender
        if (socket.receive(buffer, length) != length) {
            cerr << "ClockServer ignored packet with wrong length.\n";
            continue;
        }
        ClockPacket packet(buffer);
        switch (packet.getType()) {
            case ClockPacket::REQUEST:
                packet.setType(ClockPacket::REPLY);
                packet.setServerReplyTime(now);
                packet.write(buffer);
                if (socket.send(buffer, length) != length)
                    cerr << "ClockServer sent incomplete packet.\n";
                break;
            case ClockPacket::ACKNOWLEDGE:
                updateEntry(peer.getHostname(), packet.getClockOffset(), packet.rtt());
                break;
            case ClockPacket::KILL:
                return;
            default:
                cerr << "ClockServer ignored packet with unknown type.\n";
        }
    }
}

void ClockServer::updateEntry(const string& addr, dur offset, dur rtt)
{
    if (!log_)
        return;
    const auto now = clock_.getValue();
    if (now == tpInvalid) {
        // Very unlikely, for std::chrono::system_clock::now().
        cerr << "ClockServer's clock is corrupt.\n";
        return;
    }
    const auto nowStr = timestampToString(now);
    cout << nowStr << ' ' << addr << '\t' << UsecFromDur(offset) << '\t' << UsecFromDur(rtt) << endl;
    ackData_[addr] = Entry(now, offset, rtt);

    if (now < tRecalculated_ + 250ms)
        return;
    tRecalculated_ = now;

    // Purge entries older than 1.5 s.
    // (erase+remove_if+lambda would that fails with map; wait for C++20's erase_if.)
    const auto tPurge = now - 1500ms;
    for (auto it = ackData_.begin(); it != ackData_.end();) {
        if (it->second.time < tPurge)
            it = ackData_.erase(it);
        else
            ++it;
    }

    // Recalculate the maximum offset.
    int64_t offsetMax = 0;
    for (const auto& data : ackData_) {
        const auto& entry = data.second;
        if (entry.offset == durInvalid || entry.rtt == durInvalid) {
            offsetMax = usecInvalid;
            break;
        }
        // RHS isn't invalid.
        offsetMax = max(offsetMax, abs(UsecFromDur(entry.offset)) + UsecFromDur(entry.rtt) / 2);
    }
    cout << nowStr << ' ' << "offsetMax" << '\t' << offsetMax << '\t' << "---" << endl;
}

}  // namespace dex
