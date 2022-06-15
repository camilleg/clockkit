MAKEFLAGS += --jobs=99
CPP = c++

# -fPIC is only to make $(SWIGEXES).
# -fpic would be smaller and faster, but that is only for gcc.
CFLAGS := -O3 -fPIC -Wall -Werror -Weffc++ -std=c++17 -pedantic
LIBS := -lpthread -ldl

# Optional file containing debugging options for CFLAGS and LDFLAGS.
-include Rules.debug

SWIG = swig -Wallkw -Werror
RUBY   := $(shell pkg-config --list-all | grep ruby-   | sort | tail -1 | sed 's/ .*//')
PYTHON := $(shell pkg-config --list-all | grep python- | sort | tail -1 | sed 's/ .*//')
# pkg-config doesn't know about tcl.

# Output of swig isn't effc++ compliant, starting with Ruby 3.
ifeq "$(RUBY)" "ruby-3.0"
	CFLAGS_SPECIAL := -Wno-effc++
endif

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

OBJS_EXE = \
  ClockServerMain.o \
  kissnet-udp.o \
  PhaseLockMain.o \
  test-unit.o

OBJS_ALL = $(sort $(OBJS) $(OBJS_EXE))

EXES = \
  ckserver \
  ckphaselock \
  cktest-standalone \
  cktest-unit \

SWIGEXES = \
  python/_clockkit.so \
  ruby/clockkit.so \
  tcl/clockkit.so

all: $(EXES)

bindings: $(SWIGEXES)

debug: override CFLAGS += -O0 -ggdb
debug: all

profile: CFLAGS += --coverage -fprofile-arcs -ftest-coverage -O0
profile: $(EXES)

test-bindings: ckserver test-python test-python2 test-ruby test-ruby2 test-tcl test-tcl2

# Different ports let these tests run simultaneously, via --jobs=99.
test: ckserver ckphaselock
	./testcase.sh 4440
test-standalone: cktest-standalone
	./$? 4480 500 3 > /dev/null
test-unit: cktest-unit
	./$?
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

cktest-unit: test-unit.o $(OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

ckserver: ClockServerMain.o $(OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

ckphaselock: PhaseLockMain.o $(OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

python/wrap.cpp: clockkit.h
	$(SWIG) -python -o $@ clockkit.h
python/wrap.o: python/wrap.cpp
	$(CPP) -c $(CFLAGS) -I. $$(pkg-config --cflags $(PYTHON)) $? -o $@
python/_clockkit.so: python/wrap.o $(OBJS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

tcl/wrap.cpp: clockkit.h
	$(SWIG) -tcl -o $@ clockkit.h
tcl/wrap.o: tcl/wrap.cpp
	$(CPP) -c $(CFLAGS) -I. -I/usr/include/tcl8.6 $? -o $@
tcl/clockkit.so: tcl/wrap.o $(OBJS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

# The sed casts _wrap_ck's to (VALUE (*)(...)) in lines starting with rb_define_module_function.
# -Wno-register is for Ubuntu 18, whose Ruby 2.5's .h files use C++17-illegal "register".
ruby/wrap.cpp: clockkit.h
	$(SWIG) -ruby -o $@ clockkit.h
	sed -i -e "s/ _wrap_ck/ (VALUE (*)(...))_wrap_ck/" $@
ruby/wrap.o: ruby/wrap.cpp
	$(CPP) -c $(CFLAGS) $(CFLAGS_SPECIAL) -Wno-pedantic -Wno-register -I. $$(pkg-config --cflags $(RUBY)) $? -o $@
ruby/clockkit.so: ruby/wrap.o $(OBJS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

clean:
	rm -rf $(EXES) $(SWIGEXES) $(OBJS) $(OBJS_EXE) .depend */wrap.o */wrap.cpp python/clockkit.py *.gcno *.gcov

purge: clean
	rm -rf *.gcda

DEPENDFLAGS = -MMD -MT $@ -MF $(patsubst %.o,.depend/%.d,$@)
%.o: %.cpp
	@mkdir -p .depend
	$(CPP) -c $(CFLAGS) $(DEPENDFLAGS) $<

# .depend/*.d is built by %.o:%.cpp.
-include $(patsubst %.o,.depend/%.d,$(OBJS_ALL))

.PHONY: \
  all \
  bindings \
  clean \
  debug \
  profile \
  purge \
  test \
  test-30 \
  test-bindings \
  test-python test-python2 \
  test-remote test-remote2 \
  test-ruby test-ruby2 \
  test-standalone \
  test-tcl test-tcl2 \
  test-unit
