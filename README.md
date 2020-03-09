# Synchronous data collection from diverse hardware

*Camille Goudeseune and Braden Kowitz  
Integrated Systems Laboratory, Beckman Institute, UIUC*

Published in 2004 on http://zx81.isl.uiuc.edu/camilleg/clockkit (defunct).  
Software revised and published on GitHub in in 2020.

We describe an accurate open-source C++ distributed clock for networked
commodity PC's.  With no extra hardware, this clock correlates sensor data
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

Here is the [complete paper](dsceu04.pdf).
