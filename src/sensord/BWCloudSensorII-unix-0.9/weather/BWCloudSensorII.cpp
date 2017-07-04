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
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <memory.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <algorithm>

#include "util/EnumNamer.h"
#include "util/Log.h"
#include "weather/BWCloudSensorII.h"


/* Yuck - linux/glibc dependancy: using undocumented struct tm.tm_gmtoff field
 * which is sometimes called __tm_gmtoff.
 */
# ifndef __USE_MISC
#  define tm_gmtoff  __tm_gmtoff 
# endif /* __USE_BSD */


/*
 * Class BWCloudSensorII
 */

const char BWCloudSensorII::purgeCharSequence[] = "!**!";
const char BWCloudSensorII::driverName[] = "unix-BWCloudSensorII";
const char BWCloudSensorII::driverVersion[] = "0.8";

BWCloudSensorII::BWCloudSensorII(BWCSII_Listener *listener,
	const char *applicationName, const char *csIILogDirectory,
	DriverFlags driverFlags)
    : listener(listener), csIILogger(applicationName,
      csIILogDirectory), driverFlags(driverFlags), serial(NULL),
      readTimeoutMS(READ_TIMEOUT_MS), readLoopState(RLS_NONE),
      parseState(PS_NONE), parseAction(PA_NONE),
      nextPurgeChar(NULL), purgeReasonChar(0), memoryDumpSawFirstEnd(false),
      lastWetSkyMinusAmbientTime(0)
{
    timerclear(&parseStateTime);
    timerclear(&pollTime);
    timerclear(&requestPollTime);
}

void
BWCloudSensorII::printState(std::ostream &os, int indent, int indentIncr) const
{
    os << std::setw(indent) << "" << "BWCloudSensorII "
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    os << iStr << "listener=" << listener << std::endl;
    os << iStr << "csIILogger:" << std::endl;
    csIILogger.printState(os, indent + indentIncr, indentIncr);
    os << iStr << "driverFlags=0x"
	<< std::setbase(16) << driverFlags << std::setbase(10) << std::endl;
    os << iStr << "ttyDevice=\"" << ttyDevice << '"' << std::endl;
    os << iStr << "serial=" << serial << std::endl;
    if (serial != NULL)
	serial->printState(os, indent + indentIncr, indentIncr);
    os << iStr << "readTimeoutMS=" << readTimeoutMS << std::endl;
    os << iStr << "readLoopState=" << readLoopState << std::endl;
    os << iStr << "parseState=" << parseState << std::endl;
    os << iStr << "parseStateTime.tv_sec=" << parseStateTime.tv_sec
	<< ", tv_usec=" << parseStateTime.tv_usec
	<< " - " << toStringTimeT(parseStateTime.tv_sec) << std::endl;
    os << iStr << "parseAction=" << parseAction << std::endl;
    os << iStr << "pollTime.tv_sec=" << pollTime.tv_sec
	<< ", tv_usec=" << pollTime.tv_usec
	<< " - " << toStringTimeT(pollTime.tv_sec) << std::endl;
    os << iStr << "requestPollTime.tv_sec=" << requestPollTime.tv_sec
	<< ", tv_usec=" << requestPollTime.tv_usec
	<< " - " << toStringTimeT(requestPollTime.tv_sec) << std::endl;
    os << iStr << "inputBuf.size()=" << inputBuf.size() << std::endl;
    os << iStr << "n cmdQueue=" << cmdQueue.size() << " {" << std::endl;
    for (CmdQueue::const_iterator it = cmdQueue.begin(); it != cmdQueue.end();
	++it)
    {
	const CmdEntry *cmd = *it;
	cmd->printState(os, indent + indentIncr, indentIncr);
    }
    os << iStr << "}" << std::endl;
    os << iStr << "nextPurgeChar="
	<< (nextPurgeChar != NULL ? nextPurgeChar : "(null)") << std::endl;
    os << iStr << "purgeReasonChar=" << toStringDump(&purgeReasonChar, 1)
	<< std::endl;
    os << iStr << "memoryDumpSawFirstEnd=" << memoryDumpSawFirstEnd
	<< std::endl;
    os << iStr << "lastWetSkyMinusAmbientTime=" << lastWetSkyMinusAmbientTime
	<< " - " << toStringTimeT(lastWetSkyMinusAmbientTime) << std::endl;

    os << iStr << "lastReportI:" << std::endl;
    lastReportI.printState(os, indent + indentIncr, indentIncr);
    os << iStr << "lastReportT:" << std::endl;
    lastReportT.printState(os, indent + indentIncr, indentIncr);
    os << iStr << "lastReportK:" << std::endl;
    lastReportK.printState(os, indent + indentIncr, indentIncr);
    os << iStr << "lastReportW:" << std::endl;
    lastReportW.printState(os, indent + indentIncr, indentIncr);
    os << iStr << "lastReportC:" << std::endl;
    lastReportC.printState(os, indent + indentIncr, indentIncr);
    os << iStr << "lastReportD:" << std::endl;
    lastReportD.printState(os, indent + indentIncr, indentIncr);

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

void
BWCloudSensorII::open(const char *ttyDev)
    throw (SerialConnectionException)
{
    static const char methodName[] = "BWCloudSensorII::open";

    ttyDevice = ttyDev;
    if (serial == NULL)
	serial = new SerialConnection(ttyDevice.c_str());
    else
	serial->setDeviceName(ttyDevice.c_str());
    serial->reopen();
    serial->setTxRxBitRate(SerialConnection::BR4800, SerialConnection::BR4800);
    serial->setBitsInData(8);
    serial->setStopBits(SerialConnection::S_1);
    serial->setParity(SerialConnection::P_NONE);
    serial->setMode(SerialConnection::M_RAW);
    csIILogger.logString("c", "opened");
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: opened", methodName));
}

bool
BWCloudSensorII::isOpen() const
{
    return serial != NULL && serial->isOpen();
}

bool
BWCloudSensorII::close()
{
    static const char methodName[] = "BWCloudSensorII::close";

    if (serial != NULL) {
	if (readLoopState == RLS_READING || readLoopState == RLS_STOP
	    || readLoopState == RLS_CLOSE)
	{
	    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC,
		("%s: read loop active - requesting close.", methodName));
	    readLoopState = RLS_CLOSE;
	    try {
		serial->close();
	    } catch (const SerialConnectionException &ex) {
		logmsg.error("%s: exception closing serial device: %s",
		    methodName, ex.getMessage().c_str());
	    }
	    return false;
	}
	delete serial; /* closes device. */
	serial = NULL;
    }
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: closed.", methodName));
    return true;
}

void
BWCloudSensorII::resetSensorHead()
    throw (SerialConnectionException)
{
    static const char methodName[] = "BWCloudSensorII::resetSensorHead";

    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }
    serial->setTxRxBitRate(SerialConnection::BR1200, SerialConnection::BR1200);
    serial->writeFully("", 1);
    serial->setTxRxBitRate(SerialConnection::BR4800, SerialConnection::BR4800);
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: reset sent.", methodName));
    struct timeval now;
    gettimeofday(&now, NULL);
    resetState(now);
    /* This so a poll request will be sent quickly. */
    timerclear(&pollTime);
    requestPollTime = now;
    requestPollTime.tv_sec -= POLL_TIMEOUT_SEC - RESET_TIME_SEC;
}

void
BWCloudSensorII::requestPolls()
    throw (SerialConnectionException)
{
    static const char methodName[] = "BWCloudSensorII::requestPolls";

    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }
    gettimeofday(&requestPollTime, NULL);
    char buf[1];
    buf[0] = REQUEST_POLL;
    serial->writeFully(buf, 1);
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: request sent.", methodName));
}

void
BWCloudSensorII::readLoop()
    throw (SerialConnectionException)
{
    static const char methodName[] = "BWCloudSensorII::readLoop";

    if (readLoopState != RLS_NONE) {
	logmsg.error("%s: called when already reading (readState %s)",
	    methodName, getReadLoopStateName(readLoopState));
	return;
    }
    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }
    readLoopState = RLS_READING;
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: looping...", methodName));
    try {
	while (readLoopState == RLS_READING && serial != NULL) {
	    char buf[1];
	    /* Wait for data */
	    serial->readTimeout(buf, 0, readTimeoutMS);
	    if (readLoopState == RLS_READING) {
		checkForData();
		if (listener != NULL)
		    listener->csII_readLooping(this);
	    }
	}
	logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: finished.", methodName));
	bool doClose = readLoopState == RLS_CLOSE;
	readLoopState = RLS_NONE;
	if (doClose)
	    close();
    } catch (...) {
	logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: exception!", methodName));
	bool doClose = readLoopState == RLS_CLOSE;
	readLoopState = RLS_NONE;
	if (doClose)
	    close();
	throw;
    }
}

bool
BWCloudSensorII::inReadLoop()
{
    return readLoopState != RLS_NONE;
}

void
BWCloudSensorII::readStop()
{
    static const char methodName[] = "BWCloudSensorII::readStop";

    if (readLoopState == RLS_NONE) {
	logmsg.error("%s: called when not reading", methodName);
	return;
    }
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: requesting stop.", methodName));
    if (readLoopState != RLS_CLOSE)
	readLoopState = RLS_STOP;
}

