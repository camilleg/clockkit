#include "ClockPacket.h"

#include <iostream>

#include "Common.h"

namespace dex {

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
    , clientRequestTime_(Timestamp::bytesToTimestamp(buffer + 2))
    , serverReplyTime_(Timestamp::bytesToTimestamp(buffer + 10))
    , clientReceiveTime_(Timestamp::bytesToTimestamp(buffer + 18))
{
}

void ClockPacket::write(uint8_t* buffer) const
{
    buffer[0] = static_cast<uint8_t>(type_);
    buffer[1] = sequenceNumber_;
    auto ts = Timestamp::timestampToBytes(clientRequestTime_);
    std::copy(ts.begin(), ts.end(), buffer + 2);

    ts = Timestamp::timestampToBytes(serverReplyTime_);
    std::copy(ts.begin(), ts.end(), buffer + 10);

    ts = Timestamp::timestampToBytes(clientReceiveTime_);
    std::copy(ts.begin(), ts.end(), buffer + 18);
}

void ClockPacket::print() const
{
    std::cout << "--- PACKET ---" << std::endl
              << "clientRequestTime: " << Timestamp::timestampToString(clientRequestTime_) << "\n"
              << "serverReplyTime: " << Timestamp::timestampToString(serverReplyTime_) << "\n"
              << "clientReceiveTime: " << Timestamp::timestampToString(clientReceiveTime_) << "\n";
}

}  // namespace dex
