Clockkit provides timestamps to distributed networked PCs
with guaranteed bounds on latency and jitter, typically under 10 microseconds,
as described in the conference paper [Synchronous data collection from diverse hardware](dsceu04.pdf).

It runs on Linux, Windows, and Raspi,
and needs neither extra hardware nor elevated privileges.

It can measure a system's realtime behavior,
by providing a common time reference for events recorded by different sensors
(audio, video, gamepad, GPS, SMS, MIDI, biometrics),
and for triggering outputs (audio, video, LEDs, servos, motion bases).  
It did this originally for a full-motion [driving simulator](https://web.archive.org/web/20170517201424/http://www.isl.uiuc.edu/Labs/Driving%20Simulator/Driving%20Simulator.html) with eye tracking and a
quickly churning set of other sensors and outputs, for over a decade.

Clockkit was published in 2004 on [http://zx81.isl.uiuc.edu/camilleg/clockkit](https://web.archive.org/web/20041205064911/http://zx81.isl.uiuc.edu/camilleg/dsceu04.pdf) (defunct).  
It was revised and moved to GitHub in 2020.

The source code is licensed under the [MIT License](https://mit-license.org/).

## To install:

#### Ubuntu 20:
`sudo apt install make g++ swig tcl8.6-dev libpython3.8-dev ruby ruby2.7-dev`  
`cd ClockKit && make`

#### Ubuntu 18:<br>Raspberry Pi 3 and 4 (Debian/Raspbian):
`sudo apt install make g++ swig tcl tcl8.6-dev libpython3.8-dev ruby ruby2.5-dev`  
`cd ClockKit && make`

#### Windows 10:
Install [Windows Subsystem for Linux](https://docs.microsoft.com/en-us/windows/wsl/install-win10), using the [Ubuntu 18](https://www.microsoft.com/store/apps/9N9TNGVNDL3Q) or [Ubuntu 20](https://www.microsoft.com/store/apps/9n6svws3rx71) distro.  
`sudo apt install tcl`  
Proceed as with Ubuntu 18 or 20.  
(Native builds for older versions of Windows may return, but no older than XP.)

### To run a test on localhost:
`cd ClockKit && make test`

### To sync host B to host A:
On host A, `ckserver 4567`  
On host B:  
- `cp clockkit.conf my-clockkit.conf`
- Edit `my-clockkit.conf`.  Set the `server` to host A, e.g., `192.168.1.1` or `myhost.example.com`.  Set the `port` to 4567, or whatever port you told ckserver to use.  
- `./ckphaselock my-clockkit.conf`  
(`make test-remote` automates this, using an ssh key.)

Of course, these steps for host B can be repeated on other hosts C, D, E,... to sync them all.
Remember that "syncing" here means providing synchronized timestamps, *not* adjusting the hosts' own clocks.

### To plot performance:
`sudo apt install gnuplot`  
`cd simulation && make`

## To cite:
Camille Goudeseune and Braden Kowitz.  2004.  "Synchronous data collection from diverse hardware."  
*Driving Simulation Conference - Europe (Conférence Simulation de Conduite)*, pp. 245-252. 

## Contributing
The project is currently undergoing significant changes. Contributions of any
form are welcome nonetheless.

- To maintain the formatting style, `sudo apt install clang-format`, and use `clang-format` through the provided git hook:  
  `git config core.hooksPath .git_managed_hooks`
- New code should follow the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c-core-guidelines).
- For profiling and code coverage, `sudo apt install gcovr`.  See also `man gcovr`.  
  To collect and print statistics, `make clean && make profile`, run some tests (but not test-bindings), `gcovr`.  
  To reset statistics before another profile, `make purge`.  
  To cease profiling, `make purge && make`.

## Roadmap
When this software launched in 2004, lab software was pretty much restricted to
desktop OSes.  But by now, labs and makerspaces use many more software
development environments, especially for hardware I/O:
Arduino, musl, Raspi, and smartphones to name a few.
The choice of mature scripting languages has grown similarly.

Also, private wired 10baseT subnets have been pretty much replaced by WLAN,
with much more bandwidth but less predictable performance.

Finally, since 2004, C++ standards have improved
and software engineering in general has matured.

Therefore, these steps are proposed.

- Keep modernizing the code.
- Clean up the interface to other languages.
- Implement integration testing.
- Make reproducible performance tests for some use cases.
- Extend multiplatform support beyond POSIX, for other microarchitectures.
- For some use cases, reduce energy consumption, file size, bandwidth.
- To better exploit the strengths and manage the weaknesses of WLAN,
replace the generic network stack's lower OSI layers with
specific ones for Wi-Fi, Bluetooth LE, Zigbee, 6LoWPAN, etc.
- Throughout all these, insert optimization passes.
- Explore more distant use cases that need clock sync, such as
high performance computing, logfile evaluation, and security breach detection.
