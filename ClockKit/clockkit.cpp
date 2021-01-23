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

void ckInitializeFromConfig(const char *path)
{
    if (ckClock != NULL) return;
    ckClock = dex::PhaseLockedClockFromConfigFile(std::string(path));
}


dex::timestamp_t ckTimeAsValue()
{
    /* Avoid error but at the same time allow manual calling of ckInitialize
     * with non-default config */
    if (ckClock == NULL) ckInitialize();
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
    if (ckClock == NULL) ckInitialize();
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
    if (ckClock == NULL) ckInitialize();
    return ckClock->isSynchronized();
}

int ckOffset()
{
    if (ckClock == NULL) ckInitialize();
    try
    {
        return ckClock->getOffset();
    }
    catch (dex::ClockException e)
    {
        return 0;
    }
}
