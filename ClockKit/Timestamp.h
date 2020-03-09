//----------------------------------------------------------------------------//
#ifndef DEX_TIMESTAMP_H
#define DEX_TIMESTAMP_H
//----------------------------------------------------------------------------//
#include <string>
//----------------------------------------------------------------------------//
using namespace std;
namespace dex {
//----------------------------------------------------------------------------//
#ifdef WIN32
	typedef __int64 timestamp_t;
#else
	typedef long long timestamp_t;
#endif
//----------------------------------------------------------------------------//
class Timestamp
{
public:

    /**
     * Converts a timestamp value into a string.
     */
    static string timestampToString(timestamp_t t);
    
    /**
     * Converts a string to a timestamp value. 
     */
    static timestamp_t stringToTimestamp(string t);

    /**
     * Writes a 64 bit timestamp out to buffer location
     * with proper byte ordering.
     * - Writes 8 bytes into the buffer.
     */
    static void timestampToBytes(timestamp_t time, char* buffer);
    
    /**
     * Reads a 64 bit timestamp from a buffer location
     * with proper byte ordering.
     * - Reads 8 bytes out of the buffer.
     */
    static timestamp_t bytesToTimestamp(char* buffer);
    
private:

        Timestamp();
        
}; //class Timestamp
//----------------------------------------------------------------------------//
} // namespace dex

//----------------------------------------------------------------------------//
#endif //DEX_TIMESTAMP_H

