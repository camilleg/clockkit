
#include "ClockClient.h"
#include "HighResolutionClock.h"
#include "Exceptions.h"
#include <cc++/socket.h>
#include <cc++/thread.h>


using namespace std;
using namespace ost;
using namespace dex;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "usage clockClient <server address> <port>" << endl;
        return 0;
    }

    InetHostAddress addr(argv[1]);
    int port = atoi(argv[2]);

    cout << "Starting Client:" << endl;
    cout << "  bind: " << addr.getHostname() << endl;
    cout << "  port: " << port << endl;
        

    Clock& hires = HighResolutionClock::instance();
    ClockClient client(addr, port);
    client.setTimeout(15000);
    client.setAcknowledge(true);

    while (true)
    {
        try
        {
            timestamp_t now = client.getPhase(hires);
            cout << "offset: " << now
                 << "\trtt: " << client.getLastRTT() << endl;
        }
        catch (ClockException e) {}
        Thread::sleep(1000);
    }

    return 0;
}

