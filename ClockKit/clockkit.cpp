#include "clockkit.h"
#include "ClockClient.h"
#include "ConfigReader.h"
#include "Exceptions.h"
#include "HighResolutionClock.h"
#include "PhaseLockedClock.h"

dex::PhaseLockedClock* ckClock = NULL;

void ckInitialize()
{
    if (ckClock != NULL)
        return;
    ckClock = dex::PhaseLockedClockFromConfigFile(dex::DEFAULT_CONFIG_FILE_PATH);
}

void ckInitializeFromConfig(const char* path)
{
    if (ckClock != NULL)
        return;
    ckClock = dex::PhaseLockedClockFromConfigFile(std::string(path));
}

dex::timestamp_t ckTimeAsValue()
{
    /* Avoid error but at the same time allow manual calling of ckInitialize
     * with non-default config */
    if (ckClock == NULL)
        ckInitialize();
    try {
        return ckClock->getValue();
    }
    catch (dex::ClockException e) {
        return 0;
    }
}

const char* ckTimeAsString()
{
    if (ckClock == NULL)
        ckInitialize();
    try {
        return dex::Timestamp::timestampToString(ckClock->getValue()).c_str();
    }
    catch (dex::ClockException e) {
        return "";
    }
}

bool ckInSync()
{
    if (ckClock == NULL)
        ckInitialize();
    return ckClock->isSynchronized();
}

int ckOffset()
{
    if (ckClock == NULL)
        ckInitialize();
    try {
        return ckClock->getOffset();
    }
    catch (dex::ClockException e) {
        return 0;
    }
}
