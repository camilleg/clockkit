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

ClockPacket::ClockPacket(const std::byte* buffer)
    : sequenceNumber_(static_cast<seqnum>(buffer[1]))
    , type_(static_cast<Type>(buffer[0]))
    , clientRequestTime_(bytesToTimestamp(buffer + 2))
    , serverReplyTime_(bytesToTimestamp(buffer + 10))
    , clientReceiveTime_(bytesToTimestamp(buffer + 18))
{
}

void ClockPacket::write(std::byte* buffer) const
{
    buffer[0] = static_cast<std::byte>(type_);
    buffer[1] = static_cast<std::byte>(sequenceNumber_);
    auto t = timestampToBytes(clientRequestTime_);
    std::copy(t.begin(), t.end(), buffer + 2);

    t = timestampToBytes(serverReplyTime_);
    std::copy(t.begin(), t.end(), buffer + 10);

    t = timestampToBytes(clientReceiveTime_);
    std::copy(t.begin(), t.end(), buffer + 18);
}

const char* ClockPacket::getTypeName() const
{
    static const char* names[KILL + 1] = {"INVALID", "REQUEST", "REPLY", "ACKNOWLEDGE", "KILL"};
    return (type_ < 0 || type_ > KILL) ? "CORRUPT" : names[type_];
}

void ClockPacket::print() const
{
    std::cout << "--- PACKET ---"
              << "\nclientRequestTime: " << timestampToString(clientRequestTime_)
              << "\nserverReplyTime: " << timestampToString(serverReplyTime_)
              << "\nclientReceiveTime: " << timestampToString(clientReceiveTime_) << std::endl;
}

}  // namespace dex
