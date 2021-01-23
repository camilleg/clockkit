#ifndef DEX_CONFIG_READER_CPP
#define DEX_CONFIG_READER_CPP

#include <cc++/socket.h>

#include "ConfigReader.h"

#include "Common.h"
#include "ClockClient.h"
#include "HighResolutionClock.h"
#include "PhaseLockedClock.h"
#include "Exceptions.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

namespace dex {

#ifdef WIN32
    const string DEFAULT_CONFIG_FILE_PATH = "C:\\clockkit.conf";
#else
    const string DEFAULT_CONFIG_FILE_PATH = "/etc/clockkit.conf";
#endif

PhaseLockedClock* PhaseLockedClockFromConfigFile(string filename)
{
    string server = "localhost";
    int port = 4444;
    int timeout = 1000;
    int phasePanic = 5000;
    int updatePanic = 5000000;

    ifstream file(filename.c_str());
    if (!file.is_open())
        throw Exception("failed to open config file " + DEFAULT_CONFIG_FILE_PATH);

    while (!file.eof())
    {
        string line;
        file >> line;
        int pos = line.find(":");
        if (pos < 0) break;
        string name = line.substr(0,pos);
        string value = line.substr(pos+1);

        if (name == "server")
        {
            server = value;
        }
        else if (name == "port")
        {
            port = atoi(value.c_str());
        }
        else if (name == "timeout")
        {
            timeout = atoi(value.c_str());
        }
        else if (name == "phasePanic")
        {
            phasePanic = atoi(value.c_str());
        }
        else if (name == "updatePanic")
        {
            updatePanic = atoi(value.c_str());
        }
    }

    file.close();

    //TODO separate printing from object creation
    cout << "config [server:" << server << "]" << endl;
    cout << "config [port:" << port << "]" << endl;
    cout << "config [timeout:" << timeout << "]" << endl;
    cout << "config [phasePanic:" << phasePanic << "]" << endl;
    cout << "config [updatePanic:" << updatePanic << "]" << endl;

    ost::InetHostAddress addr( server.c_str() );
    ClockClient* client = new ClockClient(addr, port);
    client->setTimeout(timeout);
    client->setAcknowledge(true);
    PhaseLockedClock* plc = new PhaseLockedClock(
        HighResolutionClock::instance(),
        *client);
    plc->setPhasePanic(phasePanic);
    plc->setUpdatePanic(updatePanic);

    return plc;
}

} // namespace dex

#endif //DEX_CONFIG_READER_CPP
