#include "ClockClient.h"
#include "SystemClock.h"

// #define DEBUG

using namespace std;
using namespace std::chrono;

namespace dex {

ClockClient::ClockClient(const kissnet::endpoint& addr_port)
    : timeout_(1000)
    , rtt_(0u)
    , sequence_(0)
    , acknowledge_(false)
    , socket_(addr_port)
{
    kissnet_init();
}

bool ClockClient::sendPacket(const ClockPacket& packet)
{
    ClockPacket::packetbuf buffer;
    packet.write(buffer);
#ifdef DEBUG
    cerr << "\nsending " << packet.getTypeName() << "\n";
#endif
    const auto [num_bytes, status] = socket_.send(buffer);
    if (status != kissnet::socket_status::valid) {
        cerr << "ClockClient failed to send a packet: status " << status << "\n";
        return false;
    }
    if (num_bytes != ClockPacket::PACKET_LENGTH) {
        cerr << "ClockClient sent an incomplete packet.\n";
        return false;
    }
#ifdef DEBUG
    cerr << "sent\n";
    packet.print();
#endif
    return true;
}

ClockPacket ClockClient::receivePacket(Clock& clock)
{
    ClockPacket::packetbuf buffer;
    // getTimeout() isn't invalid.
    const auto timeoutMsec = std::max(1, getTimeout() / 1000);
    while (true) {
        switch (socket_.select(kissnet::fds_read, timeoutMsec).value) {
            case kissnet::socket_status::errored:
                cerr << "ClockClient errored while waiting for a packet.\n";
                return ClockPacket();
            case kissnet::socket_status::timed_out:
#ifdef DEBUG
                cerr << "timed out waiting for a packet after " << timeoutMsec << " ms\n";
#endif
                return ClockPacket();
            default:
                break;
        }
        const auto [num_bytes, status] = socket_.recv(buffer);
        if (status != kissnet::socket_status::valid) {
            cerr << "ClockClient got no packet: status " << status << "\n";
            return ClockPacket();
        }
        if (num_bytes != ClockPacket::PACKET_LENGTH) {
#ifdef DEBUG
            cerr << "ignored wrong-length packet\n";
#endif
            return ClockPacket();
        }
        const auto now = clock.getValue();  // Before anything else.

        ClockPacket packet(buffer);
#ifdef DEBUG
        cerr << "got " << packet.getTypeName() << "\n";
#endif
        if (packet.getType() == ClockPacket::KILL) {
            exit(0);  // todo: kill just the ClockClient, not the entire process?  That's too harsh.
        }
        if (packet.getSeqnum() != sequence_) {
#ifdef DEBUG
            cerr << "ignored out-of-order packet " << packet.getSeqnum() << "; expected " << sequence_ << "\n";
#endif
            continue;
        }
        if (packet.getType() != ClockPacket::REPLY) {
#ifdef DEBUG
            cerr << "ignored non-reply packet of type " << packet.getTypeName() << "\n";
#endif
            continue;
        }

        packet.setClientReceiveTime(now);
        const auto rtt = packet.rtt();
        // timeout_ isn't invalid.
        if (rtt == durInvalid || rtt > timeout_) {
#ifdef DEBUG
            cerr << "ignoring reply that arrived more than " << UsecFromDur(timeout_) << " μs later\n";
#endif
            return ClockPacket();
        }
        rtt_ = rtt;
        return packet;
    }
}

tp ClockClient::getValue()
{
    Clock& baseClock = SystemClock::instance();
    const auto phase = getPhase(baseClock, false);
    if (phase == durInvalid)
        return tpInvalid;
    return baseClock.getValue() + phase;
    // primary clock = secondary clock + phase
}

// We can't use a default value "bool acknowledge = acknowledge_"
// because the base class's signature for getPhase has only the first arg.
// Always false, while it's called from only ClockClient::getValue().
dur ClockClient::getPhase(Clock& clock, bool acknowledge)
{
    ++sequence_ %= 250;  // One byte.
    if (!sendPacket(ClockPacket(ClockPacket::REQUEST, sequence_, clock.getValue())))
        return durInvalid;
    ClockPacket packet(receivePacket(clock));
    if (packet.invalid())
        return durInvalid;
    if (acknowledge) {
        packet.setType(ClockPacket::ACKNOWLEDGE);
        if (!sendPacket(packet))
            return durInvalid;
    }
    return packet.getClockOffset();
}

}  // namespace dex