void
BWCloudSensorII::checkForData()
    throw (SerialConnectionException)
{
    static const char methodName[] = "BWCloudSensorII::checkForData";

    char buf[1024];

    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }

    struct timeval readTime;
    gettimeofday(&readTime, NULL);
    unsigned int nRead = serial->readTimeout(buf, sizeof(buf), 0);
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC_V,
	("%s: read %d bytes", methodName, nRead));
    for (unsigned int i = 0; i < nRead; ++i) {
	unsigned char c = buf[i];
	if (parseState == PS_IN_PURGE) {
	    if (*nextPurgeChar == '\0') {
		doPurge(c, readTime);
		break;
	    } else if (c == *nextPurgeChar) {
		++nextPurgeChar;
		continue;
	    } else if (c == '\n' && nextPurgeChar - purgeCharSequence == 1) {
		/* memory dump seems to be "!\n|..."; allow the \n to
		 * be ignored.
		 */
		logmsg.notice("%s: newline after purge character - sending poll request (memory dump coming?)",
		    methodName);
		requestPolls();
		continue;
	    } else if (c == MEMORY_DUMP_START) {
		startMemoryDump(readTime);
		continue;
	    } else {
		logmsg.error(
		    "%s: unexpected char after start of purge sequence (0x%02x, index %d)",
		    methodName, (int) c, nextPurgeChar - purgeCharSequence);
		/* '-' to indicate no reason (arbitrary) */
		doPurge('-', readTime);
		break;
	    }
	}
	if (c == PURGE_START) {
	    // useful?: csIILogger.logString("h", "Purge initiated");
	    setParseState(PS_IN_PURGE, readTime);
	    nextPurgeChar = purgeCharSequence + 1;
	    continue;
	}
	/* Attempt to re-sync. */
	if (c == FRAME_START) {
	    if (parseState != PS_NONE)
		logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC,
		    ("%s: received frame start in parseState=%s (inputBuf.size=%d)",
		    methodName, getParseStateName(parseState),
		    inputBuf.size()));
	    setParseState(PS_NONE, readTime);
	    /* carry on. */
	}
	switch (parseState) {
	  case PS_NONE:
	    if (c == FRAME_START) {
		setParseState(PS_IN_FRAME, readTime);
		inputBuf.clear();
	    } else if (c == '\0') {
		/* ignore - generated just ?before or after? a frame. */
	    } else if (c == BOOT_LOADER_START) {
		setParseState(PS_SKIP_TO_FRAME_END, readTime);
		logmsg.error("%s, ignoring boot loader command", methodName);
	    } else if (c == MEMORY_DUMP_START) {
		/* This is here in case the purge\n happens before
		 * this program starts, in which case the first poll
		 * will start the memory dump.
		 */
		startMemoryDump(readTime);
	    } else {
		/* Hmm... may get too verbose. */
		logmsg.warning("%s, ignoring spurious character (0x%02x)",
		    methodName, (int) c);
	    }
	    break;

	  case PS_SKIP_TO_FRAME_END:
	    if (c == FRAME_END)
		setParseState(PS_NONE, readTime);
	    break;

	  case PS_IN_MEMORY_DUMP:
	    if (c == FRAME_END) {
		/* See "pretend" comment below. */
		if (!memoryDumpSawFirstEnd) {
		    memoryDumpSawFirstEnd = true;
		} else {
		    setParseState(PS_NONE, readTime);
		    parseMemoryDump();
		}
	    } else if (!isxdigit(c)) {
		setParseState(PS_SKIP_TO_FRAME_END, readTime);
		logmsg.error("%s: non-hex digit character in memory dump (0x%02x, offset %d)", methodName, (int) c, inputBuf.size());
	    } else if (inputBuf.size() > MAX_MEMORY_DUMP_SIZE * 2) {
		setParseState(PS_SKIP_TO_FRAME_END, readTime);
		logmsg.error("%s: memory dump too large (%d)",
		    methodName, inputBuf.size());
	    } else {
		memoryDumpSawFirstEnd = true; /* pretend was seen. */
		inputBuf.push_back(c);
	    }
	    break;

	  case PS_IN_FRAME:
	    if (c == FRAME_END) {
		setParseState(PS_NONE, readTime);
		parseAction = parseFrame(readTime);
	    } else if (inputBuf.size() > FRAME_MAX_LENGTH) {
		setParseState(PS_SKIP_TO_FRAME_END, readTime);
		logmsg.error("%s: frame too large (%d) - skipping",
		    methodName, inputBuf.size());
	    } else {
		inputBuf.push_back(c);
	    }
	    break;
	  default:
	    logmsg.internalError("%s: unknown parse state: %s",
		methodName, getParseStateName(parseState));
	    setParseState(PS_NONE, readTime);
	}
    }
    /*
     * Avoid sending a response to a previous frame when in the middle
     * of another frame (half duplex).
     */
    if (parseState == PS_NONE) {
	ParseAction pa = parseAction;
	parseAction = PA_NONE;
	if (pa != PA_NONE)
	    /* Delay a little to avoid collisions on the half-duplex line. */
	    ::usleep(10000); // XXX did not help, might be good idea anyway
	switch (pa) {
	  case PA_NONE:
	    break;
	  case PA_SEND_ACK:
	    sendAck();
	    break;
	  case PA_SEND_NACK:
	    sendNack();
	    break;
	  case PA_POLL:
	    pollCheck(readTime);
	    break;
	  default:
	    logmsg.internalError("%s: unknown parseAction %s",
		methodName, getParseActionName(pa));
	}
    }
    flushCmdQueue(readTime);

    time_t timeoutSecs = PARSE_TIMEOUT_SEC;
    if (parseState == PS_IN_PURGE || parseState == PS_IN_MEMORY_DUMP)
	timeoutSecs = PARSE_PURGE_TIMEOUT_SEC;
    if (parseState != PS_NONE) {
	struct timeval tvDiff = timevalSub(readTime, parseStateTime);
	if (tvDiff.tv_sec >= timeoutSecs) {
	    logmsg.error("%s: protocol error - timeout (aborting parseState=%s (timed out, %ld seconds))",
		 methodName, getParseStateName(parseState),
		 (long) tvDiff.tv_sec);
	    setParseState(PS_NONE, readTime);
	}
    }
    /* Poll needed to start memory dump (but not during it) and is sent above,
     * but if lost should re-send it.
     */
    if (parseState != PS_IN_MEMORY_DUMP) {
	struct timeval tv;
	if (timercmp(&pollTime, &requestPollTime, >)) {
	    tv = pollTime;
	} else {
	    tv = requestPollTime;
	}
	struct timeval tvDiff = timevalSub(readTime, tv);
	if (
	    // tv.tv_sec == 0 && // XXX disable
	    tvDiff.tv_sec >= POLL_TIMEOUT_SEC)
	{
	    requestPolls();
	}
    }
}

int
BWCloudSensorII::getFileDescriptor() const
{
    return isOpen() ? serial->getFD() : -1;
}

const BWCSII_reportReset &
BWCloudSensorII::getLastReset() const
{
    return lastReportI;
}

const BWCSII_reportThresholds &
BWCloudSensorII::getLastThresholds() const
{
    return lastReportT;
}

const BWCSII_reportWetnessCalibration &
BWCloudSensorII::getLastWetnessCalibration() const
{
    return lastReportK;
}

const BWCSII_reportWetnessData &
BWCloudSensorII::getLastWetnessData() const
{
    return lastReportW;
}

const BWCSII_reportThermopileCalibration &
BWCloudSensorII::getLastThermopileCalibration() const
{
    return lastReportC;
}

const BWCSII_reportSensorData &
BWCloudSensorII::getLastSensorData() const
{
    return lastReportD;
}

void
BWCloudSensorII::requestCalibrationData()
{
    static const char methodName[] = "BWCloudSensorII::requestCalibrationData";

    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }
    queueCmd(new CmdEntry(BWCSII_Listener::CT_GET_CALIBRATION, "c"));
}

void
BWCloudSensorII::requestWetnessCalibrationData()
{
    static const char methodName[]
	 = "BWCloudSensorII::requestWetnessCalibrationData";

    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }
    queueCmd(new CmdEntry(BWCSII_Listener::CT_GET_WETNESS_CALIBRATION, "l"));
}

void
BWCloudSensorII::setThresholds(
	const BWCSII_setThresholds &newThresholds)
{
    static const char methodName[] = "BWCloudSensorII::setThresholds";

    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }
    char buf[1024];
    snprintf(buf, sizeof(buf), "t %d %d %d %d %d",
	(int) (newThresholds.cloudyThresh * 10 + 0.5),
	(int) (newThresholds.veryCloudyThresh * 10 + 0.5),
	(int) (newThresholds.windyThresh * 10 + 0.5),
	newThresholds.rainThresh, newThresholds.wetThresh);
    queueCmd(new CmdEntry(BWCSII_Listener::CT_SET_THRESHOLDS, buf));
}

void
BWCloudSensorII::setDefaultThresholds()
{
    static const char methodName[] = "BWCloudSensorII::setDefaultThresholds";

    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }
    queueCmd(new CmdEntry(BWCSII_Listener::CT_SET_THRESHOLDS_DEFAULT, "t"));
}

void
BWCloudSensorII::sendRoofClose()
{
    static const char methodName[] = "BWCloudSensorII::sendRoofClose";

    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }
    queueCmd(new CmdEntry(BWCSII_Listener::CT_ROOF_CLOSE, "b"));
}

void
BWCloudSensorII::sendRawCommand(const char *command)
{
    static const char methodName[] = "BWCloudSensorII::sendRawCommand";

    if (!isOpen()) {
	logmsg.error("%s: called when serial connection not open.", methodName);
	return;
    }
    queueCmd(new CmdEntry(BWCSII_Listener::CT_RAW, command));
}

void
BWCloudSensorII::setNDailyLogs(int nDailyLogs)
{
    csIILogger.setNDailyLogs(nDailyLogs);
}


/* Add a command to the command queue. */
void
BWCloudSensorII::queueCmd(CmdEntry *cmd)
{
    static const char methodName[] = "BWCloudSensorII::queueCmd";

    gettimeofday(&cmd->queuedTime, NULL);
    cmd->cmdState = CmdEntry::CS_PENDING;
    cmdQueue.push_back(cmd);
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC,
	("%s: queued command (cmdType %s, cmd<%s>).", methodName,
	 BWCSII_Listener::getCmdTypeName(cmd->cmdType), cmd->cmd.c_str()));
}

void
BWCloudSensorII::setParseState(ParseState ps, const struct timeval &tv)
{
    parseState = ps;
    parseStateTime = tv;
    parseAction = PA_NONE;
}

void
BWCloudSensorII::startMemoryDump(const struct timeval &readTime)
{
    csIILogger.logString("ch", "RAM Dump Initiating From Boot Loader");
    setParseState(PS_IN_MEMORY_DUMP, readTime);
    /* The ! is immediately followed by a \n which must
     * be skipped and not taken as the end of memory dump.
     */
    memoryDumpSawFirstEnd = false;
    inputBuf.clear();
}

