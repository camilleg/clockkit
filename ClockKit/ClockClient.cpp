#include "ClockClient.h"

#include "HighResolutionClock.h"

// #define DEBUG

using namespace std;

namespace dex {

ClockClient::ClockClient(ost::InetHostAddress addr, int port)
    : timeout_(1000)
    , rtt_(0u)
    , sequence_(0)
    , acknowledge_(false)

    // On Linux, 0 picks the next free port.  If eventually another
    // OS doesn't, then pass localPort in as an arg to this constructor,
    // from a localPort:5678 line in the config file.
    , socket_{new ost::UDPSocket(ost::InetAddress("0.0.0.0"), 0)}
{
    // Set the destination address.
    socket_->setPeer(addr, port);
}

timestamp_t ClockClient::getValue()
{
    Clock& baseClock = HighResolutionClock::instance();
    const auto phase = getPhase(baseClock, false);
    if (phase == invalid)
        return invalid;
    return baseClock.getValue() + phase;
    // primary clock = secondary clock + phase
}

bool ClockClient::sendPacket(const ClockPacket& packet) const
{
    constexpr auto length = ClockPacket::PACKET_LENGTH;
    uint8_t buffer[length];
    packet.write(buffer);
    return socket_->send(buffer, length) == length;
}

ClockPacket ClockClient::receivePacket(Clock& clock)
{
    constexpr auto length = ClockPacket::PACKET_LENGTH;
    uint8_t buffer[length];
    const auto timeoutMsec = std::max(1, timeout_ / 1000);

    while (true) {
        if (!socket_->isPending(ost::Socket::pendingInput, timeoutMsec)) {
#ifdef DEBUG
            cerr << "timed out waiting for packet after " << timeoutMsec << " ms\n";
#endif
            return ClockPacket();  // Timeout.
        }

        if (socket_->receive(buffer, length) != length) {
            cerr << "ignoring wrong-length packet\n";
            return ClockPacket();  // Packet had wrong length.
        }

#ifdef DEBUG
        cerr << "got packet\n";
#endif
        ClockPacket packet(buffer);
        if (packet.getType() == ClockPacket::KILL) {
            exit(0);
        }

        packet.setClientReceiveTime(clock.getValue());
        if (packet.sequenceNumber_ != sequence_) {
            cerr << "ignoring out-of-order packet " << int(packet.sequenceNumber_) << "; expected "
                 << int(sequence_) << "\n";
            continue;
        }

        if (packet.getType() != ClockPacket::REPLY) {
            cerr << "ignoring packet with wrong type\n";
            continue;
        }

        const auto rttPrev = packet.rtt();
        if (rttPrev > timeout_)
            return ClockPacket();  // Timeout.
        rtt_ = rttPrev;
        return packet;
    }
}

// We can't use a default value "bool acknowledge = acknowledge_"
// because the base class's signature for getPhase has only the first arg.
// Always false, while it's called from only ClockClient::getValue().
timestamp_t ClockClient::getPhase(Clock& clock, bool acknowledge)
{
    ++sequence_ %= 250;  // One byte.
    const ClockPacket p1(ClockPacket::REQUEST, sequence_, clock.getValue());
    if (!sendPacket(p1))
        return invalid;
    ClockPacket p2 = receivePacket(clock);
    if (p2.invalid())
        return invalid;
    if (acknowledge) {
        p2.setType(ClockPacket::ACKNOWLEDGE);
        if (!sendPacket(p2))
            return invalid;
    }
    return p2.getClockOffset();
}

}  // namespace dex
