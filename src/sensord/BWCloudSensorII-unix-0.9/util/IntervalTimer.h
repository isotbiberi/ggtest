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
#ifndef UTIL_INTERVAL_TIMER_H
# define UTIL_INTERVAL_TIMER_H

/*
 * Class that is used to tell how long something took, eg, how long
 * to service a request (request received 'til response sent).
 *
 * Separate class to hide (optional) dependancy on CycleTimer.
 */

template <class IT>
class IntervalTimerTMP {
  public:
    /* Start timer - restarts if already started. */
    void start() { it.start(); }

    /* Does nothing if not started. */
    void stop() { it.stop(); }

    void clear() { it.clear(); }

    /* Ask if between start()/stop() calls. */
    bool isRunning() const { return it.isRunning(); }

    /* Ask if between stop()/start() calls (start() must have been called). */
    bool isStopped() const { return it.isStopped(); }

    /* Pretend the timer was started nSecs later than it was.
     * nSecs may be negative.
     * No effect if timer was not started.
     */
    void moveForwardSeconds(double nSecs) {
	it.moveForwardSeconds(nSecs);
    }
    void moveBackwardSeconds(double nSecs) {
	moveForwardSeconds(-nSecs);
    }
    /* If never started or just cleared, return 0,
     * if running, get seconds since started,
     * if stopped, get stop - start.
     */
    double getSeconds() const { return it.getSeconds(); }

  private:
    IT it;
};

/* Implementation options. */
# define INTERVAL_TIMER_GETTIMEOFDAY	1
# define INTERVAL_TIMER_CYCLETIMER	2

/* Default option. */
# ifndef INTERVAL_TIMER_IMPLEMENTATION
#  define INTERVAL_TIMER_IMPLEMENTATION INTERVAL_TIMER_CYCLETIMER
# endif

/* Always include gettimeofday implementation as code that has
 * long delays are better off using this (more accurate).
 */
# if 1 || INTERVAL_TIMER_IMPLEMENTATION == INTERVAL_TIMER_GETTIMEOFDAY
#  define INTERVAL_TIMER_DEFINE_GETTIMEOFDAY
# endif
# if INTERVAL_TIMER_IMPLEMENTATION == INTERVAL_TIMER_CYCLETIMER
#  define INTERVAL_TIMER_DEFINE_CYCLETIMER
# endif

# ifdef INTERVAL_TIMER_DEFINE_GETTIMEOFDAY
#  include <sys/time.h>

class IntervalTimerGTOD {
  public:
    IntervalTimerGTOD() {
	timerclear(&startTime);
    }
    void start() {
	timerclear(&stopTime);
	gettimeofday(&startTime, NULL);
    }
    void stop() {
	/* If not started, don't stop. */
	if (timerisset(&startTime))
	    gettimeofday(&stopTime, NULL);
    }
    void clear() {
	timerclear(&startTime);
	timerclear(&stopTime);
    }
    bool isRunning() const {
	return timerisset(&startTime) && !timerisset(&stopTime);
    }
    bool isStopped() const {
	return timerisset(&startTime) && timerisset(&stopTime);
    }
    void moveForwardSeconds(double nSecs) {
	 if (timerisset(&startTime)) {
	    /* Note: nSecs may be negative. */
	    startTime.tv_sec += (time_t) nSecs;
	    nSecs -= (time_t) nSecs;
	    startTime.tv_usec += (time_t) (nSecs * 1000000);
	    if (startTime.tv_usec < 0) {
		--startTime.tv_sec;
		startTime.tv_usec += 1000000;
	    } else if (startTime.tv_usec > 1000000) {
		++startTime.tv_sec;
		startTime.tv_usec -= 1000000;
	    }
	 }
    }
    double getSeconds() const {
	if (!timerisset(&startTime))
	    return 0;
	struct timeval t;
	const struct timeval *tv = &stopTime;
	if (!timerisset(&stopTime)) {
	    gettimeofday(&t, NULL);
	    tv = &t;
	}
	timersub(tv, &startTime, &t);
	return t.tv_sec + t.tv_usec / 1000000.0;
    }

  private:
    struct timeval startTime;
    struct timeval stopTime;
};

#endif /* INTERVAL_TIMER_DEFINE_GETTIMEOFDAY */


# ifdef INTERVAL_TIMER_DEFINE_CYCLETIMER
#  include "util/CycleTime.h"

class IntervalTimerCT {
  public:
    IntervalTimerCT() {
	startTime.clear();
    }
    void start() {
	stopTime.clear();
	startTime.mark();
    }
    void stop() {
	/* If not started, don't stop. */
	if (startTime.getCycles() != 0)
	    stopTime.mark();
    }
    void clear() {
	startTime.clear();
	stopTime.clear();
    }
    bool isRunning() const {
	return startTime.getCycles() != 0 && stopTime.getCycles() == 0;
    }
    bool isStopped() const {
	return startTime.getCycles() != 0 && stopTime.getCycles() != 0;
    }
    void moveForwardSeconds(double nSecs) {
	 if (startTime.getCycles() != 0) {
	    startTime.moveForwardSeconds(nSecs);
	 }
    }
    double getSeconds() const {
	if (startTime.getCycles() == 0)
	    return 0;
	if (stopTime.getCycles() == 0) {
	    CycleTime t;
	    t.mark();
	    return t.getElapsedSeconds(startTime);
	}
	return stopTime.getElapsedSeconds(startTime);
    }

  private:
    CycleTime startTime;
    CycleTime stopTime;
};

#endif /* INTERVAL_TIMER_DEFINE_CYCLETIMER */


# if INTERVAL_TIMER_IMPLEMENTATION == INTERVAL_TIMER_GETTIMEOFDAY
typedef IntervalTimerTMP<IntervalTimerGTOD> IntervalTimer;
# else /* INTERVAL_TIMER_IMPLEMENTATION */
typedef IntervalTimerTMP<IntervalTimerCT> IntervalTimer;
# endif /* INTERVAL_TIMER_IMPLEMENTATION */

/* Use this when accuracy is more important than speed. */
typedef IntervalTimerTMP<IntervalTimerGTOD> IntervalTimerAccurate;

#endif /* UTIL_INTERVAL_TIMER_H */
