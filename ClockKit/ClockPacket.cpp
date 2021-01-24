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

ClockPacket::ClockPacket(char* buffer)
    : type_(INVALID)
    , sequenceNumber_(0)
    , clientRequestTime_(0)
    , serverReplyTime_(0)
    , clientReceiveTime_(0)
{
    read(buffer);
}

void ClockPacket::read(char* buffer)
{
    type_ = (Type)buffer[0];
    sequenceNumber_ = buffer[1];
    clientRequestTime_ = Timestamp::bytesToTimestamp(&buffer[2]);
    serverReplyTime_ = Timestamp::bytesToTimestamp(&buffer[10]);
    clientReceiveTime_ = Timestamp::bytesToTimestamp(&buffer[18]);
}

void ClockPacket::write(char* buffer)
{
    buffer[0] = (unsigned char)type_;
    buffer[1] = sequenceNumber_;
    Timestamp::timestampToBytes(clientRequestTime_, &buffer[2]);
    Timestamp::timestampToBytes(serverReplyTime_, &buffer[10]);
    Timestamp::timestampToBytes(clientReceiveTime_, &buffer[18]);
}

ClockPacket::Type ClockPacket::getType()
{
    return type_;
}

void ClockPacket::setType(Type type)
{
    type_ = type;
}

unsigned char ClockPacket::getSequenceNumber()
{
    return sequenceNumber_;
}

void ClockPacket::setSequenceNumber(unsigned char sequenceNumber)
{
    sequenceNumber_ = sequenceNumber;
}

timestamp_t ClockPacket::getClientRequestTime()
{
    return clientRequestTime_;
}

void ClockPacket::setClientRequestTime(timestamp_t t)
{
    clientRequestTime_ = t;
}

timestamp_t ClockPacket::getServerReplyTime()
{
    return serverReplyTime_;
}

void ClockPacket::setServerReplyTime(timestamp_t t)
{
    serverReplyTime_ = t;
}

timestamp_t ClockPacket::getClientReceiveTime()
{
    return clientReceiveTime_;
}

void ClockPacket::setClientReceiveTime(timestamp_t t)
{
    clientReceiveTime_ = t;
}

timestamp_t ClockPacket::getRTT()
{
    return clientReceiveTime_ - clientRequestTime_;
}

timestamp_t ClockPacket::getClockOffset()
{
    return serverReplyTime_ + getRTT() / 2 - clientReceiveTime_;
}

timestamp_t ClockPacket::getErrorBound()
{
    return getRTT() / 2;
}

void ClockPacket::print()
{
  std::cout << "--- PACKET ---" << std::endl
         << "clientRequestTime: " << Timestamp::timestampToString(clientRequestTime_) << std::endl
         << "serverReplyTime: " << Timestamp::timestampToString(serverReplyTime_) << std::endl
         << "clientReceiveTime: " << Timestamp::timestampToString(clientReceiveTime_) << std::endl;
}

}  // namespace dex
