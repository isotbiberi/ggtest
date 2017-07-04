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
#include <iostream>
#include <fstream>
#include <iomanip>

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "util/Log.h"
#include "util/toString.h"

#include "weather/BWCloudSensorII.h"

bool reportLogOpen = false;
std::ofstream reportLog;

static bool
ttyNAvailable()
{
    int bytes = 0;
    int ret = ioctl(0, FIONREAD, &bytes);
    if (ret >= 0 && bytes > 0)
	return true;
    return false;
}

bool noKeyboardAborts;

static bool
userKeyboardAbort(char *c)
{
    char buf[1];
    if (c != NULL)
	*c = '\0';
    if (noKeyboardAborts)
	return false;
    if (ttyNAvailable() && read(0, buf, 1) == 1) {
	if (c != NULL)
	    *c = buf[0];
	if ((buf[0] == 'q' || buf[0] == 'Q')) {
	    std::cout << std::endl << "Aborted from keyboard." << std::endl;
	    return true;
	}
	if (buf[0] == '!')
	    noKeyboardAborts = true;
    }
    return false;
}

static void
doSleep(unsigned int nSecs)
{
    std::cout << "sleeping for " << nSecs << " seconds..." << std::flush;
    sleep(nSecs);
    std::cout << "done." << std::endl;
}

static void
doSingleCharCmd(BWCloudSensorII *csII, char cmd)
{
    switch (cmd) {
      case '\n':
      case '\r':
	/* ignore */
	break;

      case 'C':
	std::cout << "Calling checkForData..." << std::flush;
	csII->checkForData();
	std::cout << " done." << std::endl;
	break;

      case 'c':
	std::cout << "Calling requestCalibrationData..." << std::flush;
	csII->requestCalibrationData();
	std::cout << " done." << std::endl;
	break;

      case 'k':
	std::cout << "Calling requestWetnessCalibrationData..." << std::flush;
	csII->requestWetnessCalibrationData();
	std::cout << " done." << std::endl;
	break;

      case 'p':
	std::cout << "Calling requestPolls..." << std::flush;
	csII->requestPolls();
	std::cout << " done." << std::endl;
	break;

      case 'P':
	std::cout << "Calling printState()..." << std::endl;
	csII->printState(std::cout, 4, 4);
	break;

      case 's': {
	const BWCSII_reportReset &rI = csII->getLastReset();
	std::cout << "Reset (I) report: " << std::endl;
	rI.printState(std::cout, 4, 4);

	const BWCSII_reportThresholds &rT
	    = csII->getLastThresholds();
	std::cout << "Thresholds (T) report: " << std::endl;
	rT.printState(std::cout, 4, 4);

	const BWCSII_reportWetnessCalibration &rK
	    = csII->getLastWetnessCalibration();
	std::cout << "Wetness calibration (K) report: "
	    << std::endl;
	rK.printState(std::cout, 4, 4);

	const BWCSII_reportThermopileCalibration &rC
	    = csII->getLastThermopileCalibration();
	std::cout << "Thermopile calibration (C) report: "
	    << std::endl;
	rC.printState(std::cout, 4, 4);

	const BWCSII_reportSensorData &rD
	    = csII->getLastSensorData();
	std::cout << "Sensor data (D) report: "
	    << std::endl;
	rD.printState(std::cout, 4, 4);
	break;
      }

    default:
	std::cout << "unknown command: " << toStringDump(&cmd, 1) << "."
	    << std::endl;
    }
}

class TestListener : public BWCSII_Listener {
  public:
    TestListener() {
    }

    virtual void csII_purgeEvent(BWCloudSensorII *csII,
	    unsigned char reasonChar)
    {
	std::cout << "csII_purgeEvent: reasonChar=" << reasonChar << std::endl;
    }
    virtual void csII_memoryDump(BWCloudSensorII *csII, const uint8_t *memory,
	    unsigned int nBytes)
    {
	std::cout << "csII_memoryDump: nBytes=" << nBytes << std::endl;
    }

