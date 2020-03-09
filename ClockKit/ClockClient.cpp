
//----------------------------------------------------------------------------//
#ifndef DEX_CLOCK_CLIENT_CPP
#define DEX_CLOCK_CLIENT_CPP
//----------------------------------------------------------------------------//
#include "ClockClient.h"
#include "Exceptions.h"
#include "HighResolutionClock.h"
//----------------------------------------------------------------------------//
namespace dex {
//----------------------------------------------------------------------------//

ClockClient::ClockClient(InetHostAddress addr, int port)
    :timeout_(1000), sequence_(0), lastRTT_(0), acknowledge_(false)
{
    // opens a udp socket on a local port, starting at 5000.
    InetAddress localhost("0.0.0.0"); // any
    int localPort = 5000;
    bool bound = false;
    while(!bound)
    {
        try
        {
            socket_ = new UDPSocket(localhost, localPort);
            bound = true;
        }
        catch (Socket*)
        {
            delete socket_;
            localPort++;
        }
    }
    
    // set the destination address
    socket_->setPeer(addr, port);
}

ClockClient::~ClockClient()
{
    delete socket_;
}

timestamp_t ClockClient::getValue()
{
    Clock& baseClock = HighResolutionClock::instance();

    // remember that: secondaryClock + phase = primaryClock
    timestamp_t phase = getPhase(baseClock, false);
    timestamp_t now = baseClock.getValue();
    return (now + phase);
}

void ClockClient::sendPacket(ClockPacket& packet)
{
    const int length = ClockPacket::PACKET_LENGTH;
    char buffer[length];
    packet.write(buffer);
    
    int bytesSent = socket_->send(buffer, length);
    if (bytesSent != length)
        throw ClockException("could not send packet");
}

    
ClockPacket ClockClient::receivePacket(Clock& clock)
{
    const int length = ClockPacket::PACKET_LENGTH;
    char buffer[length];
    int timeoutMsec = timeout_ / 1000;
    if (timeoutMsec < 1) timeoutMsec = 1;

    while (true)
    {
        bool packetArrived =
            socket_->isPending(Socket::pendingInput, timeoutMsec);    
        
        if (!packetArrived)
            throw ClockException("timeout");
            
        int bytesReceived = socket_->receive(buffer, length);
        
        if (bytesReceived != length)
            throw ClockException("received packet of wrong length");
            
        ClockPacket packet(buffer);
        packet.setClientReceiveTime(clock.getValue());

        if (packet.getSequenceNumber() != sequence_)
        {
            cout << "wrong sequence number, waiting for another packet" << endl;
        }
        else if (packet.getType() != ClockPacket::REPLY)
        {
            cout << "packet of wrong type, wating for another packet" << endl;
        }
        else if (packet.getRTT() > timeout_)
        {
            //throw ClockException("response took too long: " + packet.getRTT());
            throw ClockException("response took too long");
            
        }
        else
        {
            lastRTT_ = packet.getRTT();
            return packet;
        }
    }
}


timestamp_t ClockClient::getPhase(Clock& clock)
{
    return getPhase(clock, acknowledge_);
}

timestamp_t ClockClient::getPhase(Clock& clock, bool acknowledge)
{
    sequence_ = (sequence_ % 250) + 1;

    ClockPacket packet;
    packet.setType(ClockPacket::REQUEST);
    packet.setSequenceNumber(sequence_);
    packet.setClientRequestTime(clock.getValue());
    
    sendPacket(packet);
    
    packet = receivePacket(clock);

    if (acknowledge)
    {
        packet.setType(ClockPacket::ACKNOWLEDGE);
        sendPacket(packet);
    }

    return packet.getClockOffset();
}

int ClockClient::getTimeout()
{
    return timeout_;
}

void ClockClient::setTimeout(int timeout)
{
    timeout_ = timeout;
}

int ClockClient::getLastRTT()
{
    return lastRTT_;
}

void ClockClient::setAcknowledge(bool acknowledge)
{
    acknowledge_ = acknowledge;
}


//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_CLOCK_CLIENT_CPP
//----------------------------------------------------------------------------//

