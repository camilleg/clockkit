#include "ClockServer.h"

#include <cmath>
#include <iostream>

#include "ClockPacket.h"
#include "HighResolutionClock.h"

using namespace std;

namespace dex {

const timestamp_t ClockServer::SYSTEM_STATE_PURGE_TIME = 1500000;  // usec

ClockServer::ClockServer(ost::InetAddress addr, ost::tpport_t port, Clock& clock)
    : addr_(addr)
    , port_(port)
    , clock_(clock)
    , ackData_{std::map<std::string, Entry>()}
    , log_(false)
    , die_(false)
    , lastUpdate_(clock_.getValue())
{
}

void ClockServer::run()
{
    ost::UDPSocket socket(addr_, port_);
    if (log_)
        cout << "time\thost\toffset\trtt" << endl;
    const int length = ClockPacket::PACKET_LENGTH;
    uint8_t buffer[length];

    while (socket.isPending(ost::Socket::pendingInput, TIMEOUT_INF)) {
        if (die_)
            return;
        const timestamp_t serverReplyTime = clock_.getValue();
        const ost::InetAddress peer =
            socket.getPeer();  // also sets up the socket to send back to the sender
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
                    updateEntry(peer.getHostname(), packet.getClockOffset(), packet.getRTT());
                    break;
                case ClockPacket::KILL:
                    die_ = true;  // Tell *all* threads to exit.  (Common C++ can't do this.)
                    return;       // Exit this thread.
                default:
                    cerr << "ERR: packet had wrong type.\n";
            }
        }
    }
}

void ClockServer::updateEntry(string addr, int offset, int rtt)
{
    const auto now = clock_.getValue();
    const auto nowStr = Timestamp::timestampToString(now);
    ackData_[addr].time = now;
    ackData_[addr].offset = offset;
    ackData_[addr].rtt = rtt;
    if (!log_)
        return;

    cout << nowStr << '\t' << addr << '\t' << offset << '\t' << rtt << endl;
    // 1.0 seconds sets only how often to recalculate MAX_OFFSET.
    if (now < lastUpdate_ + 1000000)
        return;
    lastUpdate_ = now;

    map<string, Entry>::iterator it;

    // Purge old entries.
    for (it = ackData_.begin(); it != ackData_.end();) {
        const timestamp_t entryTime = (it->second).time;
        if (now > entryTime + SYSTEM_STATE_PURGE_TIME) {
            it = ackData_.erase(it);
        }
        else {
            ++it;
        }
    }

    // Calculate maximum offset.
    auto maxOffset = 0;
    for (it = ackData_.begin(); it != ackData_.end(); ++it) {
        const auto offset = abs((it->second).offset) + ((it->second).rtt / 2);
        if (offset > maxOffset)
            maxOffset = offset;
    }
    cout << nowStr << '\t' << "MAX_OFFSET" << '\t' << maxOffset << '\t' << "---" << endl;
}

}  // namespace dex
