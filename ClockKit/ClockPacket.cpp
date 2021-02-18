#include "ClockPacket.h"

#include <iostream>

#include "Common.h"

namespace dex {

ClockPacket::ClockPacket(Type t, unsigned char n, timestamp_t crt)
    : sequenceNumber_(n)
    , type_(t)
    , clientRequestTime_(crt)
    , serverReplyTime_(0)
    , clientReceiveTime_(0)
{
}

ClockPacket::ClockPacket(const char* buffer)
    : sequenceNumber_(buffer[1])
    , type_(Type(buffer[0]))
    , clientRequestTime_(Timestamp::bytesToTimestamp(buffer + 2))
    , serverReplyTime_(Timestamp::bytesToTimestamp(buffer + 10))
    , clientReceiveTime_(Timestamp::bytesToTimestamp(buffer + 18))
{
}

void ClockPacket::write(char* buffer) const
{
    buffer[0] = (unsigned char)type_;
    buffer[1] = sequenceNumber_;
    Timestamp::timestampToBytes(clientRequestTime_, buffer + 2);
    Timestamp::timestampToBytes(serverReplyTime_, buffer + 10);
    Timestamp::timestampToBytes(clientReceiveTime_, buffer + 18);
}

void ClockPacket::print() const
{
    std::cout << "--- PACKET ---" << std::endl
              << "clientRequestTime: " << Timestamp::timestampToString(clientRequestTime_) << "\n"
              << "serverReplyTime: " << Timestamp::timestampToString(serverReplyTime_) << "\n"
              << "clientReceiveTime: " << Timestamp::timestampToString(clientReceiveTime_) << "\n";
}

}  // namespace dex
