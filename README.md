# Synchronous data collection from diverse hardware

*Camille Goudeseune and Braden Kowitz  
Integrated Systems Laboratory, Beckman Institute, UIUC*

Software revised and published on GitHub in 2020.  
Originally published in 2004 on [http://zx81.isl.uiuc.edu/camilleg/clockkit](https://web.archive.org/web/20041205064911/http://zx81.isl.uiuc.edu/camilleg/dsceu04.pdf) (defunct).

We describe an accurate open-source C++ distributed clock for networked
commodity PCs.  With no extra hardware, this clock correlates sensor data
(head- and eye-trackers, biometrics, captured video, driving simulator
data) from multiple PC's with latency and jitter under 10 microseconds
average, 100 microseconds worst case.  PC-driven actuators like motion
bases and audio/visual/haptic warning systems are also controlled with
the same accuracy.  This lets us accurately measure driver response time
(brake at a stoplight, direct gaze at a hazard, answer a telephone).

Hardware vendors often assume that system integration revolves around
their own devices.  This clock synchronizes devices despite such assumptions.

The clock is orders of magnitude more accurate than conventional methods
in Microsoft Windows, even without real-time priority or busy-waiting.
A Linux master clock provides a stable NTP time base.  Slave clocks,
Linux or Windows, synchronize to the master clock by several mechanisms.
Measuring round-trip ping times corrects for network latency.  In the
slave's high-resolution clock, drift is predictively compensated for
by second-order curve fitting while wraparound and jitter (e.g., from
PCI bus contention) is trapped.  Performance degrades gracefully and
measurably on unreliable networks.  Several phase-locked loops, within
each slave and between slave and master, guarantee performance.

Here is the 2004 [conference paper](dsceu04.pdf).

The source code is licensed under the [Creative Commons 2.0 Attribution License](http://creativecommons.org/licenses/by/2.0).

To cite this work, use:  
Camille Goudeseune and Braden Kowitz.  2004.  "Synchronous data collection from diverse hardware."  
*Driving Simulation Conference - Europe (Conférence Simulation de Conduite)*, pp. 245-252. 

### To install:

#### Ubuntu 20:
`sudo apt install make g++ libcommoncpp2-dev swig tcl8.6-dev libpython3.8-dev ruby ruby2.7-dev`  
`cd ClockKit && make`

#### Ubuntu 18 or Raspberry Pi 3 (Debian):
`sudo apt install make g++ libcommoncpp2-dev swig tcl8.6-dev libpython3.8-dev ruby ruby2.5-dev`  
`cd ClockKit && make`

#### Windows 10:
Install [Windows Subsystem for Linux](https://docs.microsoft.com/en-us/windows/wsl/install-win10), using the [Ubuntu 18](https://www.microsoft.com/store/apps/9N9TNGVNDL3Q) or [Ubuntu 20](https://www.microsoft.com/store/apps/9n6svws3rx71) distro.  
`sudo apt install make g++ tcl`  
Proceed as with Ubuntu 18 or 20.  
(Native builds for older versions of Windows may return, but no older than XP.)

### To run a test on localhost:
`cd ClockKit && make test-all`

### To plot the accuracy of a simulated run:
`sudo apt install gnuplot`  
`cd simulation && make`

## Contributing
The project is currently undergoing significant changes. Contributions of any
form are welcome nonetheless.

- To maintain the formatting style, use `clang-format` through the provided git hook:  
  `git config core.hooksPath .git_managed_hooks`
- New code should follow the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c-core-guidelines).
