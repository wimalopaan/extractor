# -*- mode: makefile-gmake; -*-

#targets = extractor test desertest axtractor
targets = extractor axtractor avrasmopt

#CXX = clang++
#CXX = /usr/local/bin/g++
CXX = /usr/bin/g++

export LC_ALL=C

ifeq (${CXX}, clang++)
 CXXFLAGS += -g -Wall -Wextra -std=c++17 -Wconversion -pedantic # -fsanitize=undefined
 LDFLAGS += -fsanitize=undefined
else
# CXXFLAGS += -g -Wall -Wextra -Wconversion -pedantic -std=c++17 -fdiagnostics-color
 #CXXFLAGS += -g -Wall -Wextra -Wconversion -fconcepts -std=c++17 
 CXXFLAGS += -g -Wall -Wextra -Wconversion -std=c++20 -fPIC
endif

ifdef NDEBUG
 CPPFLAGS += -DNDEBUG
endif

CPPFLAGS += -I../libwm
CPPFLAGS += -I./include
CPPFLAGS += -I./lyra

LDLIBS += -lm -lstdc++ -lstdc++fs
#LDLIBS += -lboost_system # -lboost_filesystem  -lboost_program_options
LDLIBS += -lastyle
LDLIBS += ../libwm/libwm.a
LDLIBS += -lmagic

all: $(targets) ${subdirs}

# unechte Ziele (dann kein Dateikonflikt)
.PHONY: all clean subclean subdirs ${subdirs}

subdirs: ${subdirs}

${subdirs}:
	@if [ -d $@ ]; then ${MAKE} -C $@; fi

# wirldcards werden direkt nur als Vorbedingungen ausgewertet, daher hier explizit Funktionsaufruf
ccsources = $(wildcard *.cc)
headers   = $(wildcard *.h)
# patsubst: pattern-substitution, % ist bel. string
ccdeps    = $(patsubst %.cc, %.d, $(ccsources))
objects   = $(patsubst %.cc, %.o, $(ccsources))
eo = $(filter-out test.o desertest.o axtractor.o avrasmopt.o, $(objects))

extractor: $(eo)

test: test.o

desertest: desertest.o cppextractor.o

axtractor: axtractor.o asmextractor.o asciidocasmgenerator.o commandlineoption.o commandlineparser.o

avrasmopt: avrasmopt.o commandlineoption.o commandlineparser.o

# Erzeugt die dependency-includes (sed-Kommando um die %.d-Datei ebenfalls als Ziel hinzuzufuegen)
# z.B.: hello.o hello.d: hello.cc hello.h
%.d: %.cc
	$(CXX) -MM -MG $(CPPFLAGS) $(CXXFLAGS) $< | sed 's/$*\.o/& $@/g' > $@

%.o: %.cc
	 $(COMPILE.cc) $(STDFLAGS) $(OUTPUT_OPTION) $(shell pwd)/$<

%.memcheck: %
	valgrind --tool=memcheck --num-callers=20 --leak-check=yes --leak-resolution=high ./$< > $@ 2>&1

subclean:
	@for d in ${subdirs}; do \
		if [ -d $$d ]; then \
			${MAKE} -C $$d clean; \
		fi \
	done

clean: subclean
	$(RM) $(targets) $(targetsmemcheck)
	$(RM) *~
	$(RM) a.out
	$(RM) a.exe
	$(RM) $(ccdeps)
	$(RM) $(objects)
	$(RM) *.class

-include $(ccdeps)
