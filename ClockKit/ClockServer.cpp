#include "ClockServer.h"

#include <cmath>
#include <iostream>

#include "ClockPacket.h"

using namespace std;

namespace dex {

ClockServer::ClockServer(ost::InetAddress addr, int port, Clock& clock)
    : addr_(addr)
    , port_(port)
    , clock_(clock)
    , ackData_{std::map<std::string, Entry>()}
    , log_(false)
    , tRecalculated_(clock_.getValue())
{
}

void ClockServer::run()
{
    ost::UDPSocket socket(addr_, port_);
    if (log_)
        cout << "time                     host    \toffset\tround-trip-time" << endl;
    constexpr auto length = ClockPacket::PACKET_LENGTH;
    uint8_t buffer[length];

    while (socket.isPending(ost::Socket::pendingInput, TIMEOUT_INF)) {
        const timestamp_t serverReplyTime = clock_.getValue();
        const ost::InetAddress peer = socket.getPeer();  // also sets up the socket to send back to the sender
        if (socket.receive(buffer, length) != length) {
            cerr << "ERR: packet had wrong length.\n";
        }
        else {
            ClockPacket packet(buffer);
            switch (packet.getType()) {
                case ClockPacket::REQUEST:
                    packet.setServerReplyTime(serverReplyTime);
                    packet.setType(ClockPacket::REPLY);
                    packet.write(buffer);
                    if (socket.send(buffer, length) != length)
                        cerr << "ERR: sent incomplete packet.\n";
                    break;
                case ClockPacket::ACKNOWLEDGE:
                    updateEntry(peer.getHostname(), packet.getClockOffset(), packet.rtt());
                    break;
                case ClockPacket::KILL:
                    return;
                default:
                    cerr << "ERR: packet had wrong type.\n";
            }
        }
    }
}

void ClockServer::updateEntry(const string& addr, timestamp_t offset, timestamp_t rtt)
{
    if (!log_)
        return;
    const auto now = clock_.getValue();
    const auto nowStr = timestampToString(now);
    cout << nowStr << ' ' << addr << '\t' << offset << '\t' << rtt << endl;
    ackData_[addr] = {now, offset, rtt};
    // 1.0 seconds sets only how often to recalculate offsetMax.
    if (now < tRecalculated_ + 1000000)
        return;
    tRecalculated_ = now;

    // Purge old entries.
    // Don't use erase+remove_if+lambda, because that fails with map;
    // wait for C++20's std::erase_if.
    const timestamp_t SYSTEM_STATE_PURGE_TIME = 1500000;  // usec
    const auto tPurge = now - SYSTEM_STATE_PURGE_TIME;
    for (auto it = ackData_.begin(); it != ackData_.end();) {
        if (it->second.time < tPurge) {
            it = ackData_.erase(it);
        }
        else {
            ++it;
        }
    }

    // Calculate the maximum offset.
    timestamp_t offsetMax = 0L;
    for (const auto& data : ackData_) {
        const auto& entry = data.second;
        offsetMax = max(offsetMax, abs(entry.offset) + entry.rtt / 2);
    }
    cout << nowStr << ' ' << "offsetMax" << '\t' << offsetMax << '\t' << "---" << endl;
}

}  // namespace dex
