#include "ClockServer.h"
#include <cc++/socket.h>
#include "VariableFrequencyClock.h"
#include "HighResolutionClock.h"
#include <cstdlib>

using namespace std;
using namespace ost;
using namespace dex;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "usage: clockServer <port>" << endl;
        return 0;
    }

    int port = atoi(argv[1]);
    InetAddress addr( "0.0.0.0" );
    
    // used to create a off-frequency clock for testing
    //VariableFrequencyClock vfc(HighResolutionClock::instance());
    //vfc.setFrequency(1000000 - 2000);
    //ClockServer server(addr, port, vfc);
    ClockServer server(addr, port, HighResolutionClock::instance());
    
    server.setLogging(true);
    server.start();
    server.join();
    return 0;
}
