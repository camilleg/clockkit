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
    // Using more than a byte would be a breaking change for the packet format.
    using seqnum = uint8_t;
    const seqnum sequenceNumber_;

   private:
    Type type_;
    tp clientRequestTime_;  // Time on client when it sent a REQUEST packet.
    tp serverReplyTime_;    // Time on server when it got that REQUEST packet.
    tp clientReceiveTime_;  // Time on client when it got the corresponding REPLY packet.

   public:
    explicit ClockPacket(Type t, seqnum seqNum = 0, tp clientRequestTime = tpInvalid);

    // Unpack buffer into member variables.
    explicit ClockPacket(const std::byte* buffer);

    // Invalid.
    explicit ClockPacket();

    // Write member variables to a buffer of PACKET_LENGTH bytes.
    void write(std::byte* buffer) const;

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
    inline tp getClientRequestTime() const
    {
        return clientRequestTime_;
    }
    inline void setClientRequestTime(tp t)
    {
        clientRequestTime_ = t;
    }

    inline tp getServerReplyTime() const
    {
        return serverReplyTime_;
    }
    inline tp getClientReceiveTime() const
    {
        return clientReceiveTime_;
    }
#endif

    inline void setServerReplyTime(tp t)
    {
        serverReplyTime_ = t;
    }
    inline void setClientReceiveTime(tp t)
    {
        clientReceiveTime_ = t;
    }

    // Round trip time for the client-server correspondence.
    // Todo: complain if this is negative, which it should never be,
    // but is technically possible.
    inline dur rtt() const
    {
        return diff(clientReceiveTime_, clientRequestTime_);
    }

    // Estimated offset between the client and server clocks.
    dur getClockOffset() const
    {
        const auto bound = getErrorBound();
        return serverReplyTime_ == tpInvalid || clientReceiveTime_ == tpInvalid || bound == durInvalid
                   ? durInvalid
                   : serverReplyTime_ - clientReceiveTime_ + bound;
    }

    // Error bound on the calculation of clock offset.
    inline dur getErrorBound() const
    {
        const auto t = rtt();
        return t == durInvalid ? durInvalid : t / 2;
    }

    // Dump this packet to STDOUT, for debugging.
    void print() const;
};

}  // namespace dex
