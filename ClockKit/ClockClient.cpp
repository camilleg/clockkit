#include "ClockClient.h"
#include "Exceptions.h"
#include "HighResolutionClock.h"

using namespace std;

namespace dex {

ClockClient::ClockClient(InetHostAddress addr, int port)
    : timeout_(1000)
    , sequence_(0)
    , lastRTT_(0)
    , acknowledge_(false)
{
    // Open a UDP socket on the first open local port beyond 5000.
    const InetAddress localhost("0.0.0.0");  // any
    int localPort = 5000;
    while (true) {
        try {
            socket_ = new UDPSocket(localhost, localPort);
            break;
        }
        catch (Socket*) {
            delete socket_;
            ++localPort;
        }
    }
    // Set the destination address.
    socket_->setPeer(addr, port);
}

timestamp_t ClockClient::getValue()
{
    Clock& baseClock = HighResolutionClock::instance();
    return baseClock.getValue() + getPhase(baseClock, false);
    // primary clock = secondary clock + phase
}

void ClockClient::sendPacket(ClockPacket& packet)
{
    const int length = ClockPacket::PACKET_LENGTH;
    char buffer[length];
    packet.write(buffer);
    if (socket_->send(buffer, length) != length)
        throw ClockException("could not send packet");
}

ClockPacket ClockClient::receivePacket(Clock& clock)
{
    const int length = ClockPacket::PACKET_LENGTH;
    char buffer[length];
    const int timeoutMsec = std::max(1, timeout_ / 1000);

    while (true) {
        const bool packetArrived = socket_->isPending(Socket::pendingInput, timeoutMsec);
        if (!packetArrived)
            throw ClockException("timeout");

        if (socket_->receive(buffer, length) != length)
            throw ClockException("packet had wrong length");

        ClockPacket packet(buffer);
        packet.setClientReceiveTime(clock.getValue());
        if (packet.getSequenceNumber() != sequence_) {
            cout << "packet out of order, so waiting for another" << endl;
        }
        else if (packet.getType() != ClockPacket::REPLY) {
            cout << "packet had wrong type, so waiting for another" << endl;
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

timestamp_t ClockClient::getPhase(Clock& clock)
{
    return getPhase(clock, acknowledge_);
}

// We can't use a default value "bool acknowledge = acknowledge_"
// because the base class's signature for getPhase has only the first arg.
timestamp_t ClockClient::getPhase(Clock& clock, bool acknowledge)
{
    sequence_ = sequence_ % 250 + 1;  // One byte.
    ClockPacket packet;
    packet.setType(ClockPacket::REQUEST);
    packet.setSequenceNumber(sequence_);
    packet.setClientRequestTime(clock.getValue());
    sendPacket(packet);
    packet = receivePacket(clock);
    if (acknowledge) {
        packet.setType(ClockPacket::ACKNOWLEDGE);
        sendPacket(packet);
    }
    return packet.getClockOffset();
}

}  // namespace dex