/*
 * CSII has asked for a purge - flush commands, let listener know.
 */
void
BWCloudSensorII::doPurge(unsigned char reasonChar, const struct timeval &tv)
    throw (SerialConnectionException)
{
    static const char methodName[] = "BWCloudSensorII::doPurge";

    if (!isOpen()) {
	logmsg.internalError("%s: called when serial connection not open.",
	    methodName);
	return;
    }

    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: purging...", methodName));
    purgeReasonChar = reasonChar;
    resetState(tv);
    if (listener != NULL)
	listener->csII_purgeEvent(this, reasonChar);
    /* Done last in case of exception. */
    serial->discardIO();
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: purged.", methodName));
}

/*
 * Reset parse state and empty the command queue.
 */
void
BWCloudSensorII::resetState(const struct timeval &tv)
{
    setParseState(PS_NONE, tv);
    while (!cmdQueue.empty()) {
	CmdEntry *cmd = cmdQueue.front();
	cmdQueue.pop_front();
	cmd->cmdState = CmdEntry::CS_PURGED;
	if (listener != NULL)
	    listener->csII_cmdOver(this, cmd->cmdType,
		BWCSII_Listener::COR_PURGED);
	delete cmd;
    }
}

/* A memory dump "frame" has been received - parse the contents and
 * let the listener know.
 */
void
BWCloudSensorII::parseMemoryDump()
{
    static const char methodName[] = "BWCloudSensorII::parseMemoryDump";

    std::string binaryDump;
    for (std::string::iterator it = inputBuf.begin();
	it != inputBuf.end(); )
    {
	unsigned char c1 = *it++;
	if (it == inputBuf.end()) {
	    logmsg.error("%s: odd number of hex digits in memory dump (%d)",
		methodName, inputBuf.size());
	    return;
	}
	unsigned char c2 = *it++;
	c1 = (unsigned char) toHexDigit(c1);
	c2 = (unsigned char) toHexDigit(c2);
	binaryDump.push_back((char) (c1 * 16 + c2));
    }
    if (binaryDump.size() == 0) {
	logmsg.error("%s: empty memory dump", methodName);
	return;
    }
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: have dump (%u bytes).", methodName,
	    (unsigned int) binaryDump.size()));
    {
	const unsigned int bytesPerLine = 16;
	const unsigned int bytesPerBlock = 8;
	const int addrBase = 0x60;
	csIILogger.logString("ch", "RAM Dump From Boot Loader", false);
	char buf[7 + bytesPerLine * 3];
	for (unsigned int i = 0; i < binaryDump.size(); i += bytesPerLine) {
	    snprintf(buf, sizeof(buf), "%04x ", addrBase + i);
	    unsigned int bufI = strlen(buf);
	    unsigned int n = i + bytesPerLine;
	    if (n > binaryDump.size())
		n = binaryDump.size();
	    for (unsigned int j = i; j < n; ++j) {
		snprintf(buf + bufI, sizeof(buf) - bufI, " %02x",
		    binaryDump[j]);
		bufI += 3;
		if (j - i == bytesPerBlock && bufI < sizeof(buf) - 1)
		    buf[bufI++] = ' ';
	    }
	    if (bufI < sizeof(buf))
		buf[bufI] = '\0';
	    csIILogger.logString("cD", buf, false);
	}
    }
    if (listener != NULL)
	listener->csII_memoryDump(this, (uint8_t *) binaryDump.c_str(),
	    (unsigned int) binaryDump.size());
}

/* A complete frame has been received - parse it and take any needed actions.
 */
BWCloudSensorII::ParseAction
BWCloudSensorII::parseFrame(const struct timeval &readTime)
    throw (SerialConnectionException)
{
    static const char methodName[] = "BWCloudSensorII::parseFrame";

    unsigned int frameLen = inputBuf.size();
    if (frameLen == 0) {
	logmsg.error("%s: ignoring empty frame", methodName);
	return PA_NONE;
    }
    unsigned char cmdChar = inputBuf[0];
    if (cmdChar == 'P' && frameLen == 1) {
	return PA_POLL;
    } else if (cmdChar == 'A' && frameLen == 1) {
	ackReceived(readTime);
	return PA_NONE;
    } else if (cmdChar == 'N' && frameLen == 1 + 4) {
	nackReceived(readTime, inputBuf.c_str() + 1);
	return PA_NONE;
    } else if (cmdChar == 'M') {
	if (frameLen < 1 + 4) {
	    logmsg.error("%s: invalid data frame: cmdChar=0x%02x, frameLen=%d",
		methodName, cmdChar, frameLen);
	    return PA_SEND_NACK;
	}
	const char *p = (const char *) inputBuf.c_str();
	unsigned int calcedSum = calcCRC16(p + 1, inputBuf.size() - (1 + 4));
	int receivedSum = parseCRC(p + frameLen - 4);
	if (receivedSum < 0) {
	    logmsg.error("%s: bad characters in frame crc", methodName);
	    return PA_SEND_NACK;
	}
	if ((unsigned int) receivedSum != calcedSum) {
	    logmsg.error(
		"%s: invalid CRC in frame (received 0x%04x, calculated 0x%04x)",
		methodName, receivedSum, calcedSum);
	    return PA_SEND_NACK;
	}
	parseFrameData(readTime, inputBuf.substr(1, frameLen - 5).c_str(),
	    frameLen - 5);
	return PA_SEND_ACK;
    } else {
	logmsg.error("%s: invalid frame: cmdChar=0x%02x, frameLen=%d",
	    methodName, cmdChar, frameLen);
	return PA_NONE;
    }
}

/* A "M" frame has been received - parse it and take any needed actions. */
void
BWCloudSensorII::parseFrameData(const struct timeval &readTime,
	const char *p, unsigned int nBytes)
{
    static const char methodName[] = "BWCloudSensorII::parseFrameData";

    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC_V, ("%s: received data <%s>", methodName,
	    toStringDump(p, nBytes).c_str()));
    BWCSII_report::ReportType reportType = BWCSII_report::RT__UNINITIALIZED;
    const char *reportHeader = NULL;
    switch (p[0]) {
      case 'I': {
	BWCSII_reportReset r;
	reportType = r.reportType;
	reportHeader = r.reportHeader();
	if (!r.parseReport(methodName, readTime, p)) {
	    break;
	}
	lastReportI = r;
	if (listener != NULL)
	    listener->csII_reportReset(this, r);
	break;
      }

      case 'T': {
	BWCSII_reportThresholds r;
	reportType = r.reportType;
	reportHeader = r.reportHeader();
	if (!r.parseReport(methodName, readTime, p)) {
	    break;
	}
	lastReportT = r;
	if (listener != NULL)
	    listener->csII_reportThresholds(this, r);
	break;
      }

      case 'K': {
	BWCSII_reportWetnessCalibration r;
	reportType = r.reportType;
	reportHeader = r.reportHeader();
	if (!r.parseReport(methodName, readTime, p)) {
	    break;
	}
	lastReportK = r;
	if (listener != NULL)
	    listener->csII_reportWetnessCalibration(this, r);
	break;
      }

      case 'W': {
	BWCSII_reportWetnessData r;
	reportType = r.reportType;
	reportHeader = r.reportHeader();
	if (!r.parseReport(methodName, readTime, p)) {
	    break;
	}
	lastReportW = r;
	if (listener != NULL)
	    listener->csII_reportWetnessData(this, r);
	break;
      }

      case 'C': {
	BWCSII_reportThermopileCalibration r;
	reportType = r.reportType;
	reportHeader = r.reportHeader();
	if (!r.parseReport(methodName, readTime, p)) {
	    break;
	}
	lastReportC = r;
	if (listener != NULL)
	    listener->csII_reportThermopileCalibration(this, r);
	break;
      }

      case 'D': {
	BWCSII_reportSensorData r;
	reportType = r.reportType;
	reportHeader = r.reportHeader();
	if (!r.parseReport(methodName, readTime, p)) {
	    break;
	}
	if (r.wetSkyMinusAmbientTemperature())
	    lastWetSkyMinusAmbientTime = r.reportTime;
	if (r.reportTime - lastWetSkyMinusAmbientTime
		< BWCSII_reportSensorData::RECENTLY_WET_SECS)
	    r.recentlyWet = true;
	lastReportD = r;
	if (listener != NULL)
	    listener->csII_reportSensorData(this, r);
	break;
      }

      case 'Q':
	/* Report a command received from the PC.
	 * "Q ~%s~", a_inmsg
	 */
	logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: CSII received command <%s>",
	    methodName, toStringDump(p).c_str()));
	break;

      case 'R':
	if (listener != NULL)
	    listener->csII_roofClose(this);
	break;

	/* Debugging and development commands */
	/* "X rhr %d %f", rhr, rainheatt
	 *    Control of the rain heater.
	 */
      case 'X':
	/* "Y %f %d %d", rainheatt, suppressrh, suppressrrh
	 *   Direct setting of the rain heater target temperature.
	 */
      case 'Y':
	/* "Z %d %d %d", RAINHEAT, suppressrh, suppressrrh
	 *   Direct setting of the rain heater PWM hardware value.
	 */
      case 'Z':
	  logmsg.warning("%s: ignoring debugging/development command: <%s>",
	    methodName, toStringDump(p).c_str());
	  break;

      default:
	logmsg.error("%s: unrecognized frame data: char=%s, len=%d",
	    methodName, toStringDump(p, 1).c_str(), nBytes);
    }
    csIILogger.logReceivedFrame(readTime, p, nBytes, reportType, reportHeader);
}

/* Send an ACK frame. */
void
BWCloudSensorII::sendAck()
    throw (SerialConnectionException)
{
    sendFrame("a", 1);
}

/* Send a NACK frame. */
void
BWCloudSensorII::sendNack()
    throw (SerialConnectionException)
{
    sendFrame("n", 1);
}

