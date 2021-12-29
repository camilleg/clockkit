#include "ClockPacket.h"

namespace dex {

ClockPacket::ClockPacket()
    : type_(INVALID)
    , sequenceNumber_(0)
    , clientRequestTime_(tpInvalid)
    , serverReplyTime_(tpInvalid)
    , clientReceiveTime_(tpInvalid)
{
}

ClockPacket::ClockPacket(Type t, seqnum n, tp crt)
    : type_(t)
    , sequenceNumber_(n)
    , clientRequestTime_(crt)
    , serverReplyTime_(tpInvalid)
    , clientReceiveTime_(tpInvalid)
{
}

ClockPacket::ClockPacket(const packetbuf& buffer)
    : type_(static_cast<Type>(buffer[0]))
    , sequenceNumber_(static_cast<seqnum>(buffer[1]))
    , clientRequestTime_(TpFromBytes(buffer.data() + 2))
    , serverReplyTime_(TpFromBytes(buffer.data() + 10))
    , clientReceiveTime_(TpFromBytes(buffer.data() + 18))
{
}

void ClockPacket::write(packetbuf& buffer) const
{
    buffer[0] = static_cast<std::byte>(type_);
    buffer[1] = static_cast<std::byte>(sequenceNumber_);
    BytesFromTp(clientRequestTime_, buffer.data() + 2);
    BytesFromTp(serverReplyTime_, buffer.data() + 10);
    BytesFromTp(clientReceiveTime_, buffer.data() + 18);
}

const char* ClockPacket::getTypeName() const
{
    static const char* names[KILL + 1] = {"INVALID", "REQUEST", "REPLY", "ACKNOWLEDGE", "KILL"};
    return (type_ < 0 || type_ > KILL) ? "CORRUPT" : names[type_];
}

void ClockPacket::print() const
{
    std::cout << "--- Packet " << getTypeName() << " ---"
              << "\n  clientRequestTime " << StringFromTp(clientRequestTime_) << "\n    serverReplyTime "
              << StringFromTp(serverReplyTime_) << "\n  clientReceiveTime " << StringFromTp(clientReceiveTime_)
              << std::endl;
}

}  // namespace dex
