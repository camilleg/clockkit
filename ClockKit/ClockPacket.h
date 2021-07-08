#pragma once
#include "Timestamp.h"

namespace dex {

// UDP packet used with ClockServer and ClockClient.
class ClockPacket {
   public:
    // - INVALID: uninitialized.
    // - REQUEST: Client sends packet to server requesting current time.
    // - REPLY: Server sends client the current time, after a REQUEST.
    // - ACKNOWLEDGE: Client sends server the status of its synchronization, from getPhase().
    // - KILL: terminate client and server.
    enum Type { INVALID = 0, REQUEST, REPLY, ACKNOWLEDGE, KILL };

    // Each packet has:
    // - type : 1 byte
    // - sequence : 1 bytes
    // - 3 timestamps : 8 bytes each
    // - total : 26 bytes
    enum { PACKET_LENGTH = 26 };

    // Detects out-of-order packets and thus delayed responses.
    // (A byte isn't too small, because even at 10 packets per second,
    // this detects packets as much as 25 seconds late,
    // which is much larger than the typical tolerance of a fraction of a second.)
    const uint8_t sequenceNumber_;

   private:
    Type type_;
    timestamp_t clientRequestTime_;  // Time on client when it sent a REQUEST packet.
    timestamp_t serverReplyTime_;    // Time on server when it got that REQUEST packet.
    timestamp_t clientReceiveTime_;  // Time on client when it got the corresponding REPLY packet.

   public:
    explicit ClockPacket(Type t, uint8_t seqNum, timestamp_t clientRequestTime);

    // Unpack buffer into member variables.
    explicit ClockPacket(uint8_t* buffer);

    // Invalid.
    explicit ClockPacket();

    // Write member variables to a buffer of PACKET_LENGTH bytes.
    void write(uint8_t* buffer) const;

    inline Type getType() const
    {
        return type_;
    }
    inline void setType(Type t)
    {
        type_ = t;
    }
    const char* getTypeName() const;  // Prettyprint type_.

    inline bool invalid() const
    {
        return type_ == INVALID;
    }

#ifdef UNUSED
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
    inline timestamp_t getClientReceiveTime() const
    {
        return clientReceiveTime_;
    }
#endif

    inline void setServerReplyTime(timestamp_t t)
    {
        serverReplyTime_ = t;
    }
    inline void setClientReceiveTime(timestamp_t t)
    {
        clientReceiveTime_ = t;
    }

    // Round trip time for the client-server correspondence.
    // Todo: complain if this is negative, which it should never be,
    // but is technically possible.
    inline timestamp_t rtt() const
    {
        return clientReceiveTime_ - clientRequestTime_;
    }

    // Estimated offset between the client and server clocks.
    timestamp_t getClockOffset() const
    {
        return serverReplyTime_ + getErrorBound() - clientReceiveTime_;
    }

    // Error bound on the calculation of clock offset.
    inline timestamp_t getErrorBound() const
    {
        return rtt() / 2;
    }

    // Dump this packet to STDOUT, for debugging.
    void print() const;
};

}  // namespace dex
