#ifndef DEX_CLOCK_PACKET_H
#define DEX_CLOCK_PACKET_H

#include "Common.h"

namespace dex {

/**
 * Used with ClockServer and ClockClient.
 * This is the UDPpacket that gets send over the netowrk.
 */
class ClockPacket
{
public:
    /**
     * - INVALID: Used to detect uninitialized packets.
     * - REQUEST: Client sends packet to server requesting current time.
     * - REPLY: Server sends client the current time.
     * - ACKNOWLEDGE: Clent sends server the status of it's synchronization.
     */
    enum Type
	{
		INVALID = 0,
		REQUEST = 1,
		REPLY = 2,
		ACKNOWLEDGE = 3
	};
    
    /**
     * each packet encodes:
     * - type : 1 byte
     * - sequence : 1 bytes
     * - 3 timestamps : 8 bytes each
     * - total : 26 bytes
     */
    //static const int 
	enum
	{
		PACKET_LENGTH = 26
	};

private:
    Type type_;
    unsigned char sequenceNumber_;
    timestamp_t clientRequestTime_;
    timestamp_t serverReplyTime_;
    timestamp_t clientReceiveTime_;
    
public: 
    /**
     * Creates a ClockPacket that is initially Empty.
     * The type is initially set to INVALID
     * and the sequence and times are all set to 0.
     */
    ClockPacket();
    
    /**
     * Creates an packet by reading PACKET_LENGTH bytes
     * out of the provided buffer.
     */
    ClockPacket(char* buffer);
    
    /**
     * Reads from a buffer of PACKET_LENGTH bytes and stores
     * the values into this packet.
     */
    void read(char* buffer);
    
    /**
     * Writes this packet out to a buffer of PACKET_LENGTH bytes.
     */
    void write(char* buffer);
    
    /**
     * Gets the type of this packet.
     * @see ClockPacket::Type
     */
    Type getType();
    
    /**
     * Sets the type of this packet.
     * @see ClockPacket::Type
     */
    void setType(Type t);

    /**
     * Gets the sequence number of the packet.
     * Sequence numbers range from 0 to 255.
     * They are used to prevent packets from being crossed in the communication
     * between the client and the server. The sequence number allows for
     * detection of delayed responses.
     */
    unsigned char getSequenceNumber();
    
    /**
     * Sets the sequence number of the packet.
     * Sequence numbers range from 0 to 255.
     * They are used to prevent packets from being crossed in the communication
     * between the client and the server. The sequence number allows for
     * detection of delayed responses.
     */
    void setSequenceNumber(unsigned char sequenceNumber);
    
    /**
     * Gets the Client Request Time.
     * This is the time on the client machine when the packet is sent.
     */
    timestamp_t getClientRequestTime();
    
    /**
     * Sets the Client Request Time.
     * This is the time on the client machine when the packet is sent.
     */
    void setClientRequestTime(timestamp_t t); 

    /**
     * Gets the Server Reply Time.
     * This is the time on the server when the REQUEST packet is received.
     */
    timestamp_t getServerReplyTime();
    
    /**
     * Sets the Server Reply Time.
     * This is the time on the server when the REQUEST packet is received.
     */
    void setServerReplyTime(timestamp_t t); 

    /**
     * Gets the Client Receive Time.
     * This is the time on the client when the server REPLY packet is received.
     */
    timestamp_t getClientReceiveTime();
    
    /**
     * Sets the Client Receive Time.
     * This is the time on the client when the server REPLY packet is received.
     */
    void setClientReceiveTime(timestamp_t t); 

    /**
     * Calculates the round trip time for the client-server correspondance.
     * @return round trip time in microseonds (usec).
     */
	timestamp_t getRTT();

    /**
     * Calculates the estimated offset between the client and server clocks.
     * @return estimated clock offset in microseonds (usec).
     */
	timestamp_t getClockOffset();
	
    /**
     * Gives the error bound on the clock offset calculation.
     * @return error bound in microseconds (usec).
     */
	timestamp_t getErrorBound();
    
    /**
     * Prints out this packet to STDOUT.
     * This is primarly meant for debugging.
     */
    void print();
    
};

}
#endif