/* Send a frame (add framing characters and CRC). */
void
BWCloudSensorII::sendFrame(const char *p, unsigned int nBytes)
    throw (SerialConnectionException)
{
    static const char methodName[] = "BWCloudSensorII::sendFrame";

    std::string frame;

    if (!isOpen()) {
	logmsg.internalError("%s: called when serial connection not open.",
	    methodName);
	return;
    }

    // frame += (char) '\0'; // XXX filler - attempt to make csII happy - no go
    frame += (char) FRAME_START;
    frame.append(p, nBytes);
    /* Need to append a CRC? */
    if (nBytes > 0 && *p == 'm') {
	unsigned int crc = calcCRC16(p + 1, nBytes - 1);
	char crcBuf[5];
	sprintf(crcBuf, "%02X%02X", crc / 256, crc & 0xff);
	frame.append(crcBuf, 4);
    }
    frame += (char) FRAME_END;
    /* This so the csII will continue talking; with out this, it will stop
     * after 5 messages until a SOH character is sent.
     */
    frame += (char) REQUEST_POLL;
    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC_V, ("%s: sending <%s>", methodName,
	    toStringDump(frame).c_str()));
    serial->writeFully(frame.c_str(), frame.size());
}

/* CSII has sent a poll request - send the appropriate response. */
void
BWCloudSensorII::pollCheck(const struct timeval &tv)
    throw (SerialConnectionException)
{
    static const char methodName[] = "BWCloudSensorII::pollCheck";

    pollTime = tv;
    if (listener != NULL)
	listener->csII_polled(this);
    if (cmdQueue.empty()) {
	logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC_V, ("%s: no commands.", methodName));
	sendAck();
    } else {
	CmdEntry *cmd = cmdQueue.front();
	if (cmd->cmdState == CmdEntry::CS_SENT) {
	    logmsg.warning("%s: cmd in sent state, re-sending", methodName);
	    cmd->cmdState = CmdEntry::CS_PENDING;
	}
	if (cmd->cmdState != CmdEntry::CS_PENDING) {
	    logmsg.internalError(
		"%s: cmd in unexpected state %s (cmdType %s, queuedTime %ld, now %ld)",
		methodName, CmdEntry::getCmdStateName(cmd->cmdState),
		BWCSII_Listener::getCmdTypeName(cmd->cmdType),
		(long) cmd->queuedTime.tv_sec, (long) tv.tv_sec);
	    sendAck();
	} else {
	    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC,
		("%s: sending command (cmdType %s).",
		 methodName, BWCSII_Listener::getCmdTypeName(cmd->cmdType)));
	    cmd->cmdState = CmdEntry::CS_SENT;
	    cmd->nSends++;
	    cmd->sentTime = tv;
	    std::string frame;
	    frame += 'm';
	    frame.append(cmd->cmd);
	    sendFrame(frame.c_str(), frame.size());
	}
    }
}

/* CSII has sent an ACK frame - mark the command as sent and let the listener
 * know.
 */
void
BWCloudSensorII::ackReceived(const struct timeval &tv)
{
    static const char methodName[] = "BWCloudSensorII::ackReceived";

    if (cmdQueue.empty()) {
	logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: cmdQueue empty!", methodName));
	return;
    }
    CmdEntry *cmd = cmdQueue.front();
    if (cmd->cmdState == CmdEntry::CS_SENT) {
	logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: cmd acked (cmdType %s)",
		methodName, BWCSII_Listener::getCmdTypeName(cmd->cmdType)));
	cmdQueue.pop_front();
	cmd->cmdState = CmdEntry::CS_DONE;
	cmd->ackedTime = tv;
	if (listener != NULL)
	    listener->csII_cmdOver(this, cmd->cmdType,
		BWCSII_Listener::COR_SUCCESS);
	delete cmd;
    } else {
	logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: unsent command! (cmdType %s)",
		methodName, BWCSII_Listener::getCmdTypeName(cmd->cmdType)));
    }
}

/* CSII has sent a NACK frame - mark the command for re-sending. */
void
BWCloudSensorII::nackReceived(const struct timeval &tv, const char *crcStr)
{
    static const char methodName[] = "BWCloudSensorII::nackReceived";

    int crc = parseCRC(crcStr);
    if (crc >= 0)
	logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: received NACK (crc 0x%04x)",
	    methodName, crc));
    else
	logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: received NACK (invalid CRC returned)",
	    methodName));

    if (cmdQueue.empty())
	return;
    CmdEntry *cmd = cmdQueue.front();
    if (cmd->cmdState == CmdEntry::CS_SENT) {
	cmd->cmdState = CmdEntry::CS_PENDING;
    }
}

/* Check the command queue for commands that have timed out: remove them
 * and let the listener know.
 */
void
BWCloudSensorII::flushCmdQueue(const struct timeval &tv)
{
    static const char methodName[] = "BWCloudSensorII::flushCmdQueue";

    while (!cmdQueue.empty()) {
	CmdEntry *cmd = cmdQueue.front();
	struct timeval timeQueued = timevalSub(tv, cmd->queuedTime);
	if ((cmd->cmdState == CmdEntry::CS_PENDING
		|| cmd->cmdState == CmdEntry::CS_SENT)
	    && timeQueued.tv_sec >= CMD_TIMEOUT_SEC)
	{
	    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC, ("%s: failing command (queued %ld seconds ago, sent %d times, state %s, cmdType %s)",
		    methodName, (long) timeQueued.tv_sec,
		    cmd->nSends, CmdEntry::getCmdStateName(cmd->cmdState),
		    BWCSII_Listener::getCmdTypeName(cmd->cmdType)));
	    cmdQueue.pop_front();
	    cmd->cmdState = CmdEntry::CS_TIMED_OUT;
	    if (listener != NULL)
		listener->csII_cmdOver(this, cmd->cmdType,
		    BWCSII_Listener::COR_TIMED_OUT);
	    delete cmd;
	} else
	    break;
    }
}

/* Calculate the 16 bit CRC for the given data. */
unsigned int
BWCloudSensorII::calcCRC16(const void *bufP, unsigned int nBytes)
{
    /* Table generated using code written by Michael Barr
     * (http://www.netrino.com/Connecting/2000-01/crc.zip).
     */
    static unsigned int crc16Table[] = {
	    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
	};

    const unsigned char *buf = (const unsigned char *) bufP;
    unsigned short crc = 0;
    for (unsigned int i = 0 ; i < nBytes; ++i)
	crc = (unsigned short) ((crc << 8) ^ crc16Table[((crc >> 8) ^ buf[i]) & 0xff]);
    return crc;
}

/* Parse and return 4 hex-digit number; returns -1 if the p doesn't
 * point to 4 hex digits.  The digits may be followed by anything.
 */
int
BWCloudSensorII::parseCRC(const char *p)
{
    if (!isxdigit(p[0]) || !isxdigit(p[1])
	    || !isxdigit(p[2]) || !isxdigit(p[3]))
    {
	return -1;
    }
    return (toHexDigit(p[0]) << 12) | (toHexDigit(p[1]) << 8)
	    | (toHexDigit(p[2]) << 4) | (toHexDigit(p[3]) << 0);
}

/* Returns a - b; */
struct timeval
BWCloudSensorII::timevalSub(const struct timeval &a, const struct timeval &b)
{
    struct timeval tv;
    tv.tv_sec = a.tv_sec - b.tv_sec;
    tv.tv_usec = a.tv_usec - b.tv_usec;
    if (tv.tv_usec < 0) {
	tv.tv_usec += 1000000;
	tv.tv_sec--;
    } else if (tv.tv_usec >= 1000000) {
	tv.tv_usec -= 1000000;
	tv.tv_sec++;
    }
    return tv;
}

const char *
BWCloudSensorII::getReadLoopStateName(ReadLoopState val)
{
    const static EnumNamer::entry_t names[] = {
	    ENUM_NAMER_ENTRY(RLS_NONE),
	    ENUM_NAMER_ENTRY(RLS_READING),
	    ENUM_NAMER_ENTRY(RLS_STOP),
	    ENUM_NAMER_ENTRY(RLS_CLOSE),
	};
    static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
    return namer.getName(val);
}

const char *
BWCloudSensorII::getParseStateName(ParseState val)
{
    const static EnumNamer::entry_t names[] = {
	    ENUM_NAMER_ENTRY(PS_NONE),
	    ENUM_NAMER_ENTRY(PS_IN_PURGE),
	    ENUM_NAMER_ENTRY(PS_SKIP_TO_FRAME_END),
	    ENUM_NAMER_ENTRY(PS_IN_MEMORY_DUMP),
	    ENUM_NAMER_ENTRY(PS_IN_FRAME),
	};
    static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
    return namer.getName(val);
}

const char *
BWCloudSensorII::getParseActionName(ParseAction val)
{
    const static EnumNamer::entry_t names[] = {
	    ENUM_NAMER_ENTRY(PA_NONE),
	    ENUM_NAMER_ENTRY(PA_SEND_ACK),
	    ENUM_NAMER_ENTRY(PA_SEND_NACK),
	    ENUM_NAMER_ENTRY(PA_POLL),
	};
    static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
    return namer.getName(val);
}


/*
 * Class BWCloudSensorII::CmdEntry
 */

BWCloudSensorII::CmdEntry::CmdEntry(BWCSII_Listener::CmdType cmdType,
	const char *cmd)
    : cmdType(cmdType), cmdState(CS_INITIAL),
      nSends(0), cmd(cmd)
{
    timerclear(&queuedTime);
    timerclear(&sentTime);
    timerclear(&ackedTime);
}

