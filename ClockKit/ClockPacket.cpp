#include "ClockPacket.h"

#include <iostream>

namespace dex {

ClockPacket::ClockPacket()
    : sequenceNumber_(0)
    , type_(INVALID)
    , clientRequestTime_(tpInvalid)
    , serverReplyTime_(tpInvalid)
    , clientReceiveTime_(tpInvalid)
{
}

ClockPacket::ClockPacket(Type t, seqnum n, tp crt)
    : sequenceNumber_(n)
    , type_(t)
    , clientRequestTime_(crt)
    , serverReplyTime_(tpInvalid)
    , clientReceiveTime_(tpInvalid)
{
}

ClockPacket::ClockPacket(const packetbuf& buffer)
    : sequenceNumber_(static_cast<seqnum>(buffer[1]))
    , type_(static_cast<Type>(buffer[0]))
    , clientRequestTime_(bytesToTimestamp(buffer.data() + 2))
    , serverReplyTime_(bytesToTimestamp(buffer.data() + 10))
    , clientReceiveTime_(bytesToTimestamp(buffer.data() + 18))
{
}

void ClockPacket::write(packetbuf& buffer) const
{
    buffer[0] = static_cast<std::byte>(type_);
    buffer[1] = static_cast<std::byte>(sequenceNumber_);
    timestampToBytes(clientRequestTime_, buffer.data() + 2);
    timestampToBytes(serverReplyTime_, buffer.data() + 10);
    timestampToBytes(clientReceiveTime_, buffer.data() + 18);
}

const char* ClockPacket::getTypeName() const
{
    static const char* names[KILL + 1] = {"INVALID", "REQUEST", "REPLY", "ACKNOWLEDGE", "KILL"};
    return (type_ < 0 || type_ > KILL) ? "CORRUPT" : names[type_];
}

void ClockPacket::print() const
{
    std::cout << "--- Packet " << getTypeName() << " ---"
              << "\n  clientRequestTime " << timestampToString(clientRequestTime_) << "\n    serverReplyTime "
              << timestampToString(serverReplyTime_) << "\n  clientReceiveTime "
              << timestampToString(clientReceiveTime_) << std::endl;
}

}  // namespace dex
