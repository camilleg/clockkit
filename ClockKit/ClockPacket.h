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

    const unsigned char
        sequenceNumber_;  // Detects out-of-order packets and thus delayed responses.

   private:
    Type type_;
    timestamp_t clientRequestTime_;  // Time on client host when packet is sent.
    timestamp_t serverReplyTime_;    // Time on server when REQUEST packet is received.
    timestamp_t clientReceiveTime_;  // Time on client when REPLY packet is received.

   public:
    explicit ClockPacket(Type t, unsigned char seqNum, timestamp_t clientRequestTime);

    // Values are read from the buffer.
    explicit ClockPacket(const char* buffer);

    // Write values to a buffer of PACKET_LENGTH bytes.
    void write(char* buffer) const;

    inline Type getType() const
    {
        return type_;
    }
    inline void setType(Type t)
    {
        type_ = t;
    }

    inline timestamp_t getClientRequestTime() const
    {
        return clientRequestTime_;
    }
    inline void setClientRequestTime(timestamp_t t)
    {
        clientRequestTime_ = t;
    }

    inline timestamp_t getServerReplyTime() const
    {
        return serverReplyTime_;
    }
    inline void setServerReplyTime(timestamp_t t)
    {
        serverReplyTime_ = t;
    }

    inline timestamp_t getClientReceiveTime() const
    {
        return clientReceiveTime_;
    }
    inline void setClientReceiveTime(timestamp_t t)
    {
        clientReceiveTime_ = t;
    }

    // Return the round trip time for the client-server correspondence.
    inline timestamp_t getRTT() const
    {
        return clientReceiveTime_ - clientRequestTime_;
    }

    // Return the estimated offset between the client and server clocks.
    timestamp_t getClockOffset() const
    {
        return serverReplyTime_ + getErrorBound() - clientReceiveTime_;
    }

    // Return the error bound on the clock offset calculation.
    inline timestamp_t getErrorBound() const
    {
        return getRTT() / 2;
    }

    // Dump this packet to STDOUT, for debugging.
    void print() const;
};

}  // namespace dex
