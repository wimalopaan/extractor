# -*- mode: makefile-gmake; -*-

all:
	make -C libwm all
	make -C extract all
	make -C doc all

testing:
	make -C test all

clean:
	make -C libwm clean
	make -C extract clean
	make -C doc clean
