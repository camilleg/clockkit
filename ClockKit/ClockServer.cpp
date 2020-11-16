//----------------------------------------------------------------------------//
#ifndef DEX_CLOCK_SERVER_CPP
#define DEX_CLOCK_SERVER_CPP
//----------------------------------------------------------------------------//
#include "ClockServer.h"
#include "ClockPacket.h"
#include "Clock.h"
#include "HighResolutionClock.h"
#include <iostream>
#include <cmath>
//----------------------------------------------------------------------------//
namespace dex {
//----------------------------------------------------------------------------//


const timestamp_t ClockServer::SYSTEM_STATE_PURGE_TIME = 5000000; // 5 seconds

ClockServer::ClockServer(InetAddress addr, tpport_t port, Clock& clock)
    : addr_(addr), port_(port), clock_(clock), log_(false)
{
    lastUpdate_ = clock_.getValue();
}

void ClockServer::run()
{

    UDPSocket socket(addr_, port_);

    if (log_)
    {
        cout << "time\thost\toffset\trtt" << endl;
    }
    
    const int length = ClockPacket::PACKET_LENGTH;
    char buffer[length];
    ClockPacket packet;
    
    while (socket.isPending(Socket::pendingInput, TIMEOUT_INF))
    {
        timestamp_t serverReplyTime = clock_.getValue();
        InetAddress peer = socket.getPeer(); // also sets up the socket to send back to the sender
        int bytesReceived = socket.receive(buffer, length);
        if (bytesReceived != length)
        {
            cerr << "ERR: Received packet of wrong length." << endl;
        }
        else
        {
            packet.read(buffer);
            if (packet.getType() == ClockPacket::REQUEST)
            {
                packet.setServerReplyTime(serverReplyTime);
                packet.setType(ClockPacket::REPLY);
                packet.write(buffer);
                int bytesSent = socket.send(buffer, length);
                if (bytesSent != length)
                {
                    cerr << "ERR: Could not send entire packet." << endl;
                }
            }
            else if (packet.getType() == ClockPacket::ACKNOWLEDGE)
            {
                updateEntry(
                    peer.getHostname(),
                    packet.getClockOffset(),
                    packet.getRTT());
            }
            else
            {
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
    timestamp_t now = clock_.getValue();
    string nowStr = Timestamp::timestampToString(now);
    
    ackData_[addr].time = now;
    ackData_[addr].offset = offset;
    ackData_[addr].rtt = rtt;
    
    if (log_)
    {
        cout << nowStr << '\t'
            << addr << '\t'
            << offset << '\t'
            << rtt << endl;
            
        if ( (now - lastUpdate_) > 1000000 )
        {
            lastUpdate_ = now;
        
            map<string, Entry>::iterator it;
            
            // purge old entries
            for(it = ackData_.begin(); it != ackData_.end();)
            {
                timestamp_t entryTime = (it->second).time;
                if (now - entryTime > SYSTEM_STATE_PURGE_TIME) {
                    it = ackData_.erase(it);
                } else {
                    ++it;
                }
            }
        
            // calculate maximum offset
            int maxOffset = 0;
            for(it = ackData_.begin(); it != ackData_.end(); it++)
            {
                int offset = abs( (it->second).offset ) + ((it->second).rtt / 2);
                if (offset > maxOffset) maxOffset = offset;
            }
            
            cout << nowStr << '\t'
                 << "MAX_OFFSET"<< '\t'
                 << maxOffset << '\t'
                 << endl;        
        }
    }
}



//----------------------------------------------------------------------------//
} // namespace dex
//----------------------------------------------------------------------------//
#endif //DEX_CLOCK_SERVER_CPP

