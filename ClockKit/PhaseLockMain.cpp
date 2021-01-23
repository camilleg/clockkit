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
        cerr << "usage: " << argv[0] << " [config_file]" << endl;
        return 1;
    }
    PhaseLockedClock* plc = PhaseLockedClockFromConfigFile(
	argc == 2 ? argv[1] : DEFAULT_CONFIG_FILE_PATH);
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
    return 0;
}
