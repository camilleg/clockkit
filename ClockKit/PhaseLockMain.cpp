
#include "PhaseLockedClock.h"
#include "ConfigReader.h"
#include "Exceptions.h"

#include <cc++/socket.h>
#include <cc++/thread.h>

using namespace std;
using namespace ost;
using namespace dex;

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        cout << "usage phaseLock [config file]" << endl;
        return 0;
    }

    string configFile;
    if (argc == 2) configFile = argv[1];
    else configFile = DEFAULT_CONFIG_FILE_PATH;

    PhaseLockedClock* plc =
        PhaseLockedClockFromConfigFile( configFile );

    while (true)
    {
        try
        {
            int offset = plc->getOffset();            
            timestamp_t now = plc->getValue();
			cout << "offset: " << offset << endl;
			cout << "time: " << Timestamp::timestampToString(now) << endl;
        }
        catch (ClockException e)
        {
            cout << "offset: OUT OF SYNC" << endl;
        }

        Thread::sleep(1000);
    }
    
    cout << "DONE." << endl;
    return 0;
}

