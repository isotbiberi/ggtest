#
#  Copyright (c) 2007,2008 MyTelescope.com
#  All rights reserved.
#  This file is part of the BWCloudSensorII-unix package.
#

CXX=g++
CXXFLAGS=-g -Wconversion -Wall -fexceptions -O
CPPFLAGS=
CC=gcc
CFLAGS=-g -Wall -O

INSTALL=install

MAKE_SUBDIRS=\
	for i in dummy $(SUBDIRS); do \
	    [ X"$$i" = Xdummy ] && continue; \
	    echo "+++ $P$$i:"; \
	    if [ -d $$i ]; then \
		(cd $$i && $(MAKE) --no-print-directory $(MFLAGS) P="$P$$i/" $@) || case '$(MFLAGS)' in (-[ki]*|*' '-[ki]*) ;; (*) exit 1;; esac \
	    else \
		echo "    (skipping $P$$i - directory does not exist)"; \
	    fi \
	done; echo "+++ $P:"
