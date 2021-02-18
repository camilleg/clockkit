#include "ClockClient.h"

#include "Exceptions.h"
#include "HighResolutionClock.h"

using namespace std;

namespace dex {

ClockClient::ClockClient(InetHostAddress addr, int port)
    : timeout_(1000)
    , lastRTT_(0)
    , sequence_(0)
    , acknowledge_(false)

    // On Linux, 0 picks the next free port.  If eventually another
    // OS doesn't, then pass localPort in as an arg to this constructor,
    // from a localPort:5678 line in the config file.
    , socket_{new UDPSocket(InetAddress("0.0.0.0"), 0)}
{
    // Set the destination address.
    socket_->setPeer(addr, port);
}

timestamp_t ClockClient::getValue()
{
    Clock& baseClock = HighResolutionClock::instance();
    return baseClock.getValue() + getPhase(baseClock, false);
    // primary clock = secondary clock + phase
}

void ClockClient::sendPacket(const ClockPacket& packet)
{
    constexpr auto length = ClockPacket::PACKET_LENGTH;
    uint8_t buffer[length];
    packet.write(buffer);
    if (socket_->send(buffer, length) != length)
        throw ClockException("could not send packet");
}

ClockPacket ClockClient::receivePacket(Clock& clock)
{
    constexpr auto length = ClockPacket::PACKET_LENGTH;
    uint8_t buffer[length];
    const auto timeoutMsec = std::max(1, timeout_ / 1000);

    while (true) {
        if (!socket_->isPending(Socket::pendingInput, timeoutMsec))
            throw ClockException("timeout");

        if (socket_->receive(buffer, length) != length)
            throw ClockException("packet had wrong length");

        ClockPacket packet(buffer);
        packet.setClientReceiveTime(clock.getValue());
        if (packet.sequenceNumber_ != sequence_) {
            cout << "ignoring out-of-order packet\n";
        }
        else if (packet.getType() != ClockPacket::REPLY) {
            cout << "ignoring packet with wrong type\n";
        }
        else if (packet.getRTT() > timeout_) {
            throw ClockException("response timed out");
        }
        else {
            lastRTT_ = packet.getRTT();
            return packet;
        }
    }
}

// We can't use a default value "bool acknowledge = acknowledge_"
// because the base class's signature for getPhase has only the first arg.
timestamp_t ClockClient::getPhase(Clock& clock, bool acknowledge)
{
    ++sequence_ %= 250;  // One byte.
    const ClockPacket p1(ClockPacket::REQUEST, sequence_, clock.getValue());
    sendPacket(p1);
    ClockPacket p2 = receivePacket(clock);
    if (acknowledge) {
        p2.setType(ClockPacket::ACKNOWLEDGE);
        sendPacket(p2);
    }
    return p2.getClockOffset();
}

}  // namespace dex
