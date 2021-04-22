# -*- mode: makefile-gmake; -*-

targets = libwm.so libwm.a libwmtests

#CXX = clang++
CXX = /usr/bin/g++

export LC_ALL=C

CXXFLAGS += -fPIC

ifeq (${CXX}, clang++)
 CXXFLAGS += -g -Wall -Wextra -std=c++14 -Wconversion -pedantic # -fsanitize=undefined
 LDFLAGS += -fsanitize=undefined
else
 CXXFLAGS += -g -Wall -Wextra -Wconversion -pedantic -std=c++2a -fdiagnostics-color -fPIC
endif

#CXXFLAGS += $(shell llvm-config --cxxflags) -fexceptions

ifdef NDEBUG
 CPPFLAGS += -DNDEBUG
endif

#LDLIBS += -lclangFrontendTool -lclangFrontend -lclangDriver -lclangSerialization -lclangCodeGen -lclangParse -lclangSema \
#-lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangAnalysis -lclangIndex -lclangARCMigrate \
#-lclangRewriteCore -lclangRewriteFrontend -lclangAST -lclangLex -lclangBasic -lclangTooling

#LDLIBS +=  -lclangFrontend -lclangSerialization -lclangDriver -lclangParse -lclangSema -lclangStaticAnalyzerFrontend \
#           -lclangAnalysis -lclangRewriteFrontend -lclangEdit -lclangAST -lclangLex -lclangBasic -lclangASTMatchers -lclangDriver -lclangTooling

#LDLIBS += $(shell llvm-config  --libs)

#LDLIBS += -lwm
LDFLAGS += -Wl,-rpath,$(shell pwd) -L$(shell pwd)

LDLIBS += -lstdc++ -lm -ldl -lpthread -lmagic #-lcurses
LDLIBS += -lstdc++fs
#LDLIBS += -lboost_filesystem -lboost_system -lboost_regex
#LDLIBS += -lLLVM-3.4
#LDLIBS += -lclang

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

libwm.so: $(objects)
	$(LINK.cc) $(CXXFLAGS) $(LDFLAGS) -shared $(objects) -o $@

libwm.a: $(objects)
	$(AR) cqs $@ $+

libwmtests: $(objects)


# Erzeugt die dependency-includes (sed-Kommando um die %.d-Datei ebenfalls als Ziel hinzuzufuegen)
# z.B.: hello.o hello.d: hello.cc hello.h
%.d: %.cc
	$(CXX) -MM -MG $(CXXFLAGS) $< | sed 's/$*\.o/& $@/g' > $@

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
