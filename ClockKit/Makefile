CPP = c++
# -fPIC is for */*.so.
CFLAGS = -O3 -fPIC
LIBS = -lccgnu2 -lpthread -ldl
SWIG = swig -Wallkw -Werror

OBJECTS = \
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

EXES = \
  ckserver \
  ckphaselock \
  python/_clockkit.so \
  ruby/clockkit.so \
  tcl/clockkit.so

all: $(EXES)

test-all: test test-tcl test-python test-ruby

test: ckserver ckphaselock
	./testcase.sh
test-python: ckserver python/_clockkit.so
	cd python && ./testcase.sh
test-ruby: ckserver ruby/clockkit.so
	cd ruby && ./testcase.sh
test-tcl: ckserver tcl/clockkit.so
	cd tcl && ./testcase.sh

ckserver: ClockServerMain.cpp $(OBJECTS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

ckphaselock: PhaseLockMain.cpp $(OBJECTS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

# <python.h>, <ruby.h>, and <tcl.h> might be in different directories than these -I's.

python/wrap.c:
	$(SWIG) -python -o $@ clockkit.h
python/wrap.o: python/wrap.c
	$(CPP) -c $(CFLAGS) -I. -I/usr/include/python3.8 $? -o $@
python/_clockkit.so: python/wrap.o $(OBJECTS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

# In lines starting with rb_define_module_function, cast _wrap_ck's to (VALUE (*)(...)).
ruby/wrap.c:
	$(SWIG) -ruby -o $@ clockkit.h
	sed -i -e "s/ _wrap_ck/ (VALUE (*)(...))_wrap_ck/" $@
ruby/wrap.o: ruby/wrap.c
	$(CPP) -c $(CFLAGS) -I. -I/usr/include/ruby-2.5.0 -I/usr/include/x86_64-linux-gnu/ruby-2.5.0 $? -o $@
ruby/clockkit.so: ruby/wrap.o $(OBJECTS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

tcl/wrap.c:
	$(SWIG) -tcl -o $@ clockkit.h
tcl/wrap.o: tcl/wrap.c
	$(CPP) -c $(CFLAGS) -I. -I/usr/include/tcl8.6 $? -o $@
tcl/clockkit.so: tcl/wrap.o $(OBJECTS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

docs:
	doxygen doxygen.conf

clean:
	rm -rf $(OBJECTS) $(EXES) docs doxygen.warn */wrap.c */wrap.o python/clockkit.py

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCLUDE) $< 

.c.o:
	$(CPP) -c $(CFLAGS) $(INCLUDE) $<