    virtual void csII_cmdOver(BWCloudSensorII *csII, CmdType cmdType,
	    CmdOverReason reason)
    {
	std::cout << "csII_cmdOver: cmdType=" << getCmdTypeName(cmdType)
	    << ", reason=" << getCmdOverReasonName(reason) << std::endl;
    }

    virtual void csII_reportReset(BWCloudSensorII *csII,
	    const BWCSII_reportReset &report)
    {
	std::cout << "csII_reportReset:" << std::endl;
	report.printState(std::cout, 8, 4);
	if (reportLogOpen) {
	    report.printState(reportLog, 8, 4);
	    reportLog.flush();
	}
    }
    virtual void csII_reportThresholds(BWCloudSensorII *csII,
        const BWCSII_reportThresholds &report)
    {
	std::cout << "csII_reportThresholds:" << std::endl;
	report.printState(std::cout, 8, 4);
	if (reportLogOpen) {
	    report.printState(reportLog, 8, 4);
	    reportLog.flush();
	}
    }
    virtual void csII_reportWetnessCalibration(BWCloudSensorII *csII,
        const BWCSII_reportWetnessCalibration &report)
    {
	std::cout << "csII_reportWetnessCalibration:" << std::endl;
	report.printState(std::cout, 8, 4);
	if (reportLogOpen) {
	    report.printState(reportLog, 8, 4);
	    reportLog.flush();
	}
    }
    virtual void csII_reportThermopileCalibration(BWCloudSensorII *csII,
        const BWCSII_reportThermopileCalibration &report)
    {
	std::cout << "csII_reportThermopileCalibration:" << std::endl;
	report.printState(std::cout, 8, 4);
	if (reportLogOpen) {
	    report.printState(reportLog, 8, 4);
	    reportLog.flush();
	}
    }
    virtual void csII_reportSensorData(BWCloudSensorII *csII,
        const BWCSII_reportSensorData &report)
    {
	std::cout << "csII_reportSensorData:" << std::endl;
	report.printState(std::cout, 8, 4);
	if (reportLogOpen) {
	    report.printState(reportLog, 8, 4);
	    reportLog.flush();
	}
    }

    virtual void csII_roofClose(BWCloudSensorII *csII) {
	std::cout << "csII_roofClose:" << std::endl;
    }
    virtual void csII_readLooping(BWCloudSensorII *csII) {
	char c;
	if (userKeyboardAbort(&c))
	    csII->readStop();
	else if (c != '\0')
	    doSingleCharCmd(csII, c);
    }
};

