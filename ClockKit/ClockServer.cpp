#include "ClockServer.h"
#include <cmath>
#include <iostream>
#include "ClockPacket.h"
#include "HighResolutionClock.h"

using namespace std;

namespace dex {

const timestamp_t ClockServer::SYSTEM_STATE_PURGE_TIME = 5000000;  // 5 seconds

ClockServer::ClockServer(InetAddress addr, tpport_t port, Clock& clock)
    : addr_(addr)
    , port_(port)
    , clock_(clock)
    , log_(false)
    , lastUpdate_(clock_.getValue())
{
}

void ClockServer::run()
{
    UDPSocket socket(addr_, port_);
    if (log_)
        cout << "time\thost\toffset\trtt" << endl;
    const int length = ClockPacket::PACKET_LENGTH;
    char buffer[length];

    while (socket.isPending(Socket::pendingInput, TIMEOUT_INF)) {
        const timestamp_t serverReplyTime = clock_.getValue();
        const InetAddress peer =
            socket.getPeer();  // also sets up the socket to send back to the sender
        if (socket.receive(buffer, length) != length) {
            cerr << "ERR: Received packet of wrong length." << endl;
        }
        else {
            ClockPacket packet(buffer);
            if (packet.getType() == ClockPacket::REQUEST) {
                packet.setServerReplyTime(serverReplyTime);
                packet.setType(ClockPacket::REPLY);
                packet.write(buffer);
                if (socket.send(buffer, length) != length) {
                    cerr << "ERR: Could not send entire packet." << endl;
                }
            }
            else if (packet.getType() == ClockPacket::ACKNOWLEDGE) {
                updateEntry(peer.getHostname(), packet.getClockOffset(), packet.getRTT());
            }
            else {
                cerr << "ERR: received packet of invalid type" << endl;
            }
        }
    }
}

void ClockServer::setLogging(bool log)
{
    log_ = log;
}

void ClockServer::updateEntry(string addr, int offset, int rtt)
{
    const timestamp_t now = clock_.getValue();
    const string nowStr = Timestamp::timestampToString(now);
    ackData_[addr].time = now;
    ackData_[addr].offset = offset;
    ackData_[addr].rtt = rtt;
    if (!log_)
        return;

    cout << nowStr << '\t' << addr << '\t' << offset << '\t' << rtt << endl;
    if ((now - lastUpdate_) > 1000000) {
        lastUpdate_ = now;
        map<string, Entry>::iterator it;

        // Purge old entries.
        for (it = ackData_.begin(); it != ackData_.end();) {
            const timestamp_t entryTime = (it->second).time;
            if (now - entryTime > SYSTEM_STATE_PURGE_TIME) {
                it = ackData_.erase(it);
            }
            else {
                ++it;
            }
        }

        // Calculate maximum offset.
        int maxOffset = 0;
        for (it = ackData_.begin(); it != ackData_.end(); ++it) {
            const int offset = abs((it->second).offset) + ((it->second).rtt / 2);
            if (offset > maxOffset)
                maxOffset = offset;
        }
        cout << nowStr << '\t' << "MAX_OFFSET" << '\t' << maxOffset << '\t' << endl;
    }
}

}  // namespace dex
