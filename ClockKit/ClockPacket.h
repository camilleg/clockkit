#pragma once
#include "Timestamp.h"

namespace dex {

// UDP packet used with ClockServer and ClockClient.
class ClockPacket {
   public:
    // - INVALID: uninitialized.
    // - REQUEST: Client sends packet to server requesting current time.
    // - REPLY: Server sends client the current time.
    // - ACKNOWLEDGE: Client sends server the status of its synchronization.
    // - KILL: terminate client and server.
    enum Type { INVALID = 0, REQUEST, REPLY, ACKNOWLEDGE, KILL };

    // Each packet has:
    // - type : 1 byte
    // - sequence : 1 bytes
    // - 3 timestamps : 8 bytes each
    // - total : 26 bytes
    enum { PACKET_LENGTH = 26 };

    // XXX is a single byte not a bit less for a sequence number?
    const uint8_t sequenceNumber_;  // Detects out-of-order packets and thus delayed responses.

   private:
    Type type_;
    timestamp_t clientRequestTime_;  // Time on client when packet was sent.
    timestamp_t serverReplyTime_;    // Time on server when REQUEST packet was received.
    timestamp_t clientReceiveTime_;  // Time on client when REPLY packet was received.

   public:
    explicit ClockPacket(Type t, uint8_t seqNum, timestamp_t clientRequestTime);

    // Values are read from the buffer.
    explicit ClockPacket(uint8_t* buffer);

    // Invalid.
    explicit ClockPacket();

    // Write values to a buffer of PACKET_LENGTH bytes.
    void write(uint8_t* buffer) const;

    inline Type getType() const
    {
        return type_;
    }
    inline void setType(Type t)
    {
        type_ = t;
    }

    inline bool invalid() const
    {
        return type_ == INVALID;
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
    // todo: complain if this is negative, which it should never be,
    // but it's technically possible.
    inline timestamp_t rtt() const
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
        return rtt() / 2;
    }

    // Dump this packet to STDOUT, for debugging.
    void print() const;
};

}  // namespace dex
