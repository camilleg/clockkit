#include "ClockPacket.h"
#include <iostream>
#include "Common.h"

namespace dex {

ClockPacket::ClockPacket()
    : type_(INVALID)
    , sequenceNumber_(0)
    , clientRequestTime_(0)
    , serverReplyTime_(0)
    , clientReceiveTime_(0)
{
}

void ClockPacket::read(char* buffer)
{
    type_ = Type(buffer[0]);
    sequenceNumber_ = buffer[1];
    clientRequestTime_ = Timestamp::bytesToTimestamp(buffer + 2);
    serverReplyTime_ = Timestamp::bytesToTimestamp(buffer + 10);
    clientReceiveTime_ = Timestamp::bytesToTimestamp(buffer + 18);
}

void ClockPacket::write(char* buffer) const
{
    buffer[0] = (unsigned char)type_;
    buffer[1] = sequenceNumber_;
    Timestamp::timestampToBytes(clientRequestTime_, buffer + 2);
    Timestamp::timestampToBytes(serverReplyTime_, buffer + 10);
    Timestamp::timestampToBytes(clientReceiveTime_, buffer + 18);
}

timestamp_t ClockPacket::getRTT() const
{
    return clientReceiveTime_ - clientRequestTime_;
}

timestamp_t ClockPacket::getClockOffset() const
{
    return serverReplyTime_ + getErrorBound() - clientReceiveTime_;
}

timestamp_t ClockPacket::getErrorBound() const
{
    return getRTT() / 2;
}

void ClockPacket::print() const
{
    std::cout << "--- PACKET ---" << std::endl
              << "clientRequestTime: " << Timestamp::timestampToString(clientRequestTime_)
              << std::endl
              << "serverReplyTime: " << Timestamp::timestampToString(serverReplyTime_) << std::endl
              << "clientReceiveTime: " << Timestamp::timestampToString(clientReceiveTime_)
              << std::endl;
}

}  // namespace dex
