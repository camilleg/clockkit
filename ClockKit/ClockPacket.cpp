#include "ClockPacket.h"

#include <iostream>

namespace dex {

ClockPacket::ClockPacket()
    : sequenceNumber_(0)
    , type_(INVALID)
    , clientRequestTime_(0)
    , serverReplyTime_(0)
    , clientReceiveTime_(0)
{
}

ClockPacket::ClockPacket(Type t, uint8_t n, timestamp_t crt)
    : sequenceNumber_(n)
    , type_(t)
    , clientRequestTime_(crt)
    , serverReplyTime_(0)
    , clientReceiveTime_(0)
{
}

ClockPacket::ClockPacket(uint8_t* buffer)
    : sequenceNumber_(buffer[1])
    , type_(Type(buffer[0]))
    , clientRequestTime_(bytesToTimestamp(buffer + 2))
    , serverReplyTime_(bytesToTimestamp(buffer + 10))
    , clientReceiveTime_(bytesToTimestamp(buffer + 18))
{
}

void ClockPacket::write(uint8_t* buffer) const
{
    buffer[0] = static_cast<uint8_t>(type_);
    buffer[1] = sequenceNumber_;
    auto t = timestampToBytes(clientRequestTime_);
    std::copy(t.begin(), t.end(), buffer + 2);

    t = timestampToBytes(serverReplyTime_);
    std::copy(t.begin(), t.end(), buffer + 10);

    t = timestampToBytes(clientReceiveTime_);
    std::copy(t.begin(), t.end(), buffer + 18);
}

void ClockPacket::print() const
{
    std::cout << "--- PACKET ---"
              << "\nclientRequestTime: " << timestampToString(clientRequestTime_)
              << "\nserverReplyTime: " << timestampToString(serverReplyTime_)
              << "\nclientReceiveTime: " << timestampToString(clientReceiveTime_) << std::endl;
}

}  // namespace dex
