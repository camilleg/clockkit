#include <cc++/socket.h>
#include <cc++/thread.h>

#include "ClockClient.h"
#include "Exceptions.h"
#include "HighResolutionClock.h"

using namespace std;
using namespace ost;
using namespace dex;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        cerr << "usage: " << argv[0] << " <server_ip_address> <port>\n";
        return 1;
    }
    const InetHostAddress addr(argv[1]);
    const int port = atoi(argv[2]);
    cout << "Starting client for " << addr.getHostname() << ":" << port << "\n";

    Clock& hires = HighResolutionClock::instance();
    try {
        ClockClient client(addr, port);
        client.setTimeout(15000);
        client.setAcknowledge(true);

        while (true) {
            try {
                const timestamp_t now = client.getPhase(hires);
                cout << "offset: " << now << "\trtt: " << client.getLastRTT() << "\n";
            }
            catch (ClockException& e) {
            }
            Thread::sleep(1000);
        }
    }
    catch (Socket*) {
        cout << "Failed to get any outgoing port\n";
        return 127;
    }
    return 0;
}
