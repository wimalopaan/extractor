# -*- mode: makefile-gmake; -*-

CXXFLAGS += -std=c++17
CXXFLAGS += -Wall -Wextra
LDLIBS += -lstdc++ -lm -ldl -lpthread

all: test

test: test.o processenvironment.o commandlineoptionspecifier.o commandlineparser.o commandlineoption.o

ccsources = $(wildcard *.cc)
headers   = $(wildcard *.h)
ccdeps    = $(patsubst %.cc, %.d, $(ccsources))
objects   = $(patsubst %.cc, %.o, $(ccsources))

%.d: %.cc
	$(CXX) -MM -MG $(CXXFLAGS) $< | sed 's/$*\.o/& $@/g' > $@

clean:
	$(RM) $(targets) $(targetsmemcheck)
	$(RM) *~
	$(RM) a.out
	$(RM) a.exe
	$(RM) $(ccdeps)
	$(RM) $(objects)
	$(RM) *.class

-include $(ccdeps)
