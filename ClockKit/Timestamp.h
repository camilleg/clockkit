#pragma once
#include <string>

using namespace std;
namespace dex {
#ifdef WIN32
	typedef __int64 timestamp_t;
#else
	typedef long long timestamp_t;
#endif

class Timestamp
{
public:
       // Converts a timestamp to a string.
    static string timestampToString(timestamp_t t);
    
    // Converts a string to a timestamp. 
    static timestamp_t stringToTimestamp(string t);

    // Writes a 64 bit timestamp with proper byte ordering.
    static void timestampToBytes(timestamp_t time, char* buffer);
    
    // Reads a 64 bit timestamp with proper byte ordering.
    static timestamp_t bytesToTimestamp(char* buffer);
    
private:
        Timestamp();
};

}
