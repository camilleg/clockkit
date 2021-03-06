MAKEFLAGS += --jobs=99
CPP = c++
# -fPIC is only to make $(SWIGEXES).
# -fpic would be smaller and faster, but that is only for gcc.
CFLAGS = -O3 -fPIC -std=c++14 -Wall -Werror -Weffc++
LIBS = -lccgnu2 -lpthread -ldl
SWIG = swig -Wallkw -Werror
RUBY   = $$(pkg-config --list-all | grep ruby-   | sort | tail -1 | sed 's/ .*//')
PYTHON = $$(pkg-config --list-all | grep python- | sort | tail -1 | sed 's/ .*//')
# pkg-config doesn't know about tcl.

OBJS = \
  ClockClient.o \
  ClockPacket.o \
  ClockServer.o \
  ConfigReader.o \
  PhaseLockedClock.o \
  SystemClock.o \
  Timestamp.o \
  VariableFrequencyClock.o \
  clockkit.o

EXES = \
  ckserver \
  ckphaselock \
  cktest-standalone \

SWIGEXES = \
  python/_clockkit.so \
  ruby/clockkit.so \
  tcl/clockkit.so

all: CFLAGS += -pedantic
all: $(EXES)

bindings: all $(SWIGEXES)

debug: override CFLAGS += -O0 -ggdb
debug: all

profile: CFLAGS += --coverage -fprofile-arcs -ftest-coverage -O0
profile: $(EXES)

# .dependencies/*.d are built by the %.o:%.cpp rule, via -MMD -MT -MF.
# They define the .h dependencies of each .o file.
DEPS := $(patsubst %.o,.dependencies/%.d,$(OBJS)) # $(OBJS:.o=.d)
-include $(DEPS)

test-bindings: test-python test-python2 test-ruby test-ruby2 test-tcl test-tcl2

# Different ports let these tests run simultaneously, via --jobs=99.
test: ckserver ckphaselock
	./testcase.sh 4440
test-standalone: cktest-standalone
	./$? 4480 5 3 > /dev/null
test-30: ckserver ckphaselock
	./testcase-n.sh 30 4430

test-python: ckserver python/_clockkit.so
	cd python && ./testcase.sh 4450
test-python2: ckserver python/_clockkit.so
	cd python && ./testcase-pkill.sh 4452
test-ruby: ckserver ruby/clockkit.so
	cd ruby && ./testcase.sh 4460
test-ruby2: ckserver ruby/clockkit.so
	cd ruby && ./testcase-pkill.sh 4462
test-tcl: ckserver tcl/clockkit.so
	cd tcl && ./testcase.sh 4470
test-tcl2: ckserver tcl/clockkit.so
	cd tcl && ./testcase-pkill.sh 4472

test-remote: ckphaselock
	./test2hosts.sh config-remote.sh
test-remote2: ckphaselock
	./test2hosts.sh config-remote2.sh
# Don't generalize this here.  Instead, manually ./test2hosts.sh config-myOtherHost.sh.

cktest-standalone: test-standalone.cpp $(OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

ckserver: ClockServerMain.cpp $(OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

ckphaselock: PhaseLockMain.cpp $(OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

python/wrap.c: clockkit.h
	$(SWIG) -python -o $@ clockkit.h
python/wrap.o: python/wrap.c
	$(CPP) -c $(CFLAGS) -I. $$(pkg-config --cflags $(PYTHON)) $? -o $@
python/_clockkit.so: python/wrap.o $(OBJS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

tcl/wrap.c: clockkit.h
	$(SWIG) -tcl -o $@ clockkit.h
tcl/wrap.o: tcl/wrap.c
	$(CPP) -c $(CFLAGS) -I. -I/usr/include/tcl8.6 $? -o $@
tcl/clockkit.so: tcl/wrap.o $(OBJS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

# The sed casts _wrap_ck's to (VALUE (*)(...)) in lines starting with rb_define_module_function.
ruby/wrap.c: clockkit.h
	$(SWIG) -ruby -o $@ clockkit.h
	sed -i -e "s/ _wrap_ck/ (VALUE (*)(...))_wrap_ck/" $@
ruby/wrap.o: ruby/wrap.c
	$(CPP) -c $(CFLAGS) -Wno-pedantic -I. $$(pkg-config --cflags $(RUBY)) $? -o $@
ruby/clockkit.so: ruby/wrap.o $(OBJS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

clean:
	rm -rf $(EXES) $(SWIGEXES) $(OBJS) $(DEPS) */wrap.o */wrap.c python/clockkit.py *.gcno *.gcov

purge: clean
	rm -rf *.gcda

%.o: %.cpp
	$(CPP) -c $(CFLAGS) -MMD -MT $@ -MF $(patsubst %.o,.dependencies/%.d,$@) $<

.PHONY: all profile bindings purge clean test-all test-30 test-standalone test-bindings test-python test-ruby test-tcl
