MAKEFLAGS += --jobs=99
CPP = c++
# -fPIC is for */*.so.
CFLAGS = -O3 -fPIC -std=c++11 -Wall -Werror -Weffc++ -pedantic
LIBS = -lccgnu2 -lpthread -ldl
SWIG = swig -Wallkw -Werror
RUBY   = $$(pkg-config --list-all | grep ruby-   | sort | tail -1 | sed 's/ .*//')
PYTHON = $$(pkg-config --list-all | grep python- | sort | tail -1 | sed 's/ .*//')
# pkg-config doesn't know about tcl.

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

SWIGEXES = \
  python/_clockkit.so \
  ruby/clockkit.so \
  tcl/clockkit.so

all: $(EXES)
bindings: $(SWIGEXES)

test-bindings: test-python test-ruby test-tcl

# Different ports let these tests run simultaneously, via --jobs=99.
test: ckserver ckphaselock
	./testcase.sh 4440
test-python: ckserver python/_clockkit.so
	cd python && ./testcase.sh 4450
test-ruby: ckserver ruby/clockkit.so
	cd ruby && ./testcase.sh 4460
test-tcl: ckserver tcl/clockkit.so
	cd tcl && ./testcase.sh 4470

ckserver: ClockServerMain.cpp $(OBJECTS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

ckphaselock: PhaseLockMain.cpp $(OBJECTS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

python/wrap.c:
	$(SWIG) -python -o $@ clockkit.h
python/wrap.o: python/wrap.c
	$(CPP) -c $(CFLAGS) -I. $$(pkg-config --cflags $(PYTHON)) $? -o $@
python/_clockkit.so: python/wrap.o $(OBJECTS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

ruby/wrap.c:
	$(SWIG) -ruby -o $@ clockkit.h
	# In lines starting with rb_define_module_function, cast _wrap_ck's to (VALUE (*)(...)).
	sed -i -e "s/ _wrap_ck/ (VALUE (*)(...))_wrap_ck/" $@
ruby/wrap.o: ruby/wrap.c
	$(CPP) -c $(CFLAGS) -I. $$(pkg-config --cflags $(RUBY)) $? -o $@
ruby/clockkit.so: ruby/wrap.o $(OBJECTS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

tcl/wrap.c:
	$(SWIG) -tcl -o $@ clockkit.h
tcl/wrap.o: tcl/wrap.c
	$(CPP) -c $(CFLAGS) -I. -I/usr/include/tcl8.6 $? -o $@
tcl/clockkit.so: tcl/wrap.o $(OBJECTS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

clean:
	rm -rf $(EXES) $(SWIGEXES) $(OBJECTS) */wrap.o */wrap.c python/clockkit.py

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCLUDE) $< 

.PHONY: all bindings clean test-all test-bindings test-python test-ruby test-tcl
