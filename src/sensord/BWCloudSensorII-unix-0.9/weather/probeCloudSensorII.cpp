/*
    Copyright (c) 2007,2008 MyTelescope.com
    All rights reserved.

    This file is part of the BWCloudSensorII-unix package.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this package; if not, write to the
    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA  02111-1307  USA.
 */
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string>

#include "weather/BWCloudSensorII.h"

#define DEFAULT_POLL_REQUEST_SECS 2
#define DEFAULT_TIMEOUT_SECS (DEFAULT_POLL_REQUEST_SECS * 3 + 1)


const char *progName = "probeCloudSensorII";

static void usage();

class ProbeListener : public BWCSII_Listener
{
  private:
    bool verbose;
    unsigned int timeoutSecs;
    unsigned int pollRequestSecs;
    bool success;
    struct timeval stopTime;
    struct timeval nextPollTime;
  public:
    ProbeListener(bool verbose, unsigned int timeoutSecs)
	: verbose(verbose), timeoutSecs(timeoutSecs),
          pollRequestSecs(DEFAULT_POLL_REQUEST_SECS), success(false)
    {
	timerclear(&stopTime);
    }
    bool succeeded() const {
	return success;
    }
    /*
     * BWCSII_Listener interface.
     */
    void csII_readLooping(BWCloudSensorII *csII)
    {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	if (stopTime.tv_sec == 0) {
	    stopTime = tv;
	    stopTime.tv_sec += timeoutSecs;
	    nextPollTime = tv;
	    nextPollTime.tv_sec += pollRequestSecs;
	} else if (timercmp(&tv, &stopTime, >=)) {
	    if (verbose)
		std::cerr << progName << ": timed out waiting for response"
		    << std::endl;
	    csII->readStop();
	} else if (timercmp(&tv, &nextPollTime, >=)) {
	    if (verbose)
		std::cerr << progName << ": requesting another poll"
		    << std::endl;
	    nextPollTime = tv;
	    nextPollTime.tv_sec += pollRequestSecs;
	    try {
		/* This done so as not to rely on a single poll character
		 * getting through, and we don't have to wait for the longer
		 * poll time out that is in the driver.
		 */
		csII->requestPolls();
	    } catch (const SerialConnectionException &ex) {
		std::cerr << progName << ": error requesting poll: "
		    << ex.getMessage() << std::endl;
	    }
	}
    }
    void csII_reportReset(BWCloudSensorII *csII,
	    const BWCSII_reportReset &report)
    {
	receivedReport(csII, &report);
    }
    void csII_reportThresholds(BWCloudSensorII *csII,
	const BWCSII_reportThresholds &report)
    {
	receivedReport(csII, &report);
    }
    void csII_reportWetnessCalibration(BWCloudSensorII *csII,
	const BWCSII_reportWetnessCalibration &report)
    {
	receivedReport(csII, &report);
    }
    void csII_reportWetnessData(BWCloudSensorII *csII,
	const BWCSII_reportWetnessData &report)
    {
	receivedReport(csII, &report);
    }
    void csII_reportThermopileCalibration(BWCloudSensorII *csII,
	const BWCSII_reportThermopileCalibration &report)
    {
	receivedReport(csII, &report);
    }
    void csII_reportSensorData(BWCloudSensorII *csII,
	    const BWCSII_reportSensorData &report)
    {
	receivedReport(csII, &report);
    }
    void receivedReport(BWCloudSensorII *csII, const BWCSII_report *report)
    {
	success = true;
	if (verbose)
	    std::cerr << progName << ": received "
		<< BWCSII_report::getReportTypeName(report->reportType)
		<< "report!" << std::endl;
	csII->readStop();
    }
};

int
main(int argc, char *argv[])
{
    const char *dev;
    bool verbose = false;
    int opt;
    unsigned int timeoutSecs = DEFAULT_TIMEOUT_SECS;
    const char *logDirectory = NULL;
    BWCloudSensorII::DriverFlags driverFlags
	= BWCloudSensorII::DF__UNINITIALIZED;

    while ((opt = getopt(argc, argv, "d:o:t:v")) != EOF) {
	switch (opt) {
	  case 'd':
	    logDirectory = optarg;
	    break;
	  case 't': {
	    char *end;
	    errno = 0;
	    long val = strtol(optarg, &end, 10);
	    if (errno != 0 || *end || val <= 0) {
		std::cerr << progName << ": invalid -t (timeout) option: "
		    << optarg << std::endl;
		exit(1);
	    }
	    timeoutSecs = val;
	    break;
	  }
	  case 'o':
	    if (strcmp(optarg, "serialLog") == 0) {
		driverFlags = BWCloudSensorII::DriverFlags(
		    driverFlags | BWCloudSensorII::DF_SERIAL_LOG);
	    } else {
		std::cerr << progName << ": unknown -o option: "
		    << optarg << std::endl;
		exit(1);
	    }
	    break;

	  case 'v':
	    verbose = true;
	    break;

	  default:
	    usage();
	}
    }
    if (argc == optind) {
	std::cerr << progName << ": no tty to probe given." << std::endl;
	usage();
    }
    dev = argv[optind++];
    if (argc != optind) {
	std::cerr << progName << ": too many arguments on command line."
	    << std::endl;
	usage();
    }

    if (logDirectory == NULL) {
	std::cerr << progName << ": no log directory specified."
	    << std::endl;
	usage();
    } else {
	struct stat statb;
	if (stat(logDirectory, &statb) < 0) {
	    std::cerr << progName << ": error accessing log directory: "
		<< logDirectory << ": " << strerror(errno) << std::endl;
	    exit(1);
	}
	if (!S_ISDIR(statb.st_mode)) {
	    std::cerr << progName
		<< ": invalid log directory (not a directory): "
		<< logDirectory << std::endl;
	    exit(1);
	}
    }

    if (!verbose)
	/* Effectivly turns off error reporting. */
	logmsg.setDebuggingState("logTo(type=stderr,priority=emerg)");
    logmsg.setProgName(progName); /* after turning off reporting... */

    const char *what = "?";
    ProbeListener listener(verbose, timeoutSecs);
    try {
	what = "creating BWCloudSensorII instance";
	BWCloudSensorII csII(&listener, progName, logDirectory, driverFlags);
	what = "opening csII";
	csII.open(dev);
	what = "reading";
	csII.readLoop();
	what = "closing csII";
	csII.close();
	what = "destroying BWCloudSensorII instance";
    } catch (const GenericException &e) {
	if (verbose)
	    std::cerr << progName << ": exception " << what
		<< ": " << e.getMessage() << std::endl;
	return 2;
    }
    if (verbose && listener.succeeded())
	std::cerr << progName << ": probe succeeded on " << dev << std::endl;

    return listener.succeeded() ? 0 : 1;
}

static void
usage()
{
    cerr << "Usage: " << progName << " [options] -d ld ttyToProbe\n";
    cerr <<
"    Options:\n\
	-d ld	Set the log directory to ld.\n\
	-t secs	Set the timeout, in seconds (default is "
	    << DEFAULT_TIMEOUT_SECS << ")\n\
	-o opt	Set option opt:\n\
		  serialLog	Enable logging of characters sent/received.\n\
	-v	verbose mode.\n\
    Test if a CloudSensorII device can be found on the given tty (/dev/...).\n\
    Exit code of 0 indicates probe succeeded, 1 means it failed, anything\n\
    else indicates something that shouldn't happen did.\n\
";
    exit(2);
}
