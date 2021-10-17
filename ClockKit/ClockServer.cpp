#include "ClockServer.h"

#include <cmath>
#include <iostream>

#include "ClockPacket.h"

using namespace std;
using namespace std::chrono;

namespace dex {

ClockServer::ClockServer(kissnet::endpoint addr_port, Clock& clock)
    : addr_port_(addr_port)
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
    kissnet::buffer<length> buffer;
    kissnet::udp_socket socket(addr_port_);
    socket.bind();

    for (;;) {
        switch (socket.select(0x1 /*fds_read*/, std::numeric_limits<int64_t>::max()).value) {
            case kissnet::socket_status::errored:
                cerr << "error waiting for packet\n";
                continue;
            case kissnet::socket_status::valid:;  // cerr << "We have something to recv.\n";
#if 0
	case kissnet::socket_status::timed_out:
	    cerr << "timed out waiting for packet for 293,000 years\n";
	    continue;
#endif
            default:
                break;
        }

        const auto now = clock_.getValue();  // Before anything else.
        if (now == tpInvalid) {
            // Very unlikely, for std::chrono::system_clock::now().
            cerr << "ClockServer's clock is corrupt.\n";
            continue;
        }
        kissnet::addr_collection peer;
        const auto [num_bytes, status] = socket.recv(buffer, 0, &peer);
        const auto peer2 = socket.get_recv_endpoint();  // Same as peer.
        if (status != 0x1 /*valid*/) {
            cerr << "ClockServer had problems receiving a packet; status " << status << "\n";
            continue;
        }
        if (num_bytes != length) {
            cerr << "ClockServer ignored packet with wrong length " << num_bytes << ".\n";
            continue;
        }
        ClockPacket packet(buffer);
        // cout << "got "; packet.print();
        switch (packet.getType()) {
            case ClockPacket::REQUEST:
                packet.setType(ClockPacket::REPLY);
                packet.setServerReplyTime(now);
                // cout << "srv replying: "; packet.print();
                packet.write(buffer.data());
                {
                    const auto [num_bytes, status] = socket.send(buffer, length, &peer);
                    if (status != kissnet::socket_status::valid)
                        cerr << "ClockServer had problems sending a packet; status " << status << "\n";
                    if (num_bytes != length)
                        cerr << "ClockServer sent incomplete packet.\n";
                    // cout << "sent to " << peer2.address << ":" << peer2.port << "\n";
                    break;
                }
            case ClockPacket::ACKNOWLEDGE:
                // CC++ printed localhost as "localhost", unlike kissnet's "127.0.0.1."
                // But no test cases used that.
                updateEntry(peer2.address, packet.getClockOffset(), packet.rtt(), now);
                break;
            case ClockPacket::KILL:
                return;
            default:
                cerr << "ClockServer ignored packet with unexpected type " << packet.getTypeName() << ".\n";
        }
    }
}

void ClockServer::updateEntry(const string& addr, dur offset, dur rtt, tp now)
{
    if (!log_)
        return;
    const auto nowStr = timestampToString(now);
    cout << nowStr << ' ' << addr << '\t' << UsecFromDur(offset) << '\t' << UsecFromDur(rtt) << endl;
    ackData_[addr] = Entry(now, offset, rtt);

    if (now < tRecalculated_ + 500ms)
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
