//----------------------------------------------------------------------------//
#ifndef DEX_TIMESTAMP_CPP
#define DEX_TIMESTAMP_CPP
//----------------------------------------------------------------------------//
#include "Timestamp.h"
#include "Common.h"
#include <string>
#include <stdio.h>
#include <cc++/config.h>
//----------------------------------------------------------------------------//
using namespace std;
namespace dex {
//----------------------------------------------------------------------------//


string Timestamp::timestampToString(timestamp_t t)
{
	const timestamp_t second = 1000000;
	int secs  = (int) (t / second);
	int usecs = (int) (t % second);

    char buff[256];
    sprintf(buff, "<time %i %i>", secs, usecs); 
    return string(buff);
}

timestamp_t Timestamp::stringToTimestamp(string t)
{
	int secs, usecs;
	if (sscanf(t.c_str(), "<time %i %i>", &secs, &usecs) != 2)
	  return 0;
	const timestamp_t second = 1000000;
	return timestamp_t(secs * second) + usecs;
}


void Timestamp::timestampToBytes(timestamp_t time, char* buffer)
{
	char* t = (char*) &time;
	if (__BYTE_ORDER == __BIG_ENDIAN)
		for (int i=0; i<8; i++) buffer[i] = t[i];	
	else
		for (int i=0; i<8; i++) buffer[i] = t[7-i];
}

timestamp_t Timestamp::bytesToTimestamp(char* buffer)
{
	timestamp_t time;
	char* t = (char*) &time;
	if (__BYTE_ORDER == __BIG_ENDIAN) 
		for (int i=0; i<8; i++) t[i] = buffer[i];	
	else
		for (int i=0; i<8; i++) t[i] = buffer[7-i];
	return time;
}

//----------------------------------------------------------------------------//
} // namespace dex

//----------------------------------------------------------------------------//
#endif //DEX_TIMESTAMP_CPP
