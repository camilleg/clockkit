#include <cc++/socket.h>

#include "clockkit.h"

#include "ConfigReader.h"
#include "ClockClient.h"
#include "HighResolutionClock.h"
#include "PhaseLockedClock.h"
#include "Exceptions.h"

dex::PhaseLockedClock* ckClock = NULL;
std::string ckTimeString;

void ckInitialize()
{
    if (ckClock != NULL) return;
    ckClock = dex::PhaseLockedClockFromConfigFile(dex::DEFAULT_CONFIG_FILE_PATH);
}


dex::timestamp_t ckTimeAsValue()
{
    ckInitialize();
    try
    {
        return ckClock->getValue();
    }
    catch (dex::ClockException e)
    {
        return 0;
    }
}

const char* ckTimeAsString()
{
	ckInitialize();
	try
    {
    	ckTimeString = dex::Timestamp::timestampToString( ckClock->getValue() );
    }
    catch (dex::ClockException e)
    {
        ckTimeString = "";
    }
    return ckTimeString.c_str();
}


bool ckInSync()
{
	ckInitialize();
	return ckClock->isSynchronized();
}

int ckOffset()
{
	ckInitialize();
	try
	{
		return ckClock->getOffset();
	}
	catch (dex::ClockException e)
	{
		return 0;	
	}
}
