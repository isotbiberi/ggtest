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
#ifndef UTIL_CYCLE_TIME_H
# define UTIL_CYCLE_TIME_H

class CycleTime {
public:
    typedef long long Cycles;

private:
    static __inline__ Cycles rdtsc() {
	Cycles x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	return x;
    }
    // Make non-private if needed
    static Cycles cycles() {
	return rdtsc();
    }
    Cycles getElapsedCycles(const CycleTime &ct) const {
	return nCycles - ct.nCycles;
    }

public:
    CycleTime()
	: nCycles(0)
    {
    }
    // Note the current time
    void mark() {
	nCycles = cycles();
    }
    /*
     * Figure out differece between this time and a previous time:
     *	CycleTime t0, t1;
     *	t0.mark();
     *	...
     *	t1.mark();
     *	double nSeconds = t1.getElapsedSeconds(t0);
     */
    double getElapsedSeconds(const CycleTime &ct) const {
	return (double) getElapsedCycles(ct) / procSpeed;
    }
    /* Pretend the timer was started nSecs later than it was. */
    void moveForwardSeconds(double nSecs) {
	nCycles += (Cycles) (nSecs * procSpeed);
    }
    void clear() {
	nCycles = 0;
    }

    int compare(const CycleTime &ct) const {
	Cycles cmp = nCycles - ct.nCycles;
	return cmp < 0 ? -1 : (cmp != 0);
    }

    Cycles getCycles() const {
	return nCycles;
    }

private:
    static double calcProcessorSpeed();

    Cycles nCycles;
    static double procSpeed;
};

class CycleTimer {
public:
    void start() {
	t0.mark();
    }
    void stop() {
	t1.mark();
    }
    void clear() {
	t0.clear();
	t1.clear();
    }
    /* Shouldn't be needed
    CycleTime::Cycles getElapsedCycles() const {
	return t1.getElapsedCycles(t0);
    }
    */
    double getElapsedSeconds() const {
	/* Danger: assumes stop() has been called. */
	return t1.getElapsedSeconds(t0);
    }
    double getRunningSeconds() const {
	CycleTime running;
	running.mark();
	return running.getElapsedSeconds(t0);
    }

private:
    CycleTime t0, t1;
};

#endif /* UTIL_CYCLE_TIME_H */
