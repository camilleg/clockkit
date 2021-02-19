#include <cc++/socket.h>
#include <cc++/thread.h>

#include "ConfigReader.h"
#include "Exceptions.h"
#include "PhaseLockedClock.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " config_file\n";
        return 1;
    }

    dex::PhaseLockedClock* clock = dex::PhaseLockedClockFromConfigFile(argv[1]);
    while (true) {
        try {
            std::cout << "offset: " << clock->getOffset()
                      << "\ntime: " << dex::Timestamp::timestampToString(clock->getValue())
                      << std::endl;
            // endl flushes stdout, to show output even after Ctrl+C.
        }
        catch (dex::ClockException& e) {
            std::cout << "offset: OUT OF SYNC\n";
        }
        ost::Thread::sleep(100);
    }
    return 0;
}
