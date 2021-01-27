CPP = c++
INCLUDE = -I/usr/include/python3.8
LIBDIR = 
LIBS = -lccgnu2 -lpthread -ldl
CFLAGS = -O -fPIC

OBJECTS = \
  Clock.o \
  ClockClient.o \
  ClockPacket.o \
  ClockServer.o \
  ConfigReader.o \
  Exceptions.o \
  PhaseLockedClock.o \
  SystemClock.o \
  Timestamp.o \
  VariableFrequencyClock.o \
  clockkit.o

all: $(OBJECTS) ckserver ckphaselock tcl ruby python

test: ckserver ckphaselock
	./testcase.sh

# To test, run "./ckserver 4444" in one window, and in other windows run
# ckphaselock, ckphaselock.py, ckphaselock.rb, or ckphaselock.tcl.

tcl: clockkit_tcl.so
python: _clockkit.so
ruby: clockkit.so

ckserver: ClockServerMain.o $(OBJECTS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

ckphaselock: PhaseLockMain.o $(OBJECTS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

# Your <tcl.h> may be in a different directory than this -I.
clockkit_tcl_wrap.c:
	swig -tcl -o $@ clockkit.h > $@
clockkit_tcl_wrap.o: clockkit_tcl_wrap.c
	$(CPP) -c $(CFLAGS) $(INCLUDE) -I/usr/include/tcl8.6 $?
clockkit_tcl.so: clockkit_tcl_wrap.o $(OBJECTS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

# Prepend the cast (VALUE (*)(...)) to _wrap* in lines starting with rb_define_module_function.
# Name the target clockkit.so, not clockkit_ruby.so,
# because Ruby needs foo.so to have the same name as the module Foo.
# Your <ruby.h> may be in a different directory than this -I.
clockkit_ruby_wrap.c:
	swig -ruby -o /tmp/clockkit.tmp clockkit.h
	sed -e "s/ _wrap_ck/ (VALUE (*)(...))_wrap_ck/" /tmp/clockkit.tmp > $@
	rm /tmp/clockkit.tmp
clockkit_ruby_wrap.o: clockkit_ruby_wrap.c
	$(CPP) -c $(CFLAGS) $(INCLUDE) -I/usr/include/ruby-2.5.0 -I/usr/include/x86_64-linux-gnu/ruby-2.5.0 $?
clockkit.so: clockkit_ruby_wrap.o $(OBJECTS)
	$(CPP) $(CFLAGS) -shared clockkit_ruby_wrap.o $(OBJECTS) $(LIBS) -o $@

# Bug: if Ruby's clockkit.so was built, ckphaselock.py uses it instead of _clockkit.so.
# Bug: implicit relative imports stopped working in Python 3.  It's broken.
clockkit_python_wrap.c:
	swig -python -o $@ clockkit.h
clockkit_python_wrap.o: clockkit_python_wrap.c
	$(CPP) -c $(CFLAGS) $(INCLUDE) -I/usr/include/python3.8 $?
_clockkit.so: clockkit_python_wrap.o $(OBJECTS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

docs:
	doxygen doxygen.conf

clean:
	rm -rf *.o ckserver ckphaselock docs doxygen.warn *.pyc clockkit_*_wrap.c *clockkit*.so clockkit.py

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCLUDE) $< 

.c.o:
	$(CPP) -c $(CFLAGS) $(INCLUDE) $<
