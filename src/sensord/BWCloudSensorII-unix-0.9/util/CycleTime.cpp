/*
    Copyright (c) 2007,2008 MyTelescope.com
    All rights reserved.

    This file is part of the BWCloudSensorII-unix package.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this package; if not, write to the
    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA  02111-1307  USA.
 */
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "util/CycleTime.h"


double CycleTime::procSpeed = CycleTime::calcProcessorSpeed();

static int
deltaCmp(const void *a, const void *b)
{
    CycleTime::Cycles diff;
    diff = *(const CycleTime::Cycles *) a - *(const CycleTime::Cycles *) b;
    return diff < 0 ? -1 : (diff != 0);
}

double
CycleTime::calcProcessorSpeed()
{
    // std::cout << "calibrating timer..." << flush;

    /* Ask the system if possible (avoids a 1..2 second delay on startup) */
    /* Easier in stdio... */
    FILE *fp;
    /* Disabled as doesn't work on AMD Phenom quad code CPUs (cpuinfo
     * says 800.00 Mhz per CPU, but counts at about 4 times that).
     */
    if (false && (fp = fopen("/proc/cpuinfo", "r")) != (FILE *) 0) {
	char buf[1024];
	double mhz;
	bool found = false;

	while (fgets(buf, sizeof(buf), fp))
	    if (sscanf(buf, "cpu MHz : %lf", &mhz) == 1) {
		found = true;
		break;
	    }
	fclose(fp);
	if (found) {
	    // std::cout << " CPU speed: " << mhz << "Mhz (/proc)" << std::endl;
	    return mhz * 1000 * 1000;
	}
    }

    const int nSamples = 2;	// do not change
    const double delay = .05 * 8;
    Cycles sample[nSamples];
    for (int i = 0; i < nSamples; i++) {
	/* 2010-02: seems n=1 and long delay is better
	 * that many short delays...
	 */
	const int nSleeps = 1;
	const int nThrow = 0;	// throw lowest N, highest N
	CycleTime t0, t1;
	CycleTime::Cycles deltas[nSleeps];
	for (int j = 0; j < nSleeps; j++) {
	    t0.mark();
	    usleep((unsigned long) (delay * 1000000 * (i + 1)));
	    t1.mark();
	    deltas[j] = t1.getElapsedCycles(t0);
	}
	qsort(deltas, nSleeps, sizeof(deltas[0]), deltaCmp);
	Cycles totalCycles = 0;
	for (int j = nThrow; j < nSleeps - nThrow; j++)
	    totalCycles += deltas[j];
	sample[i] = totalCycles / (nSleeps - nThrow * 2);
    }
    /* Knows nSamples == 2 */
    double speed = (sample[1] - sample[0]) / delay;

    // std::cout << " CPU speed: " << (speed / (1000*1000)) << "Mhz" << std::endl;
    return speed;
}
