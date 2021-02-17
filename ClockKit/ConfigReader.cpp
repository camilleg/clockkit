#include "ConfigReader.h"

#include "ClockClient.h"
#include "Common.h"
#include "Exceptions.h"
#include "HighResolutionClock.h"
#include "PhaseLockedClock.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

namespace dex {

PhaseLockedClock* PhaseLockedClockFromConfigFile(const string& filename)
{
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cerr << "failed to open config file " << filename << endl;
        return NULL;
    }

    string server = "localhost";
    int port = 4444;
    int timeout = 1000;
    int phasePanic = 5000;
    int updatePanic = 5000000;
    bool found = false;

    while (!file.eof()) {
        string line;
        file >> line;
        const int pos = line.find(":");
        if (pos < 0)
            break;
        const string name(line.substr(0, pos));
        const string value(line.substr(pos + 1));

        if (name == "server") {
            server = value;
            found = true;
        }
        else if (name == "port") {
            port = atoi(value.c_str());
            found = true;
        }
        else if (name == "timeout") {
            timeout = atoi(value.c_str());
            found = true;
        }
        else if (name == "phasePanic") {
            phasePanic = atoi(value.c_str());
            found = true;
        }
        else if (name == "updatePanic") {
            updatePanic = atoi(value.c_str());
            found = true;
        }
    }
    file.close();
    if (!found)
        cerr << "using defaults because of useless config file '" << filename << "'" << endl;

    // TODO separate printing from object creation
    cout << "config [server:" << server << "]" << endl;
    cout << "config [port:" << port << "]" << endl;
    cout << "config [timeout:" << timeout << "]" << endl;
    cout << "config [phasePanic:" << phasePanic << "]" << endl;
    cout << "config [updatePanic:" << updatePanic << "]" << endl;

    const ost::InetHostAddress addr(server.c_str());
    ClockClient* client = new ClockClient(addr, port);
    client->setTimeout(timeout);
    client->setAcknowledge(true);
    PhaseLockedClock* plc = new PhaseLockedClock(HighResolutionClock::instance(), *client);
    plc->setPhasePanic(phasePanic);
    plc->setUpdatePanic(updatePanic);
    return plc;
}

}  // namespace dex
