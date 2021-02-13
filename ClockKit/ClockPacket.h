#pragma once
#include "Common.h"

namespace dex {

// UDP packet used with ClockServer and ClockClient.
class ClockPacket {
   public:
    // - INVALID: uninitialized.
    // - REQUEST: Client sends packet to server requesting current time.
    // - REPLY: Server sends client the current time.
    // - ACKNOWLEDGE: Client sends server the status of its synchronization.
    enum Type { INVALID = 0, REQUEST, REPLY, ACKNOWLEDGE };

    // Each packet has:
    // - type : 1 byte
    // - sequence : 1 bytes
    // - 3 timestamps : 8 bytes each
    // - total : 26 bytes
    enum { PACKET_LENGTH = 26 };

   private:
    Type type_;
    unsigned char sequenceNumber_;   // Detects out-of-order packets and thus delayed responses.
    timestamp_t clientRequestTime_;  // Time on client host when packet is sent.
    timestamp_t serverReplyTime_;    // Time on server when REQUEST packet is received.
    timestamp_t clientReceiveTime_;  // Time on client when REPLY packet is received.

   public:
    // Empty and INVALID.
    ClockPacket();

    // Values are read from the buffer.
    ClockPacket(char* buffer)
    {
        read(buffer);
    }

    // Read/write values to/from a buffer of PACKET_LENGTH bytes.
    void read(char* buffer);
    void write(char* buffer) const;

    Type getType() const
    {
        return type_;
    }
    void setType(Type t)
    {
        type_ = t;
    }

    unsigned char getSequenceNumber() const
    {
        return sequenceNumber_;
    }
    void setSequenceNumber(unsigned char n)
    {
        sequenceNumber_ = n;
    }

    timestamp_t getClientRequestTime() const
    {
        return clientRequestTime_;
    }
    void setClientRequestTime(timestamp_t t)
    {
        clientRequestTime_ = t;
    }

    timestamp_t getServerReplyTime() const
    {
        return serverReplyTime_;
    }
    void setServerReplyTime(timestamp_t t)
    {
        serverReplyTime_ = t;
    }

    timestamp_t getClientReceiveTime() const
    {
        return clientReceiveTime_;
    }
    void setClientReceiveTime(timestamp_t t)
    {
        clientReceiveTime_ = t;
    }

    // Return the round trip time for the client-server correspondence.
    timestamp_t getRTT() const;

    // Return the estimated offset between the client and server clocks.
    timestamp_t getClockOffset() const;

    // Return the error bound on the clock offset calculation.
    timestamp_t getErrorBound() const;

    // Dump this packet to STDOUT, for debugging.
    void print() const;
};

}  // namespace dex