void
BWCloudSensorII::CmdEntry::printState(std::ostream &os, int indent,
    int indentIncr) const
{
    os << std::setw(indent) << "" << "BWCloudSensorII::CmdEntry "
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    os << iStr << "cmdType=" << BWCSII_Listener::getCmdTypeName(cmdType)
	<< std::endl;
    os << iStr << "cmdState=" << getCmdStateName(cmdState) << std::endl;
    os << iStr << "nSends=" << nSends << std::endl;
    os << iStr << "cmd=\"" << cmd << '"' << std::endl;
    os << iStr << "queuedTime.tv_sec=" << queuedTime.tv_sec
	<< ", tv_usec=" << queuedTime.tv_usec
	<< " - " << toStringTimeT(queuedTime.tv_sec) << std::endl;
    os << iStr << "sentTime.tv_sec=" << sentTime.tv_sec
	<< ", tv_usec=" << sentTime.tv_usec
	<< " - " << toStringTimeT(sentTime.tv_sec) << std::endl;
    os << iStr << "ackedTime.tv_sec=" << ackedTime.tv_sec
	<< ", tv_usec=" << ackedTime.tv_usec
	<< " - " << toStringTimeT(ackedTime.tv_sec) << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

const char *
BWCloudSensorII::CmdEntry::getCmdStateName(CmdState val)
{
    const static EnumNamer::entry_t names[] = {
	    ENUM_NAMER_ENTRY(CS_INITIAL),
	    ENUM_NAMER_ENTRY(CS_PENDING),
	    ENUM_NAMER_ENTRY(CS_SENT),
	    ENUM_NAMER_ENTRY(CS_DONE),
	    ENUM_NAMER_ENTRY(CS_TIMED_OUT),
	    ENUM_NAMER_ENTRY(CS_PURGED),
	};
    static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
    return namer.getName(val);
}


/*
 * Class CSIILogger
 */

BWCloudSensorII::CSIILogger::CSIILogger(const char *applicationName,
	const char *logDirectory)
    : applicationName(applicationName), logDirectory(logDirectory),
      dailyOut(NULL), longTermOut(NULL),
      dailyDay(0), longTermLastSensorDataTime(0), dailySensorDataCount(0),
      longTermSensorDataCount(0), longTermThresholdsFlag(0),
      lastOpenAttemptTime(0), nDailyLogs(N_DAILY_LOGS)
{
    longTermFileName = logDirectory;
    longTermFileName += "/longtermlog.txt";
}

BWCloudSensorII::CSIILogger::~CSIILogger()
{
    if (dailyOut != NULL || longTermOut != NULL)
	logString("c", std::string(driverName) + " Shut Down");
    if (dailyOut != NULL)
	delete dailyOut;
    if (longTermOut != NULL)
	delete longTermOut;
}

void
BWCloudSensorII::CSIILogger::printState(std::ostream &os, int indent,
	int indentIncr) const
{
    os << std::setw(indent) << "" << "BWCloudSensorII::CSIILogger"
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    os << iStr << "applicationName=\"" << applicationName << '"' << std::endl;
    os << iStr << "logDirectory\"=" << logDirectory << '"' << std::endl;
    os << iStr << "dailyFileName\"=" << dailyFileName << '"' << std::endl;
    os << iStr << "dailyOut=" << dailyOut << std::endl;
    os << iStr << "longTermFileName\"=" << longTermFileName << '"' << std::endl;
    os << iStr << "longTermOut=" << longTermOut << std::endl;
    os << iStr << "dailyDay=" << dailyDay << std::endl;
    os << iStr << "longTermLastSensorDataTime=" << longTermLastSensorDataTime
	<< " - " << toStringTimeT(longTermLastSensorDataTime) << std::endl;
    os << iStr << "dailySensorDataCount=" << dailySensorDataCount << std::endl;
    os << iStr << "longTermSensorDataCount=" << longTermSensorDataCount
	<< std::endl;
    os << iStr << "longTermThresholdsFlag=" << longTermThresholdsFlag
	<< std::endl;
    os << iStr << "lastOpenAttemptTime=" << lastOpenAttemptTime
	<< " - " << toStringTimeT(lastOpenAttemptTime) << std::endl;
    os << iStr << "nDailyLogs=" << nDailyLogs << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

void
BWCloudSensorII::CSIILogger::logString(const char *code, const std::string &str,
	bool logToLongTerm)
{
    struct timeval now;
    struct tm nowTM;
    std::string timeStr;
    gettimeofday(&now, NULL);
    formatTime(now, &nowTM, timeStr);

    checkOpenRotate(now, nowTM, timeStr);

    if (dailyOut != NULL) {
	*dailyOut << timeStr << ' ' << code <<  ' ' << str << std::endl;
    }
    if (longTermOut != NULL && logToLongTerm) {
	*longTermOut << timeStr << ' ' << code << ' ' << str << std::endl;
    }
}

void
BWCloudSensorII::CSIILogger::logReceivedFrame(const struct timeval &readTime,
	const char *p, unsigned int nBytes,
	BWCSII_report::ReportType reportType, const char *reportHeader)
{
    struct tm readTimeTM;
    std::string timeStr;
    formatTime(readTime, &readTimeTM, timeStr);

    checkOpenRotate(readTime, readTimeTM, timeStr);

    if (dailyOut != NULL) {
	bool printHeader = false;
	if (reportType == BWCSII_report::RT_SENSOR_DATA) {
	    if (--dailySensorDataCount <= 0) {
		dailySensorDataCount = SENSOR_DATA_HEADER_COUNT;
		printHeader = true;
	    }
	} else if (reportHeader != NULL) {
	    printHeader = true;
	}
	if (printHeader)
	    *dailyOut << timeStr << " h   " << reportHeader << std::endl;
	*dailyOut << timeStr << " M  ~";
	dailyOut->write(p, nBytes);
	*dailyOut << '~';
	if (reportType == BWCSII_report::RT_THRESHOLDS)
	    *dailyOut << std::setw(5) << driverVersion;
	*dailyOut << std::endl;
	dailyOut->flush();
    }
    if (longTermOut != NULL) {
	bool printHeader = false;
	bool printFrame = false;
	if (reportType == BWCSII_report::RT_SENSOR_DATA) {
	    if (readTime.tv_sec - longTermLastSensorDataTime >=
		LONG_TERM_DATA_INTERVAL_SECS)
	    {
		longTermLastSensorDataTime = readTime.tv_sec;
		if (--longTermSensorDataCount <= 0) {
		    longTermSensorDataCount = SENSOR_DATA_HEADER_COUNT;
		    printHeader = true;
		}
		printFrame = true;
	    }
	} else {
	    /* Report all frames until the second T report is received. */
	    if (reportType == BWCSII_report::RT_THRESHOLDS) {
		if (longTermThresholdsFlag == 0)
		    longTermThresholdsFlag = 1;
		else
		    longTermThresholdsFlag = 2;
	    }
	    if (reportType != BWCSII_report::RT__UNINITIALIZED) {
		if (longTermThresholdsFlag < 2) {
		    printFrame = true;
		    if (reportHeader != NULL)
			printHeader = true;
		}
	    } else {
		/* ??? record all unknown frames? */
		printFrame = true;
		if (reportHeader != NULL)
		    printHeader = true;
	    }
	}
	if (printFrame) {
	    if (printHeader)
		*longTermOut << timeStr << " h   " << reportHeader << std::endl;
	    *longTermOut << timeStr << " M  ~";
	    longTermOut->write(p, nBytes);
	    *longTermOut << '~';
	    if (reportType == BWCSII_report::RT_THRESHOLDS)
		*longTermOut << std::setw(5) << driverVersion;
	    *longTermOut << std::endl;
	    longTermOut ->flush();
	}
    }
}

void
BWCloudSensorII::CSIILogger::setNDailyLogs(int nDailyLogs)
{
    this->nDailyLogs = nDailyLogs;
}

void
BWCloudSensorII::CSIILogger::checkOpenRotate(struct timeval now,
	const struct tm &nowTM, const std::string &timeStr)
{
    static const char methodName[]
	 = "BWCloudSensorII::CSIILogger::checkOpenRotate";

    if (dailyOut != NULL && nowTM.tm_mday != dailyDay) {
	dailyOut->close();
	delete dailyOut;
	dailyOut = NULL;
	lastOpenAttemptTime = 0; /* force a re-open. */
	dailySensorDataCount = 0;
	/*
	 * Remove old logs.
	 */
	if (nDailyLogs > 0) {
	    DIR *dir = opendir(logDirectory.c_str());
	    time_t oldTime = now.tv_sec - (60 * 60 * 24 * nDailyLogs);
	    struct tm *oldTM = localtime(&oldTime);
	    if (oldTM == NULL) {
		logmsg.error("%s: localtime(%ld) failed (oldTime)",
		    methodName, oldTime);
	    } if (dir == NULL) {
		logmsg.error("%s: failed to open log directory %s - %s",
		    methodName, logDirectory.c_str(), strerror(errno));
	    } else {
		struct dirent *de;
		std::string oldStr = toStringFmt("%04d-%02d-%02d.txt",
		    oldTM->tm_year + 1900, oldTM->tm_mon + 1, oldTM->tm_mday);

		while ((de = readdir(dir)) != NULL) {
		    int year, mon, mday, nScanned = 0;
		    if (sscanf(de->d_name, "%4d-%2d-%2d.txt %n",
			    &year, &mon, &mday, &nScanned) >= 3
			&& de->d_name[nScanned] == '\0')
		    {
			logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC,
			    ("%s: checking if old: %s", methodName,
			     de->d_name));
			if (strcmp(de->d_name, oldStr.c_str()) <= 0) {
			    std::string oldName = logDirectory
				+ "/" + de->d_name;
			    logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC,
				("%s: removing old daily log %s", methodName,
				 oldName.c_str()));
			    if (::unlink(oldName.c_str()) < 0) {
				logmsg.error("%s: failed to remove old daily log %s - %s",
				    methodName, oldName.c_str(),
				    strerror(errno));
			    }
			}
		    } else
			logmsg_DEBUG(DBG_PKG_OBS_DEV_MISC,
			    ("%s: ignoring non-daily log: %s", methodName,
			     de->d_name));
		}
		closedir(dir);
	    }
	}
    }

    /* Try opening once an hour. */
    if ((dailyOut == NULL || longTermOut == NULL)
	&& now.tv_sec - lastOpenAttemptTime > 1 * 60 * 60)
    {
	lastOpenAttemptTime = now.tv_sec;
	if (dailyOut == NULL) {
	    dailyDay = nowTM.tm_mday;
	    dailyFileName = toStringFmt("%s/%04d-%02d-%02d.txt",
		logDirectory.c_str(),
		nowTM.tm_year + 1900, nowTM.tm_mon + 1, nowTM.tm_mday);
	    dailyOut = new std::ofstream();
	    dailyOut->open(dailyFileName.c_str(),
		/* Some older g++ libraries don't have ios_base so use ostream
		 * instead.
		 */
		std::ostream::out | std::ostream::app | std::ostream::ate);
	    if (!*dailyOut) {
		logmsg.error("%s: failed to open log file %s - %s",
		    methodName, dailyFileName.c_str(), strerror(errno));
		delete dailyOut;
		dailyOut = NULL;
	    } else {
		printStartUpMessage(dailyOut, timeStr);
		printTimezoneOffset(dailyOut, now, nowTM, timeStr);
	    }
	}
	if (longTermOut == NULL) {
	    longTermOut = new std::ofstream();
	    longTermOut->open(longTermFileName.c_str(),
		/* Some older g++ libraries don't have ios_base so use ostream
		 * instead.
		 */
		std::ostream::out | std::ostream::app | std::ostream::ate);
	    if (!*longTermOut) {
		logmsg.error("%s: failed to open log file %s - %s",
		    methodName, longTermFileName.c_str(), strerror(errno));
		delete longTermOut;
		longTermOut = NULL;
	    } else {
		printStartUpMessage(longTermOut, timeStr);
		printTimezoneOffset(longTermOut, now, nowTM, timeStr);
	    }
	}
    }
}