int
main(int argc, char *argv[])
{
    const char *dev = "/dev/cloudSensorII";
    const char *progName = "test_bwcsII";

    logmsg.setProgName(progName);
    logmsg.setDebuggingState("all,logTo(type=stderr,priority=debug)");
    for (int i = 1; i < argc; ++i) {
	if (strncmp(argv[i], "/dev/", 5) == 0) {
	    dev = argv[i];
	} else {
	    std::cerr << argv[0] << ": unknown argument: " << argv[i]
		<< std::endl;
	    exit(1);
	}
    }
    TestListener listener;
    BWCloudSensorII *csII = new BWCloudSensorII(&listener, progName, "/tmp",
	BWCloudSensorII::DF_SERIAL_LOG);

    /*
     * put the terminal in raw io to allow single character reads.
     */
    struct termios saveAttr, tAttr;
    if ( tcgetattr( 0, &tAttr ) < 0 ) {
	std::cout << "cannot get params" << std::endl;
        return 1;
    }
    saveAttr = tAttr;
    tAttr.c_lflag &= ~(ICANON);
    if ( tcsetattr( 0, TCSANOW, &tAttr ) < 0 ) {
	std::cout << "can not set params" << std::endl;
    }
    /* test harness. */

    std::cout << "? for help" << std::endl;

    char xCmd[1024];
    bool useXCmd = false;
    bool printPrompt = true;
    xCmd[0] = '\0';
    while ( 1 ) {
	try {
	    if (printPrompt) {
		std::cout << "> " << std::flush;
	    }
	    printPrompt = true;
	    char cmd;
	    if ( read(0, &cmd, 1 ) != 1 ) break;
	    if (cmd != ':')
		std::cout << std::endl;
	    switch( cmd ) {
	      case '?': {
		std::ostringstream helpOut;
		helpOut << "Commands:" << std::endl;
		helpOut << "	:quit<RETURN>	exit." << std::endl;
		helpOut << "	:sleep n	sleep for n seconds."
		    << std::endl;
		helpOut << "	:x ... 		Save ... in the x buffer (see x cmd)."
		    << std::endl;
		helpOut << "	:printState 	Ask csII driver to print its state info."
		    << std::endl;
		helpOut << "	:open [tty]	Open the serial connection."
		    << std::endl;
		helpOut << "	:close 		Close the serial connection."
		    << std::endl;
		helpOut << "	:resetSensorHead 	Reset the firmware."
		    << std::endl;
		helpOut << "	:requestPolls 	Ask CSII to send data to the PC."
		    << std::endl;
		helpOut << "	:readLoop 	Loop reading data (q to stop)"
		    << std::endl;
		helpOut << "	:readStop 	Tell readLoop to stop looping."
		    << std::endl;
		helpOut << "	:checkForData 	Check for and process data from the CSII"
		    << std::endl << "			(does not block)."
		    << std::endl;
		helpOut << "	:setT cloudy veryCloudy windy rain wet "
		    << std::endl;
		helpOut << "			Set the thresholds, first 3 are reals,"
		    << std::endl << "			last 2 are integers."
		    << std::endl;
		helpOut << "	:setT defaults	Set the thresholds to the factory defaults."
		    << std::endl;
		helpOut << "	:roofClose 	Ask CSII to ask the roof to close."
		    << std::endl;
		helpOut << "	:crc ... 	Calculate and print the CRC of ..."
		    << std::endl;
		helpOut << "	:raw ... 	Send raw data (...) to the CSII."
		    << std::endl;
		helpOut << "	Cntl-D		Exit." << std::endl;
		helpOut << "	x		Execute command in the x buffer (see :x command)."
		    << std::endl;
		helpOut << "	C		Call checkForData()."
		    << std::endl;
		helpOut << "	c		Call requestCalibrationData()."
		    << std::endl;
		helpOut << "	k		Call requestWetnessCalibrationData()."
		    << std::endl;
		helpOut << "	p 		Call requestPolls: ask CSII to send data to the PC."
		    << std::endl;
		helpOut << "	P		Call printState()."
		    << std::endl;
		helpOut << "	s		Print all the lastest report info."
		    << std::endl;
		signal(SIGPIPE, SIG_IGN);
		FILE *pager = popen("${PAGER:-less}", "w");
		std::string helpStr = helpOut.str();
		fwrite(helpStr.c_str(), 1, helpStr.length(), pager);
		pclose(pager);
		signal(SIGPIPE, SIG_DFL);
		break;
	      }
	      colonCmd:
	      case ':': /* command line mode */
	      {
		char buf[1024];
		char cmdWord[sizeof(buf)];
		int nWords = 0;
		int scanLen = 0; /* %n value from sscanf() */

		if (useXCmd) {
		    useXCmd = false;
		    unsigned int len = strlen(xCmd);
		    if (len >= sizeof(buf))
			len = sizeof(buf) - 1;
		    memcpy(buf, xCmd, len);
		    buf[len] = '\0';
		    std::cout << buf << std::endl;
		} else {
		    unsigned int offset = 0;
		    do {
			if (read(0, &buf[offset], 1) != 1)
			    break;
			if (buf[offset] == '\010' || buf[offset] == '\177') {
			    if (offset > 0) {
				std::cout << "\b\b\b    \b\b\b\b" << std::flush;
				--offset;
			    } else
				std::cout << "\b\b  \b\b" << std::flush;
			} else if (buf[offset] == '\025') {
			    std::cout << "\r                                      \r> :" << std::flush;
			    offset = 0;
			} else if (buf[offset] == ':' && offset == 0)
			    std::cout << "\b \b" << flush;
			else if (offset >= sizeof(buf))
			    std::cout << "\a\b \b" << flush;
			else
			    offset++;
		    } while ( offset == 0 || buf[offset-1] != '\n' );
		    buf[offset-1] = '\0';
		}
		/* Trim leading white space and note first word. */
		{
		    const char *p = buf;
		    while (*p && isspace(*p))
			++p;
		    if (p != buf)
			memmove(buf, p, strlen(p) + 1);
		    for (p = buf; *p && !isspace(*p); ++p)
			cmdWord[p - buf] = *p;
		    cmdWord[p - buf] = '\0';
		}
		/* Count the words. */
		{
		    bool inWord = false;
		    const char *p = buf;
		    for (; *p != '\0'; ++p) {
			if (isspace(*p)) {
			    if (inWord)
				inWord = false;
			} else {
			    if (!inWord) {
				inWord = true;
				nWords++;
			    }
			}
		    }
		}
		if ( nWords == 0 ) {
		    break;
		} else if ( strcmp(cmdWord, "quit") == 0 ) {
		    if (nWords == 1)
			goto endloop;
		    else
			std::cout << "Invalid " << cmdWord << " command: "
			    << buf << std::endl;
		} else if (buf[0] == '#') {
		    /* comment. */
		} else if (strcmp(cmdWord, "sleep") == 0) {
		    unsigned int nSecs = 0;
		    if (sscanf(buf, "sleep %u %n", &nSecs, &scanLen) >= 1
			&& buf[scanLen] == '\0')
		    {
			doSleep(nSecs);
		    } else
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		} else if (strcmp(cmdWord, "x") == 0) {
		    const char *p = buf + 1;
		    while (isspace(*p) || *p == ':')
			++p;
		    unsigned int len = strlen(p);
		    if (len == 0) {
			std::cout << xCmd << std::endl;
		    } else if (len >= sizeof(xCmd))
			std::cout << "command too long\n";
		    else
			strcpy(xCmd, p);
		} else if (strcmp(cmdWord, "printState") == 0) {
		    if (nWords == 1) {
			std::cout << "Calling printState..." << std::endl;
			csII->printState(std::cout, 4, 4);
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "open") == 0) {
		    if (nWords == 1) {
			std::cout << "Calling open(\"" << dev << "\")..."
			    << std::flush;
			csII->open(dev);
			std::cout << " done." << std::endl;
		    } else if (nWords == 2) {
			char tty[sizeof(buf)];
			tty[0] = '\0';
			sscanf(buf, "open %s", tty);
			std::cout << "Calling open(\"" << tty << "\")..."
			    << std::flush;
			csII->open(tty);
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "close") == 0) {
		    if (nWords == 1) {
			std::cout << "Calling close..." << std::flush;
			csII->close();
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "resetSensorHead") == 0) {
		    if (nWords == 1) {
			std::cout << "Calling resetSensorHead..." << std::flush;
			csII->resetSensorHead();
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "requestPolls") == 0) {
		    if (nWords == 1) {
			std::cout << "Calling requestPolls..." << std::flush;
			csII->requestPolls();
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "readLoop") == 0) {
		    if (nWords == 1) {
			std::cout << "Calling readLoop (press q to stop)..."
			    << std::flush;
			csII->readLoop();
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "readStop") == 0) {
		    if (nWords == 1) {
			std::cout << "Calling readStop..." << std::flush;
			csII->readStop();
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "checkForData") == 0) {
		    if (nWords == 1) {
			std::cout << "Calling checkForData..." << std::flush;
			csII->checkForData();
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "setT") == 0) {
		    BWCSII_setThresholds t;
		    if (sscanf(buf, "setT %lf %lf %lf %d %d %n",
			    &t.cloudyThresh, &t.veryCloudyThresh,
			    &t.windyThresh,
			    &t.rainThresh, &t.wetThresh,
			    &scanLen) >= 5
			&& buf[scanLen] == '\0')
		    {
			std::cout << "Calling setThresholds with:" << std::endl;
			t.printState(std::cout, 4, 4);
			csII->setThresholds(t);
			std::cout << "setThresholds done." << std::endl;
		    } else if (sscanf(buf, "setT defaults %n",
			    &scanLen) >= 0
			&& buf[scanLen] == '\0')
		    {
			std::cout << "Calling setDefaultThresholds..."
			    << std::flush;
			csII->setDefaultThresholds();
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "roofClose") == 0) {
		    if (nWords == 1) {
			std::cout << "Calling sendRoofClose..." << std::flush;
			csII->sendRoofClose();
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "raw") == 0) {
		    if (nWords > 0) {
			const char *p = buf;
			while (isspace(*p)) /* leading spaces */
			    ++p;
			while (*p && !isspace(*p)) /* "raw" */
			    ++p;
			while (isspace(*p)) /* more spaces */
			    ++p;
			std::cout << "Calling raw command <" << p << ">..."
			    << std::flush;
			csII->sendRawCommand(p);
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "crc") == 0) {
		    if (nWords > 0) {
			const char *p = buf;
			while (isspace(*p)) /* leading spaces */
			    ++p;
			while (*p && !isspace(*p)) /* "crc" */
			    ++p;
			while (isspace(*p)) /* more spaces */
			    ++p;
			std::cout << "Calling calcCRC16 on <" << p << ">..."
			    << std::flush;
			unsigned int crc = BWCloudSensorII::calcCRC16(p,
			    strlen(p));
			char sp[32];
			snprintf(sp, sizeof(sp), "0x%04x", crc);
			std::cout << sp << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else if (strcmp(cmdWord, "log") == 0) {
		    char fileName[sizeof(buf)];
		    if (sscanf(buf, "log %s %n", fileName, &scanLen) >= 1
			&& buf[scanLen] == '\0')
		    {
			if (reportLogOpen)
			    reportLog.close();
			reportLogOpen = false;
			/* Some older g++ libraries don't have ios_base so
			 * use ostream instead.
			 */
			reportLog.open(fileName,
			    std::ostream::out | std::ostream::ate);
			if (!reportLog) {
			    std::cout << "error opening " << fileName
				<< " - " << strerror(errno) << std::endl;
			} else {
			    std::cout << "Logging to " << fileName << std::endl;
			    reportLogOpen = true;
			}
		    } else if (sscanf(buf, "setT defaults %n",
			    &scanLen) >= 0
			&& buf[scanLen] == '\0')
		    {
			std::cout << "Calling setDefaultThresholds..."
			    << std::flush;
			csII->setDefaultThresholds();
			std::cout << " done." << std::endl;
		    } else {
			std::cerr << "can't parse " << cmdWord << " command."
			    << std::endl;
		    }
		} else
		    std::cout << "unknown : command." << std::endl;
		break;
	      }

	      case 'x':
	        useXCmd = true;
		goto colonCmd;
	        break;

	      case '\004': /* Control-D */
		goto endloop;
		break;

	    default:
		doSingleCharCmd(csII, cmd);
	    }
	} catch(const SerialConnectionException &ex) {
	    std::cout << "serial connection exception: " << ex.getMessage() << std::endl;
	} catch(const GenericException &ex) {
	    std::cout << "generic exception: " << ex.getMessage() << std::endl;
	} catch(...) {
	    std::cout << "unknown exception: " << std::endl;
	}
    }

endloop:
    if ( tcsetattr( 0, TCSANOW, &saveAttr ) < 0 ) {
	std::cout << "can not reset params" << std::endl;
    }
    try {
	if (csII != NULL) {
	    std::cout << "Deleting driver object" << std::endl;
	    delete csII;
	}
    } catch (const GenericException &e) {
	std::cout << "Exception deleting driver object: " << e.getMessage()
	    << std::endl;
	return 1;
    }
    return 0;
}
