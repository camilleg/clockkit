MAKEFLAGS += --jobs=99
#CPP = c++
CPP = x86_64-none-linux-musl-c++ -L"${HOME}/usr/lib" -I"${HOME}/usr/include" -idirafter"/usr/include" -idirafter.
# -fPIC is for */*.so.
CFLAGS = -O3 -fPIC -std=c++11 -Wall -Werror -static -pie
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

profile: CFLAGS += -fPIC --coverage -fprofile-arcs -ftest-coverage -O0
profile: $(EXES)

# The *.d files are built by the rule for %.o:%.cpp via -MMD -MT -MF.
# They define the .h dependencies of each .o file.
DEPS := $(OBJS:.o=.d)
-include $(DEPS)

test-bindings: test-python test-ruby test-tcl

# Different ports let these tests run simultaneously, via --jobs=99.
test: ckserver ckphaselock
	./testcase.sh 4440
test-30: ckserver ckphaselock
	./testcase-n.sh 30 4430
test-standalone: cktest-standalone
	./$? 4480 5 3 > /dev/null
test-remote: ckphaselock
	./test2hosts.sh
test-python: ckserver python/_clockkit.so
	cd python && ./testcase.sh 4450
test-ruby: ckserver ruby/clockkit.so
	cd ruby && ./testcase.sh 4460
test-tcl: ckserver tcl/clockkit.so
	cd tcl && ./testcase.sh 4470

cktest-standalone: test-standalone.cpp $(OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

ckserver: ClockServerMain.cpp $(OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

ckphaselock: PhaseLockMain.cpp $(OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) -o $@

python/wrap.c:
	$(SWIG) -python -o $@ clockkit.h
python/wrap.o: python/wrap.c
	$(CPP) -c $(CFLAGS) -I. $$(pkg-config --cflags $(PYTHON)) $? -o $@
python/_clockkit.so: python/wrap.o $(OBJS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

tcl/wrap.c:
	$(SWIG) -tcl -o $@ clockkit.h
tcl/wrap.o: tcl/wrap.c
	$(CPP) -c $(CFLAGS) -I. -Idirafter"/usr/include/tcl8.6" $? -o $@
tcl/clockkit.so: tcl/wrap.o $(OBJS)
	$(CPP) $(CFLAGS) -shared $? $(LIBS) -o $@

ruby/wrap.c:
	$(SWIG) -ruby -o $@ clockkit.h
	# In lines starting with rb_define_module_function, cast _wrap_ck's to (VALUE (*)(...)).
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
	$(CPP) -c $(CFLAGS) -MMD -MT $@ -MF $(patsubst %.o,%.d,$@) $<

.PHONY: all profile bindings purge clean test-all test-30 test-standalone test-bindings test-python test-ruby test-tcl