void
BWCloudSensorII::CSIILogger::printStartUpMessage(std::ostream *out,
	const std::string &timeStr)
{
    *out << std::endl << timeStr << " c "
	<< driverName << " Start Up  Version " << driverVersion
	<< ", application: " << applicationName
	<< std::endl;
}

void
BWCloudSensorII::CSIILogger::printTimezoneOffset(std::ostream *out,
	struct timeval now, const struct tm &nowTM, const std::string &timeStr)
{
    *out << timeStr << " c "
	<< "current GMT offset="
	<< std::setfill('0') << std::setw(2) << (nowTM.tm_gmtoff / (60 * 60))
	<< ':' << std::setw(2) << abs(nowTM.tm_gmtoff % (60 * 60)) / 60
	<< ':' << std::setw(2) << abs(nowTM.tm_gmtoff % 60)
	<< std::setfill(' ') 
	<< ", timezone name="
	<< (nowTM.tm_isdst >= 0 ? tzname[nowTM.tm_isdst] : "?")
	<< ", isdst=" << nowTM.tm_isdst 
	<< std::endl;
}

bool
BWCloudSensorII::CSIILogger::formatTime(struct timeval tv,
	struct tm *tm, std::string &str)
{
    static const char methodName[] = "BWCloudSensorII::CSIILogger::formatTime";

    struct tm *lt = localtime(&tv.tv_sec);
    if (lt == NULL) {
	logmsg.error("%s: localtime(%ld) failed",
	    methodName, (long) tv.tv_sec);
	memset(&tm, sizeof(tm), 0);
	str = toStringFmt("[time=%ld]", tv.tv_sec);
	return false;
    }
    if (tm != NULL)
	*tm = *lt;
    str = toStringFmt("%04d-%02d-%02d %02d:%02d:%02d.%02d",
	lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
	lt->tm_hour, lt->tm_min, lt->tm_sec,
	(int) (tv.tv_usec / 10000));
    return true;
}


/*
 * Class BWCSII_setThresholds
 */

BWCSII_setThresholds::BWCSII_setThresholds()
    : cloudyThresh(0), veryCloudyThresh(0), windyThresh(0),
      rainThresh(0), wetThresh(0)
{
}

void
BWCSII_setThresholds::printState(std::ostream &os,
	int indent, int indentIncr) const
{
    os << std::setw(indent) << "" << "BWCSII_setThresholds"
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    os << iStr << "cloudyThresh=" << cloudyThresh << std::endl;
    os << iStr << "veryCloudyThresh=" << veryCloudyThresh << std::endl;
    os << iStr << "windyThresh=" << windyThresh << std::endl;
    os << iStr << "rainThresh=" << rainThresh << std::endl;
    os << iStr << "wetThresh=" << wetThresh << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}


/*
 * Class BWCSII_Listener
 */

BWCSII_Listener::~BWCSII_Listener()
{
}

void
BWCSII_Listener::csII_purgeEvent(BWCloudSensorII *csII,
	unsigned char reasonChar)
{
}

void
BWCSII_Listener::csII_memoryDump(BWCloudSensorII *csII, const uint8_t *memory,
	unsigned int nBytes)
{
}

void
BWCSII_Listener::csII_cmdOver(BWCloudSensorII *csII, CmdType cmdType,
	CmdOverReason reason)
{
}

void
BWCSII_Listener::csII_polled(BWCloudSensorII *csII)
{
}


void
BWCSII_Listener::csII_reportReset(BWCloudSensorII *csII,
	const BWCSII_reportReset &report)
{
}

void
BWCSII_Listener::csII_reportThresholds(BWCloudSensorII *csII,
	const BWCSII_reportThresholds &report)
{
}

void
BWCSII_Listener::csII_reportWetnessCalibration(BWCloudSensorII *csII,
    const BWCSII_reportWetnessCalibration &report)
{
}

void
BWCSII_Listener::csII_reportWetnessData(BWCloudSensorII *csII,
    const BWCSII_reportWetnessData &report)
{
}

void
BWCSII_Listener::csII_reportThermopileCalibration(BWCloudSensorII *csII,
    const BWCSII_reportThermopileCalibration &report)
{
}

void
BWCSII_Listener::csII_reportSensorData(BWCloudSensorII *csII,
	const BWCSII_reportSensorData &report)
{
}

void
BWCSII_Listener::csII_roofClose(BWCloudSensorII *csII)
{
}

void
BWCSII_Listener::csII_readLooping(BWCloudSensorII *csII)
{
}

const char *
BWCSII_Listener::getCmdTypeName(CmdType val)
{
    const static EnumNamer::entry_t names[] = {
	    ENUM_NAMER_ENTRY(CT__UNINITIALIZED),
	    ENUM_NAMER_ENTRY(CT_GET_CALIBRATION),
	    ENUM_NAMER_ENTRY(CT_GET_WETNESS_CALIBRATION),
	    ENUM_NAMER_ENTRY(CT_SET_THRESHOLDS),
	    ENUM_NAMER_ENTRY(CT_SET_THRESHOLDS_DEFAULT),
	    ENUM_NAMER_ENTRY(CT_ROOF_CLOSE),
	    ENUM_NAMER_ENTRY(CT_RAW),
	};
    static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
    return namer.getName(val);
}

const char *
BWCSII_Listener::getCmdOverReasonName(CmdOverReason val)
{
    const static EnumNamer::entry_t names[] = {
	    ENUM_NAMER_ENTRY(COR_SUCCESS),
	    ENUM_NAMER_ENTRY(COR_TIMED_OUT),
	    ENUM_NAMER_ENTRY(COR_PURGED),
	};
    static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
    return namer.getName(val);
}



/*
 * Class BWCSII_report
 */

BWCSII_report::BWCSII_report(ReportType reportType)
    : reportType(reportType), reportTime(0)
{
}

BWCSII_report::~BWCSII_report()
{
}

void
BWCSII_report::printState(std::ostream &os, int indent, int indentIncr) const
{
    std::string iStr(indent, ' ');
    os << iStr << "reportType=" << getReportTypeName(reportType) << std::endl;
    os << iStr << "reportTime=" << reportTime
	<< " - " << toStringTimeT(reportTime) << std::endl;
}

const char *
BWCSII_report::getReportTypeName(ReportType val)
{
    const static EnumNamer::entry_t names[] = {
	    ENUM_NAMER_ENTRY(RT__UNINITIALIZED),
	    ENUM_NAMER_ENTRY(RT_RESET),
	    ENUM_NAMER_ENTRY(RT_THRESHOLDS),
	    ENUM_NAMER_ENTRY(RT_WETNESS_CALIBRATION),
	    ENUM_NAMER_ENTRY(RT_WETNESS_DATA),
	    ENUM_NAMER_ENTRY(RT_THERMOPILE_CALIBRATION),
	    ENUM_NAMER_ENTRY(RT_SENSOR_DATA),
	};
    static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
    return namer.getName(val);
}


/*
 * Class BWCSII_reportThresholds
 */

BWCSII_reportThresholds::BWCSII_reportThresholds()
    : BWCSII_report(RT_THRESHOLDS), serialNumber(0), version(0), eSendErrs(0),
      eCloudyThresh(0), eVeryCloudyThresh(0), eWindyThresh(0),
      eVeryWindyThresh(0), eRainThresh(0), eWetThresh(0), eDaylightCode(0),
      eDayThresh(0), eVeryDayThresh(0)
{
}

BWCSII_reportThresholds::~BWCSII_reportThresholds()
{
}

