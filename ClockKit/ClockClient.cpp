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

bool ClockClient::sendPacket(const ClockPacket& packet) const
{
#ifdef DEBUG
    cerr << "\nsending " << packet.getTypeName() << "\n";
#endif
    constexpr auto length = ClockPacket::PACKET_LENGTH;
    uint8_t buffer[length];
    packet.write(buffer);
    return socket_->send(buffer, length) == length;
}

ClockPacket ClockClient::receivePacket(Clock& clock)
{
#ifdef DEBUG
    cerr << "expecting...\n";
#endif
    constexpr auto length = ClockPacket::PACKET_LENGTH;
    uint8_t buffer[length];
    const auto timeoutMsec = std::max(1, timeout_ / 1000);

    while (true) {
        if (!socket_->isPending(ost::Socket::pendingInput, timeoutMsec)) {
#ifdef DEBUG
            cerr << "timed out waiting for packet after " << timeoutMsec << " ms\n";
#endif
            return ClockPacket();
        }
        if (socket_->receive(buffer, length) != length) {
#ifdef DEBUG
            cerr << "ignoring wrong-length packet\n";
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
        if (packet.sequenceNumber_ != sequence_) {
#ifdef DEBUG
            cerr << "ignoring out-of-order packet " << int(packet.sequenceNumber_) << "; expected " << int(sequence_)
                 << "\n";
#endif
            continue;
        }
        if (packet.getType() != ClockPacket::REPLY) {
#ifdef DEBUG
            cerr << "ignoring non-reply packet\n";
#endif
            continue;
        }

        packet.setClientReceiveTime(now);
        const auto rtt = packet.rtt();
        if (rtt > timeout_) {
#ifdef DEBUG
            cerr << "ignoring reply that arrived more than " << timeout_ << " usec later\n";
#endif
            return ClockPacket();
        }
        rtt_ = rtt;
        return packet;
    }
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

// We can't use a default value "bool acknowledge = acknowledge_"
// because the base class's signature for getPhase has only the first arg.
// Always false, while it's called from only ClockClient::getValue().
timestamp_t ClockClient::getPhase(Clock& clock, bool acknowledge)
{
    ++sequence_ %= 250;  // One byte.
    if (!sendPacket(ClockPacket(ClockPacket::REQUEST, sequence_, clock.getValue())))
        return invalid;
    ClockPacket packet(receivePacket(clock));
    if (packet.invalid())
        return invalid;
    if (acknowledge) {
        packet.setType(ClockPacket::ACKNOWLEDGE);
        if (!sendPacket(packet))
            return invalid;
    }
    return packet.getClockOffset();
}

}  // namespace dex
