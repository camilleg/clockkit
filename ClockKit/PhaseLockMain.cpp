#include <cc++/socket.h>
#include <cc++/thread.h>
#include "ConfigReader.h"
#include "Exceptions.h"
#include "PhaseLockedClock.h"

using namespace std;
using namespace ost;
using namespace dex;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        cerr << "usage: " << argv[0] << " config_file" << endl;
        return 1;
    }

    PhaseLockedClock* plc = PhaseLockedClockFromConfigFile(argv[1]);
    while (true) {
        try {
            const int offset = plc->getOffset();
            const timestamp_t now = plc->getValue();
            cout << "offset: " << offset << endl;
            cout << "time: " << Timestamp::timestampToString(now) << endl;
        }
        catch (ClockException e) {
            cout << "offset: OUT OF SYNC" << endl;
        }
        Thread::sleep(1000);
    }
    return 0;
}