void
BWCSII_reportThresholds::printState(std::ostream &os,
	int indent, int indentIncr) const
{
    os << std::setw(indent) << "" << "BWCSII_reportThresholds "
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    BWCSII_report::printState(os, indent, indentIncr);
    os << iStr << "serialNumber=" << serialNumber << std::endl;
    os << iStr << "version=" << version << std::endl;
    os << iStr << "eSendErrs=" << eSendErrs << std::endl;
    os << iStr << "eCloudyThresh=" << eCloudyThresh << std::endl;
    os << iStr << "eVeryCloudyThresh=" << eVeryCloudyThresh << std::endl;
    os << iStr << "eWindyThresh=" << eWindyThresh << std::endl;
    os << iStr << "eVeryWindyThresh=" << eVeryWindyThresh << std::endl;
    os << iStr << "eRainThresh=" << eRainThresh << std::endl;
    os << iStr << "eWetThresh=" << eWetThresh << std::endl;
    os << iStr << "eDaylightCode=" << eDaylightCode << std::endl;
    os << iStr << "eDayThresh=" << eDayThresh << std::endl;
    os << iStr << "eVeryDayThresh=" << eVeryDayThresh << std::endl;

    os << iStr << "unknown1=" << unknown1 << std::endl;
    os << iStr << "unknown2=" << unknown2 << std::endl;
    os << iStr << "unknown3=" << unknown3 << std::endl;
    os << iStr << "unknown4=" << unknown4 << std::endl;
    os << iStr << "unknown5=" << unknown5 << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

bool
BWCSII_reportThresholds::parseReport(const char *callersName,
	const struct timeval &tv, const char *p)
{
    unsigned int nChars = 0;
    int nScanned;
    if ((nScanned = sscanf(p,
	  "T %u %u %u %lf %lf %lf %lf %d %d %d %d %d  %d %d %d %d %d %n",
	    &serialNumber, &version, &eSendErrs,
	    &eCloudyThresh, &eVeryCloudyThresh, &eWindyThresh,
		&eVeryWindyThresh,
	    &eRainThresh, &eWetThresh, &eDaylightCode, &eDayThresh,
		&eVeryDayThresh,
	    &unknown1, &unknown2, &unknown3, &unknown4, &unknown5,
	    &nChars))
	< 12)
    {
	logmsg.error("%s: unrecognized %c report (scanned %d): <%s>",
	    callersName, (int) p[0], nScanned, toStringDump(p).c_str());
	return false;
    }
    if (p[nChars])
	logmsg.warning("%s: unexpected chars at end of %c report: <%s>",
	    callersName, (int) p[0], toStringDump(p + nChars).c_str());
    reportTime = tv.tv_sec;
    return true;
}

void
BWCSII_reportThresholds::reportToString(char *buf, size_t bufSize) const
{
    snprintf(buf, bufSize, 
	"T %05u %05u %05u %5.1f %5.1f %5.1f  %5.1f %5d %5d %3d %5d %5d",
	    serialNumber, version, eSendErrs,
	    eCloudyThresh, eVeryCloudyThresh, eWindyThresh,
		eVeryWindyThresh,
	    eRainThresh, eWetThresh, eDaylightCode, eDayThresh,
		eVeryDayThresh
	    // , unknown1, unknown2, unknown3, unknown4, unknown5
	);
}

const char *
BWCSII_reportThresholds::reportHeader() const
{
    /* ClarV is ClarityII version. */
    return "T  Ser# Firmw Serrs  Cldy VCldy Windy VWindy  Rain   Wet  DC   Day  Vday  Ty SNBot SNTop EV WCC  lindV";
}


/*
 * Class BWCSII_reportReset
 */

BWCSII_reportReset::BWCSII_reportReset()
    : BWCSII_report(RT_RESET), serialNumber(0), version(0), mcucsrv(0),
      crashCode(0), lastCyc(0), eSendErrs(0)
{
}

BWCSII_reportReset::~BWCSII_reportReset()
{
}

void
BWCSII_reportReset::printState(std::ostream &os,
	int indent, int indentIncr) const
{
    os << std::setw(indent) << "" << "BWCSII_reportReset "
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    BWCSII_report::printState(os, indent, indentIncr);
    os << iStr << "serialNumber=" << serialNumber << std::endl;
    os << iStr << "version=" << version << std::endl;
    os << iStr << "mcucsrv=" << mcucsrv << std::endl;
    os << iStr << "crashCode=" << crashCode << std::endl;
    os << iStr << "lastCyc=" << lastCyc << std::endl;
    os << iStr << "eSendErrs=" << eSendErrs << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

bool
BWCSII_reportReset::parseReport(const char *callersName,
	const struct timeval &tv, const char *p)
{
    unsigned int nChars = 0;
    int nScanned;
    if ((nScanned = sscanf(p, "I %u %u 0x%x %u 0x%x %u %n",
	  &serialNumber, &version, &mcucsrv, &crashCode, &lastCyc, &eSendErrs,
	  &nChars)) < 6)
    {
	logmsg.error("%s: unrecognized %c report (scanned %d): <%s>",
	    callersName, (int) p[0], nScanned, toStringDump(p).c_str());
	return false;
    }
    if (p[nChars])
	logmsg.warning("%s: unexpected chars at end of %c report: <%s>",
	    callersName, (int) p[0], toStringDump(p + nChars).c_str());
    reportTime = tv.tv_sec;
    return true;
}

void
BWCSII_reportReset::reportToString(char *buf, size_t bufSize) const
{
    snprintf(buf, bufSize, 
	"I %05u %05u 0x%02x %03u 0x%02x %05u",
	  serialNumber, version, mcucsrv, crashCode, lastCyc, eSendErrs);
}

const char *
BWCSII_reportReset::reportHeader() const
{
    return "I  Ser# Firmw MCUC Cra Lcyc Serrs";
}


/*
 * Class BWCSII_reportWetnessCalibration
 */

BWCSII_reportWetnessCalibration::BWCSII_reportWetnessCalibration()
    : BWCSII_report(RT_WETNESS_CALIBRATION), eWetCal(0), eWetOscFactor(0),
      eRawWetAvg(0), eCaseT(0), eshtAmbientT(0), enomOsc(0),
      oscDry(0), minWetAvg(0), dif(0), unknown1(0)
{
}

BWCSII_reportWetnessCalibration::~BWCSII_reportWetnessCalibration()
{
}

void
BWCSII_reportWetnessCalibration::printState(std::ostream &os,
	int indent, int indentIncr) const
{
    os << std::setw(indent) << "" << "BWCSII_reportWetnessCalibration "
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    BWCSII_report::printState(os, indent, indentIncr);
    os << iStr << "eWetCal=" << eWetCal << std::endl;
    os << iStr << "eWetOscFactor=" << eWetOscFactor << std::endl;
    os << iStr << "eRawWetAvg=" << eRawWetAvg << std::endl;
    os << iStr << "eCaseT=" << eCaseT << std::endl;
    os << iStr << "eshtAmbientT=" << eshtAmbientT << std::endl;
    os << iStr << "enomOsc=" << enomOsc << std::endl;
    os << iStr << "oscDry=" << oscDry << std::endl;
    os << iStr << "minWetAvg=" << minWetAvg << std::endl;
    os << iStr << "dif=" << dif << std::endl;
    os << iStr << "unknown1=" << toStringDump(&unknown1, 1) << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

bool
BWCSII_reportWetnessCalibration::parseReport(const char *callersName,
	const struct timeval &tv, const char *p)
{
    unsigned int nChars = 0;
    int nScanned;
    /* Note: see below for more (optional)... */
    if ((nScanned = sscanf(p, "K %d %lf %u %lf %lf %u %d %d %n",
	    &eWetCal, &eWetOscFactor, &eRawWetAvg, &eCaseT, &eshtAmbientT,
	    &enomOsc, &oscDry, &minWetAvg, &nChars)) < 6)
    {
	logmsg.error("%s: unrecognized %c report (scanned %d): <%s>",
	    callersName, (int) p[0], nScanned, toStringDump(p).c_str());
	return false;
    }
    if (p[nChars]) {
	const char *pp = p + nChars;
	unsigned int nc2 = 0;
	int ns2;
	if ((ns2 = sscanf(pp, "%d %c %n", &dif, &unknown1, &nc2)) < 2) {
	    logmsg.warning(
		"%s: unexpected chars at end of %c report (nScanned=%d): <%s>",
		callersName, (int) p[0], ns2, toStringDump(pp).c_str());
	} else if (pp[nc2])
	    logmsg.warning("%s: unexpected chars at end of %c report: <%s>",
		callersName, (int) p[0], toStringDump(pp + nc2).c_str());
    }
    reportTime = tv.tv_sec;
    return true;
}

void
BWCSII_reportWetnessCalibration::reportToString(char *buf, size_t bufSize) const
{
    snprintf(buf, bufSize, 
	"K %1d %5.3f %5u %4.1f %4.1f %5u %6d %6d %3d",
	    eWetCal, eWetOscFactor, eRawWetAvg, eCaseT, eshtAmbientT,
	    enomOsc, oscDry, minWetAvg, dif
	    //, toStringDump(&unknown1, 1).c_str()
	    );
}

const char *
BWCSII_reportWetnessCalibration::reportHeader() const
{
    return "K V Factr Rawav Case Ambt Nomos Oscdry Minwav Dif";
}


/*
 * Class BWCSII_reportWetnessData
 */

BWCSII_reportWetnessData::BWCSII_reportWetnessData()
    : BWCSII_report(RT_WETNESS_DATA), caseVal(0), ambT(0),
      wAvgW(0), wAvgC(0), nomos(0), rawWT(0), wetAvg(0)
{
}

BWCSII_reportWetnessData::~BWCSII_reportWetnessData()
{
}

void
BWCSII_reportWetnessData::printState(std::ostream &os,
	int indent, int indentIncr) const
{
    os << std::setw(indent) << "" << "BWCSII_reportWetnessData "
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    BWCSII_report::printState(os, indent, indentIncr);
    os << iStr << "caseVal=" << caseVal << std::endl;
    os << iStr << "ambT=" << ambT << std::endl;
    os << iStr << "wAvgW=" << wAvgW << std::endl;
    os << iStr << "wAvgC=" << wAvgC << std::endl;
    os << iStr << "nomos=" << nomos << std::endl;
    os << iStr << "rawWT=" << rawWT << std::endl;
    os << iStr << "wetAvg=" << wetAvg << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

bool
BWCSII_reportWetnessData::parseReport(const char *callersName,
	const struct timeval &tv, const char *p)
{
    unsigned int nChars = 0;
    int nScanned;
    if ((nScanned = sscanf(p, "W %lf %lf %u %u %u %u %u %n",
	    &caseVal, &ambT, &wAvgW, &wAvgC, &nomos, &rawWT, &wetAvg,
	    &nChars)) < 6)
    {
	logmsg.error("%s: unrecognized %c report (scanned %d): <%s>",
	    callersName, (int) p[0], nScanned, toStringDump(p).c_str());
	return false;
    }
    if (p[nChars])
	logmsg.warning("%s: unexpected chars at end of %c report: <%s>",
	    callersName, (int) p[0], toStringDump(p + nChars).c_str());
    reportTime = tv.tv_sec;
    return true;
}

void
BWCSII_reportWetnessData::reportToString(char *buf, size_t bufSize) const
{
    snprintf(buf, bufSize, 
	"W %4.1f %4.1f %5u %5u %5u %5u %5u",
	caseVal, ambT, wAvgW, wAvgC, nomos, rawWT, wetAvg);
}

const char *
BWCSII_reportWetnessData::reportHeader() const
{
    return "W Case Ambt Wavgw Wavgc Nomos Rawwt Wetav";
}


/*
 * Class BWCSII_reportThermopileCalibration
 */

BWCSII_reportThermopileCalibration::BWCSII_reportThermopileCalibration()
    : BWCSII_report(RT_THERMOPILE_CALIBRATION), eThermopileCal(0), eBestK(0),
      eBestD(0), eBestOffs(0)
{
}

BWCSII_reportThermopileCalibration::~BWCSII_reportThermopileCalibration()
{
}

void
BWCSII_reportThermopileCalibration::printState(std::ostream &os,
	int indent, int indentIncr) const
{
    os << std::setw(indent) << ""
	<< "BWCSII_reportThermopileCalibration "
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    BWCSII_report::printState(os, indent, indentIncr);
    os << iStr << "eThermopileCal=" << eThermopileCal << std::endl;
    os << iStr << "eBestK=" << eBestK << std::endl;
    os << iStr << "eBestD=" << eBestD << std::endl;
    os << iStr << "eBestOffs=" << eBestOffs << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

bool
BWCSII_reportThermopileCalibration::parseReport(const char *callersName,
	const struct timeval &tv, const char *p)
{
    unsigned int nChars = 0;
    int nScanned;
    if ((nScanned = sscanf(p, "C %d %lf %lf %lf %n",
	    &eThermopileCal, &eBestK, &eBestD, &eBestOffs,
	    &nChars)) < 4)
    {
	logmsg.error("%s: unrecognized %c report (scanned %d): <%s>",
	    callersName, (int) p[0], nScanned, toStringDump(p).c_str());
	return false;
    }
    if (p[nChars])
	logmsg.warning("%s: unexpected chars at end of %c report: <%s>",
	    callersName, (int) p[0], toStringDump(p + nChars).c_str());
    reportTime = tv.tv_sec;
    return true;
}

void
BWCSII_reportThermopileCalibration::reportToString(char *buf, size_t bufSize)
    const
{
    snprintf(buf, bufSize, 
	"C %1d %10.3e %5.3f %5.2f",
	    eThermopileCal, eBestK, eBestD, eBestOffs);
}

const char *
BWCSII_reportThermopileCalibration::reportHeader() const
{
    return "C V          K     d  offs";
}


/*
 * Class BWCSII_reportSensorData
 */

BWCSII_reportSensorData::BWCSII_reportSensorData()
    : BWCSII_report(RT_SENSOR_DATA), humidstatTempCode(0), cloudCond(0),
      windCond(0), rainCond(0), skyCond(0), roofCloseRequested(0),
      skyMinusAmbientTemperature(0), ambientTemperature(0), windSpeed(0),
      wetSensor(0), rainSensor(0), relativeHumidityPercentage(0),
      dewPointTemperature(0), caseTemperature(0), rainHeaterPercentage(0),
      blackBodyTemperature(0), rainHeaterState(0), powerVoltage(0),
      anemometerTemeratureDiff(0), wetnessDrop(0), wetnessAvg(0),
      wetnessDry(0), rainHeaterPWM(0), anemometerHeaterPWM(0),
      thermopileADC(0), thermistorADC(0), powerADC(0), blockADC(0),
      anemometerThermistorADC(0), davisVaneADC(0), dkMPH(0),
      extAnemometerDirection(0), rawWetnessOsc(0), dayCond(0),
      daylightADC(0), recentlyWet(false)
{
}

BWCSII_reportSensorData::~BWCSII_reportSensorData()
{
}

void
BWCSII_reportSensorData::printState(std::ostream &os,
	int indent, int indentIncr) const
{
    os << std::setw(indent) << "" << "BWCSII_reportSensorData "
	<< this << " {" << std::endl;
    indent += indentIncr;

    std::string iStr(indent, ' ');

    BWCSII_report::printState(os, indent, indentIncr);
    os << iStr << "humidstatTempCode=" << humidstatTempCode << std::endl;
    os << iStr << "cloudCond=" << cloudCond << std::endl;
    os << iStr << "windCond=" << windCond << std::endl;
    os << iStr << "rainCond=" << rainCond << std::endl;
    os << iStr << "skyCond=" << skyCond << std::endl;
    os << iStr << "roofCloseRequested=" << roofCloseRequested << std::endl;
    os << iStr << "skyMinusAmbientTemperature=" << skyMinusAmbientTemperature << std::endl;
    os << iStr << "ambientTemperature=" << ambientTemperature << std::endl;
    os << iStr << "windSpeed=" << windSpeed << std::endl;
    os << iStr << "wetSensor=" << wetSensor << std::endl;
    os << iStr << "rainSensor=" << rainSensor << std::endl;
    os << iStr << "relativeHumidityPercentage=" << relativeHumidityPercentage << std::endl;
    os << iStr << "dewPointTemperature=" << dewPointTemperature << std::endl;
    os << iStr << "caseTemperature=" << caseTemperature << std::endl;
    os << iStr << "rainHeaterPercentage=" << rainHeaterPercentage << std::endl;
    os << iStr << "blackBodyTemperature=" << blackBodyTemperature << std::endl;
    os << iStr << "rainHeaterState=" << rainHeaterState << std::endl;
    os << iStr << "powerVoltage=" << powerVoltage << std::endl;
    os << iStr << "anemometerTemeratureDiff=" << anemometerTemeratureDiff << std::endl;
    os << iStr << "wetnessDrop=" << wetnessDrop << std::endl;
    os << iStr << "wetnessAvg=" << wetnessAvg << std::endl;
    os << iStr << "wetnessDry=" << wetnessDry << std::endl;
    os << iStr << "rainHeaterPWM=" << rainHeaterPWM << std::endl;
    os << iStr << "anemometerHeaterPWM=" << anemometerHeaterPWM << std::endl;
    os << iStr << "thermopileADC=" << thermopileADC << std::endl;
    os << iStr << "thermistorADC=" << thermistorADC << std::endl;
    os << iStr << "powerADC=" << powerADC << std::endl;
    os << iStr << "blockADC=" << blockADC << std::endl;
    os << iStr << "anemometerThermistorADC=" << anemometerThermistorADC << std::endl;
    os << iStr << "davisVaneADC=" << davisVaneADC << std::endl;
    os << iStr << "dkMPH=" << dkMPH << std::endl;
    os << iStr << "extAnemometerDirection=" << extAnemometerDirection << std::endl;
    os << iStr << "rawWetnessOsc=" << rawWetnessOsc << std::endl;
    os << iStr << "dayCond=" << dayCond << std::endl;
    os << iStr << "daylightADC=" << daylightADC << std::endl;
    os << iStr << "recentlyWet=" << recentlyWet << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

bool
BWCSII_reportSensorData::parseReport(const char *callersName,
	const struct timeval &tv, const char *p)
{
    unsigned int nChars = 0;
    int nScanned;
    if ((nScanned = sscanf(p, "D %u %u %u %u %u %u %lf %lf %lf %c %c %d %lf %lf %d %lf %d %lf %lf %d %d %d %d %d %u %u %u %u %u %u %lf %d %d %u %u %n",
	    &humidstatTempCode, &cloudCond,
	    &windCond, &rainCond, &skyCond, &roofCloseRequested,
	    &skyMinusAmbientTemperature, &ambientTemperature, &windSpeed,
	    &wetSensor, &rainSensor, &relativeHumidityPercentage,
	    &dewPointTemperature, &caseTemperature, &rainHeaterPercentage,
	    &blackBodyTemperature, &rainHeaterState, &powerVoltage,
	    &anemometerTemeratureDiff, &wetnessDrop, &wetnessAvg,
	    &wetnessDry, &rainHeaterPWM, &anemometerHeaterPWM,
	    &thermopileADC, &thermistorADC, &powerADC, &blockADC,
	    &anemometerThermistorADC, &davisVaneADC, &dkMPH,
	    &extAnemometerDirection, &rawWetnessOsc, &dayCond,
	    &daylightADC,
	    &nChars)) < 35)
    {
	logmsg.error("%s: unrecognized %c report (scanned %d): <%s>",
	    callersName, (int) p[0], nScanned, toStringDump(p).c_str());
	return false;
    }
    if (p[nChars])
	logmsg.warning("%s: unexpected chars at end of %c report: <%s>",
	    callersName, (int) p[0], toStringDump(p + nChars).c_str());
    reportTime = tv.tv_sec;
    return true;
}

void
BWCSII_reportSensorData::reportToString(char *buf, size_t bufSize) const
{
    snprintf(buf, bufSize, 
	"D %1u %1u %1u %1u %1u %1u %6.1f %5.1f %5.1f %c %c %3d %5.1f %5.1f %3d %5.1f %1d %4.1f %5.1f %5d %5d %5d %3d %3d %4u %4u %4u %4u %4u %5u %4.1f %5d %3d %3u %2u",
	    humidstatTempCode, cloudCond,
	    windCond, rainCond, skyCond, roofCloseRequested,
	    skyMinusAmbientTemperature, ambientTemperature, windSpeed,
	    wetSensor, rainSensor, relativeHumidityPercentage,
	    dewPointTemperature, caseTemperature, rainHeaterPercentage,
	    blackBodyTemperature, rainHeaterState, powerVoltage,
	    anemometerTemeratureDiff, wetnessDrop, wetnessAvg,
	    wetnessDry, rainHeaterPWM, anemometerHeaterPWM,
	    thermopileADC, thermistorADC, powerADC, blockADC,
	    anemometerThermistorADC, davisVaneADC, dkMPH,
	    extAnemometerDirection, rawWetnessOsc, dayCond,
	    daylightADC);
}

const char *
BWCSII_reportSensorData::reportHeader() const
{
    return "D E C W R 1 c    SKY   AMB  WIND w r HUM   DEW  CASE HEA  BLKT H  PWR WNDTD WDROP  WAVG  WDRY RHT AHT ASKY ACSE APSV ABLK AWND AVNE DKMPH VNE RWOSC  PH CN";
}
