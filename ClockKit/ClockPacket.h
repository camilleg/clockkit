#pragma once
#include "Timestamp.h"

#define KISSNET_NO_EXCEP
#include "kissnet.hpp"

namespace dex {

// Detects out-of-order packets and thus delayed responses.
// (A byte isn't too small, because even at 10 packets per second,
// this detects packets as much as 25 seconds late,
// much more than the typical tolerance of a fraction of a second.
// Using more than a byte would be a breaking change for the packet format.)
using seqnum = uint8_t;

// UDP packet for ClockServer and ClockClient.
class ClockPacket {
   public:
    // INVALID: uninitialized.
    // REQUEST: Client sends packet to server requesting current time.
    // REPLY: Server sends client the current time, after a REQUEST.
    // ACKNOWLEDGE: Client sends server the status of its synchronization, from getPhase().
    // KILL: terminate client and server.
    enum Type { INVALID = 0, REQUEST, REPLY, ACKNOWLEDGE, KILL };

   private:
    Type type_;
    const seqnum sequenceNumber_;
    tp clientRequestTime_;  // Time on client when it sent a REQUEST packet.
    tp serverReplyTime_;    // Time on server when it got that REQUEST packet.
    tp clientReceiveTime_;  // Time on client when it got the corresponding REPLY packet.

   public:
    // Each packet has:
    // - type, 1 byte
    // - sequence, 1 byte
    // - 3 timestamps, 3 * 8 bytes
    // - total, 26 bytes
    constexpr static auto PACKET_LENGTH = 26;
    using packetbuf = kissnet::buffer<PACKET_LENGTH>;

    explicit ClockPacket(Type t, seqnum seqNum = 0, tp clientRequestTime = tpInvalid);

    // Unpack buffer into members.
    explicit ClockPacket(const packetbuf&);

    // Initialized as invalid.
    explicit ClockPacket();

    // Write member variables to bytes.
    void write(packetbuf&) const;

    seqnum getSeqnum() const
    {
        return sequenceNumber_;
    }

    Type getType() const
    {
        return type_;
    }
    void setType(Type t)
    {
        type_ = t;
    }
    const char* getTypeName() const;  // Prettyprint type_.

    bool invalid() const
    {
        return type_ == INVALID;
    }

#ifdef UNUSED
    tp getClientRequestTime() const
    {
        return clientRequestTime_;
    }
    void setClientRequestTime(tp t)
    {
        clientRequestTime_ = t;
    }

    tp getServerReplyTime() const
    {
        return serverReplyTime_;
    }
    tp getClientReceiveTime() const
    {
        return clientReceiveTime_;
    }
#endif

    void setServerReplyTime(tp t)
    {
        serverReplyTime_ = t;
    }
    void setClientReceiveTime(tp t)
    {
        clientReceiveTime_ = t;
    }

    // Round trip time for the client-server correspondence.
    // Todo: complain if this is negative, which it should never be,
    // but is technically possible.
    dur rtt() const
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
    dur getErrorBound() const
    {
        const auto t = rtt();
        return t == durInvalid ? durInvalid : t / 2;
    }

    // Dump this packet to STDOUT, for debugging.
    void print() const;
};

}  // namespace dex
