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
#define SYSLOG_NAMES
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <cstring>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>

#include <vector>
#include <algorithm>
#include <map>

#include "util/Log.h"
#include "util/parseInt.h"
#include "util/MTRoot.h"
#include "util/FileDesc.h"
#include "util/toString.h"
#include "util/getThreadID.h"

#define B_SIZE	(8 * 1024)
#define NELEM(a) (sizeof(a) / sizeof((a)[0]))

static void getPriorityName(char *buf, int bsize, int priority,
    const Log::DebugPkgInfo *dpi, bool useColor);
static int getPriorityByName(const char *name);
static const char *getPriorityName(int priority);
static int getFacilityByName(const char *name);
static const char *getFacilityName(int facility);

class LogDst
{
  public:
    enum LDType {
	LDT_UNINITIALIZED = 0,
	LDT_SYSLOG,
	LDT_STDERR,
	LDT_FILE,
    };
    LogDst(LDType ldType, int minPriority, bool logTime)
	: ldType(ldType), minPriority(minPriority), logTime(logTime),
	  useCount(0)
    {
    }
    virtual ~LogDst()
    {
    }

    virtual std::string toString() const = 0;
    virtual bool open(const char *progName, unsigned int progID,
	    bool haveForked) = 0;
    /* Is given log dst the same (ignoring some params - see setParams_()). */
    virtual bool mostlyEqual(const LogDst *a) const = 0;
    /* Set those params not counted as different by mostlyEqual(). */
    virtual void setParams_(const char *progName, unsigned int progID,
	    const LogDst *a) = 0;
    virtual LogDst *clone() const = 0;
    virtual void doLog(const char *progName, unsigned int progID,
	    const char *timeStr, int priority,
	    const Log::DebugPkgInfo *dpi, const char *msg) = 0;
    virtual void afterFork() = 0;
    virtual void pseudoClose() = 0;

    void setParams(const char *progName, unsigned int progID,
	    const LogDst *logDst)
    {
	setMinPriority(logDst->getMinPriority());
	setLogTime(logDst->getLogTime());
	setParams_(progName, progID, logDst);
    }

    int getMinPriority() const {
	return minPriority;
    }
    void setMinPriority(int minPriority) {
	this->minPriority = minPriority;
    }

    bool getLogTime() const {
	return logTime;
    }
    void setLogTime(bool logTime) {
        this->logTime = logTime;
    }

    void incInuse() {
	++useCount;
    }

    void decInuse()
    {
	static const char methodName[] = "LogDst::decInuse";

	if (useCount == 0)
	    logmsg.internalError("%s: useCount already 0", methodName);
	else
	    --useCount;
    }

    unsigned int getInuse() const {
	return useCount;
    }

    LDType getType() const {
	return ldType;
    }

  private:
    LDType ldType;
  protected:
    int minPriority;
  private:
    bool logTime;
    unsigned int useCount;
};

class LogDstSyslog : public LogDst
{
  public:
    LogDstSyslog(int minPriority, int logOptions, int logFacility)
	: LogDst(LDT_SYSLOG, minPriority, false), progName(NULL),
	  progNameID(NULL), progID(0),
	  logOptions(logOptions), logFacility(logFacility), opened(false)
    {
    }
    ~LogDstSyslog()
    {
	if (opened)
	    closelog();
	if (progName != NULL)
	    free(progName);
	if (progNameID != NULL)
	    free(progNameID);
    }
    std::string toString() const
    {
	/* Should match format parsed in setDebuggingState(). */
	return toStringFmt("logTo(type=syslog,priority=%s,facility=%s)",
	    getPriorityName(minPriority), getFacilityName(logFacility));
    }
    bool open(const char *progName, unsigned int progID, bool haveForked)
    {
	if (!opened) {
	    setProgName(progName);
	    setProgID(progID);
	    doOpenlog();
	}
	return true;
    }
    bool mostlyEqual(const LogDst *a) const
    {
	if (a->getType() != LogDst::LDT_SYSLOG)
	    return false;
	/* Ignore logOptions/logFacility (set separately). */
	return true;
    }
    void setParams_(const char *progName, unsigned int progID, const LogDst *a)
    {
	if (a->getType() != LogDst::LDT_SYSLOG)
	    return;
	const LogDstSyslog *logDst = static_cast<const LogDstSyslog *>(a);
	setOptionsFacility(progName, progID,
	    logDst->logOptions, logDst->logFacility);
    }
    LogDst *clone() const
    {
	return new LogDstSyslog(minPriority, logOptions, logFacility);
    }
    void doLog(const char *progName, unsigned int progID,
	    const char *timeStr, int priority,
	    const Log::DebugPkgInfo *dpi, const char *msg)
    {
	if (priority > minPriority)
	    return;
	char priorityName[64];
	getPriorityName(priorityName, sizeof(priorityName), priority,
	    dpi, false);
	syslog(logFacility | priority, "%s: %s", priorityName, msg);

    }
    void afterFork()
    {
	/* nothing to do. */
    }
    void pseudoClose()
    {
	/* Can't do anything here? */
    }
    void setOptionsFacility(const char *progName, unsigned int progID,
	int logOptions, int logFacility)
    {
	if (this->progName == NULL || progName == NULL
	    || strcmp(this->progName, progName) != 0
	    || this->progID != progID
	    || this->logOptions != logOptions
	    || this->logFacility != logFacility)
	{
	    setProgName(progName);
	    setProgID(progID);
	    this->logOptions = logOptions;
	    this->logFacility = logFacility;
	    if (opened) {
		closelog();
		doOpenlog();
	    }
	}
    }
  private:
    void setProgName(const char *progName) {
	if (this->progName != NULL)
	    free(this->progName);
	this->progName = strdup(progName != NULL ? progName : "(unknown)");
    }
    void setProgID(unsigned int progID) {
        this->progID = progID;
    }
    void doOpenlog() {
	char *pn = this->progName;
	if (logOptions & MT_LOG_PROG_ID) {
	    if (progNameID != NULL)
		free(progNameID);
	    pn = progNameID = strdup(
		toStringFmt("%s-%d", progName, progID).c_str());
	}
	openlog(pn, logOptions & ~MT_LOG_OPTIONS_MASK, logFacility);
	opened = true;
    }
    char *progName;
    char *progNameID;
    unsigned int progID;
    int logOptions;
    int logFacility;
    bool opened;
};

class LogDstStderr : public LogDst
{
  public:
    enum { DEFAULT_STOP_ON_FORK = 1, DEFAULT_FD = 2, DEFAULT_LOG_TIME = 1 };

    LogDstStderr(int minPriority, bool stopOnFork, int fd = DEFAULT_FD,
	    bool logTime = DEFAULT_LOG_TIME)
	: LogDst(LDT_STDERR, minPriority, logTime), stopOnFork(stopOnFork),
	  closed(false), fd(fd), useColor(isatty(fd))
    {
    }
    ~LogDstStderr()
    {
    }
    std::string toString() const
    {
	/* Should match format parsed in setDebuggingState(). */
	return toStringFmt(
	    "logTo(type=stderr,priority=%s,fd=%d,logTime=%s,stopOnFork=%s)",
	    getPriorityName(minPriority), fd, getLogTime() ? "true" : "false",
	    stopOnFork ? "true" : "false");
    }
    bool open(const char *progName, unsigned int progID, bool haveForked)
    {
	struct stat statb;
	if (fstat(fd, &statb) < 0)
	    return false;
	if (stopOnFork && haveForked)
	    return false;
	return true;
    }
    bool mostlyEqual(const LogDst *a) const
    {
	if (a->getType() != LogDst::LDT_STDERR)
	    return false;
	const LogDstStderr *logDst = static_cast<const LogDstStderr *>(a);
	return fd == logDst->fd;
    }
    void setParams_(const char *progName, unsigned int progID, const LogDst *a)
    {
	if (a->getType() != LogDst::LDT_STDERR)
	    return;
	const LogDstStderr *logDst = static_cast<const LogDstStderr *>(a);
	stopOnFork = logDst->stopOnFork;
    }
    LogDst *clone() const
    {
	return new LogDstStderr(minPriority, stopOnFork, fd, getLogTime());
    }
    void doLog(const char *progName, unsigned int progID,
	    const char *timeStr, int priority,
	    const Log::DebugPkgInfo *dpi, const char *msg)
    {
	if (priority > minPriority || closed)
	    return;

	char priorityName[64];
	getPriorityName(priorityName, sizeof(priorityName), priority, dpi,
	    useColor);

	char buf[B_SIZE];
	snprintf(buf, sizeof(buf), "%s%s[%d]: %s: %s\n",
	    timeStr, progName, getThreadID(), priorityName, msg);
	if (write(fd, buf, strlen(buf)) < 0)
	    ; /* Ignore failures (can't log them...). */
    }
    void afterFork()
    {
	if (stopOnFork)
	    pseudoClose();
    }
    void pseudoClose()
    {
	closed = true;
    }
  private:
    bool stopOnFork;	/* Stop logging here when process forks? */
    bool closed;
    int fd;
    bool useColor;
};

class LogDstFile : public LogDst
{
  public:
    enum { DEFAULT_LOG_TIME = 1 };

    LogDstFile(int minPriority, const std::string &baseName, bool logTime,
	    /* Log file rotation; rotate when rotateLogSize exceeded,
	     * keep up to rotateFileMax files.
	     * rotateLogSize of 0 means don't rotate.
	     * rotateFileMax of 0 means truncate log file when
	     * it exceeds the rotateLogSize.
	     */
	    off_t rotateLogSize, unsigned int rotateFileMax)
	: LogDst(LDT_FILE, minPriority, logTime), baseName(baseName),
	  opened(false), rotateLogSize(rotateLogSize),
	  rotateFileMax(rotateFileMax), logSize(0)
    {
    }
    ~LogDstFile()
    {
    }
    std::string toString() const
    {
	/* Should match format parsed in setDebuggingState(). */
	return toStringFmt(
	    "logTo(type=file,priority=%s,baseName=%s,logTime=%s,rotateLogSize=%ld,rotateFileMax=%d) fileName=%s",
	    getPriorityName(minPriority), baseName.c_str(),
	    getLogTime() ? "true" : "false", rotateLogSize, rotateFileMax,
	    opened ? fileName.c_str() : "");
    }
    bool open(const char *progName, unsigned int progID, bool haveForked)
    {
	static const char methodName[] = "LogDstFile::open";

	if (opened)
	    return true;
	if (baseName.length() == 0) {
	    logmsg.internalError("%s: called with empty basename", methodName);
	    return false;
	}
	fileName = baseName;
	{
	    std::string s;
	    if (progID != 0)
		s = ::toStringFmt("%s-%d", progName, progID);
	    else
		s = progName;
	    replaceAll(fileName, "PROG_NAME_ID", s);
	}
	replaceAll(fileName, "PROG_NAME", progName);
	replaceAll(fileName, "PROG_ID", ::toString(progID));
	if (baseName[0] != '/') {
	    fileName.insert(0, MTROOT_LOG_DIR);
	    fileName.insert(0, getMTRoot());
	}
	if (fileName.find("XXXXXX") != std::string::npos) {
	    /* Not nice, but... */
	    fileName.append(1, '\0');
	    fd.setFD(mkstemp(const_cast<char *>(fileName.data())));
	    if (!fd.isClosed()) {
		int oldFlags = fcntl(fd.getFD(), F_GETFL);
		if (oldFlags < 0)
		    logmsg.error("%s: fcntl(%d,F_GETFL) failed on %s - %s",
			methodName, fd.getFD(),
			fileName.c_str(), strerror(errno));
		else if (fcntl(fd.getFD(), F_SETFL, oldFlags | O_APPEND) < 0)
		    logmsg.error("%s: fcntl(%d,F_SETFL,0x%x) failed on %s - %s",
			methodName, fd.getFD(), oldFlags | O_APPEND,
			fileName.c_str(), strerror(errno));
	    }
	} else {
	    fd.setFD(::open(fileName.c_str(), O_WRONLY|O_CREAT|O_APPEND, 0666));
	    if (!fd.isClosed()) {
		struct stat statb;
		if (fstat(fd.getFD(), &statb) >= 0)
		    logSize = statb.st_size;
	    }
	}
	if (!fd.isClosed()) {
	    if (fcntl(fd.getFD(), F_SETFD, FD_CLOEXEC) < 0)
		logmsg.error("%s: fcntl(%d,F_SETFD) failed on %s - %s",
		    methodName, fd.getFD(), fileName.c_str(), strerror(errno));
	} else {
	    logmsg.error("%s: failed to open `%s' - %s", methodName,
		fileName.c_str(), strerror(errno));
	    return false;
	}
	opened = true;
	return true;
    }
    bool mostlyEqual(const LogDst *a) const
    {
	if (a->getType() != LogDst::LDT_FILE)
	    return false;
	const LogDstFile *logDst = static_cast<const LogDstFile *>(a);
	return baseName == logDst->baseName;
    }
    void setParams_(const char *progName, unsigned int progID, const LogDst *a)
    {
	if (a->getType() != LogDst::LDT_FILE)
	    return;
	const LogDstFile *logDst = static_cast<const LogDstFile *>(a);
	rotateLogSize = logDst->rotateLogSize;
	rotateFileMax = logDst->rotateFileMax;
    }
    LogDst *clone() const
    {
	return new LogDstFile(minPriority, baseName, getLogTime(),
	    rotateLogSize, rotateFileMax);
    }
    void doLog(const char *progName, unsigned int progID,
	    const char *timeStr, int priority,
	    const Log::DebugPkgInfo *dpi, const char *msg)
    {
	static const char methodName[] = "LogDstFile::doLog";

	if (priority > minPriority || !opened)
	    return;

	char priorityName[64];
	getPriorityName(priorityName, sizeof(priorityName), priority, dpi,
	    false);

	char buf[B_SIZE];
	unsigned int len;
	snprintf(buf, sizeof(buf), "%s%s[%d]: %s: %s\n",
	    timeStr, progName, getThreadID(), priorityName, msg);
	if (write(fd.getFD(), buf, len = strlen(buf)) < 0)
	    ; /* Ignore failures (can't log them...). */
	logSize += len; /* XXX assuming is atomic operation. */
	if (rotateLogSize != 0 && logSize > rotateLogSize) {
	    logSize = 0; /* XXX assuming is atomic operation. */
	    if (rotateFileMax == 0) {
		if (lseek(fd.getFD(), 0, SEEK_SET) < 0)
		    ; /* Ignore failures (can't log them...). */
		if (ftruncate(fd.getFD(), 0) < 0)
		    ; /* Ignore failures (can't log them...). */
	    } else { /* rotate... */
		char srcFileName[fileName.size() + 16];
		char dstFileName[fileName.size() + 16];

		snprintf(buf, sizeof(buf),
		    "%s%s[%d]: %s: [end of logfile - rotating]\n",
		    timeStr, progName, getThreadID(), priorityName);
		if (write(fd.getFD(), buf, len = strlen(buf)) < 0)
		    ; /* Ignore failures (can't log them...). */
		struct stat prevStatb, newStatb;
		bool prevStatbValid = false;
		if (fstat(fd.getFD(), &prevStatb) >= 0)
		    prevStatbValid = true;
		/* Little dance to ensure fd is always valid (other threads
		 * may be logging).
		 */
		snprintf(dstFileName, sizeof(dstFileName),
		    "%s.0", fileName.c_str());
		if (rename(fileName.c_str(), dstFileName) < 0)
		    ; /* Ignore failures (can't log them...). */
		int oldFD = fd.release();
		fd.setFD(::open(fileName.c_str(), O_WRONLY|O_CREAT|O_APPEND,
		    0666));
		::close(oldFD);
		for (int i = rotateFileMax - 1; i >= 0; --i) {
		    snprintf(srcFileName, sizeof(srcFileName),
			"%s.%d", fileName.c_str(), i - 1);
		    snprintf(dstFileName, sizeof(dstFileName),
			"%s.%d", fileName.c_str(), i);
		    if (rename(srcFileName, dstFileName) < 0)
			; /* Ignore failures (can't log them...). */
		}
		if (fd.getFD() >= 0) {
		    if (fcntl(fd.getFD(), F_SETFD, FD_CLOEXEC) < 0) {
			logmsg.error("%s: fcntl(%d,F_SETFD) failed on %s - %s",
			    methodName, fd.getFD(), fileName.c_str(),
			    strerror(errno));
		    }
		    /* Attempt to restore permissions. */
		    if (prevStatbValid && fstat(fd.getFD(), &newStatb) >= 0) {
			if (fchown(fd.getFD(), prevStatb.st_uid,
				prevStatb.st_gid) < 0)
			    logmsg.warning(
				"%s: fchown(%d,%d,%d) failed on %s - %s",
				methodName, fd.getFD(),
				prevStatb.st_uid, prevStatb.st_gid,
				fileName.c_str(), strerror(errno));
			if (fchmod(fd.getFD(), prevStatb.st_mode & 0777) < 0)
			    logmsg.warning(
				"%s: fchmod(%d,%#o) failed on %s - %s",
				methodName, fd.getFD(),
				prevStatb.st_mode & 0777, fileName.c_str(),
				strerror(errno));
		    }
		} else {
		    /* Ignore failures (can't log them...). */
		}
	    }
	    snprintf(buf, sizeof(buf), "%s%s[%d]: notice: log file rotated (rotateLogSize=%ld,rotateFileMax=%d)\n",
		timeStr, progName, getThreadID(), rotateLogSize, rotateFileMax);
	    if (write(fd.getFD(), buf, len = strlen(buf)) < 0)
		; /* Ignore failures (can't log them...). */
	    logSize += len; /* XXX assuming is atomic operation. */
	}
    }
    void afterFork()
    {
	/* nothing to do. */
    }
    void pseudoClose()
    {
	/* Want to close the fd without closing it to avoid the situation
	 * where a thread is (about to) logging something and another thread
	 * is opening a file - if we close, the fd could be re-used with
	 * the not-good result of log messages going to the newly opened thing.
	 */
	if (!fd.isClosed()) {
	    int f = ::open("/dev/null", O_WRONLY, 0);
	    if (f >= 0) {
		/* if this fails, we are stuck with the log file open. */
		dup2(f, fd.getFD());
		close(f);
	    }
	}
    }
  private:
    static void replaceAll(std::string &dst, const std::string &pat,
	    const std::string &replaceStr)
    {
	std::string::size_type pos = 0;
	while ((pos = dst.find(pat, pos)) != std::string::npos) {
	    if (isalnum(dst[pos + pat.length()])) {
		++pos;
	    } else {
		dst.replace(pos, pat.length(), replaceStr);
		pos += replaceStr.length();
	    }
	}
    }
  private:
    std::string baseName;
    std::string fileName;
    FileDesc fd;
    bool opened;
    off_t rotateLogSize;
    unsigned int rotateFileMax;
    off_t logSize;
};


bool Log::noZeroCoreWarning;
Log logmsg;


/*
 * Class Log
 */

void
Log::initPkgInfo()
{
    /* In here to ensure it is constructed when its needed. */
    static DebugPkgInfo libDebugPkgInfos[] = {
	DebugPkgInfo(DBG_PKG_UTIL_SERVER_CONTROLLER, "ServerController"),
	DebugPkgInfo(DBG_PKG_UTIL_SERVER_CONTROLLER_V, "ServerControllerV"),
	DebugPkgInfo(DBG_PKG_UTIL_PTMUTEX, "PTMutex"),
	DebugPkgInfo(DBG_PKG_UTIL_PTCONDITION, "PTCondition"),
	DebugPkgInfo(DBG_PKG_UTIL_CHILD_SERVICE, "ChildService"),
	DebugPkgInfo(DBG_PKG_UTIL_SIGNAL_MANAGER, "SignalManager"),
	DebugPkgInfo(DBG_PKG_UTIL_SELECTOR, "Selector"),
	DebugPkgInfo(DBG_PKG_UTIL_CONTROLLER_WORK, "ControllerWork"),
	DebugPkgInfo(DBG_PKG_UTIL_NLS, "NLS"),
	DebugPkgInfo(DBG_PKG_UTIL_TWORKER, "TWorker"),
	DebugPkgInfo(DBG_PKG_UTIL_TWORKER_V, "TWorkerV"),
	DebugPkgInfo(DBG_PKG_UTIL_SERVICE_CONNECTION, "ServiceConn"),
	DebugPkgInfo(DBG_PKG_UTIL_MISC, "UtilMisc"),
	DebugPkgInfo(DBG_PKG_FLAG_NOFORK, "noFork"),
	DebugPkgInfo(DBG_PKG_FLAG___UNUSED1, "__unused1"),
	DebugPkgInfo(DBG_PKG_FLAG_NOCHDIR, "noChdir"),
	DebugPkgInfo(DBG_PKG_MSG_STREAM, "Stream"),
	DebugPkgInfo(DBG_PKG_MSG_SEND_RCV, "sendRcv"),
	DebugPkgInfo(DBG_PKG_MSG_SEND_RCV_V, "sendRcvV"),
	DebugPkgInfo(DBG_PKG_SURVEYDB, "surveyDB"),
	DebugPkgInfo(DBG_PKG_SURVEYDB_V, "surveyDBV"),
	DebugPkgInfo(DBG_PKG_DB, "DB"),
	DebugPkgInfo(DBG_PKG_DB_V, "DBV"),
	DebugPkgInfo(DBG_PKG_CLIENT_WAREHOUSE, "clientWarehouse"),
	DebugPkgInfo(DBG_PKG_CLIENT_WSM, "clientWSM"),
	DebugPkgInfo(DBG_PKG_CLIENT_OBS_STN, "clientObsStn"),
	DebugPkgInfo(DBG_PKG_CLIENT_IMAGE_CACHE, "clientImageCache"),
	DebugPkgInfo(DBG_PKG_CLIENT_NOTIFIER, "clientNotifier"),
	DebugPkgInfo(DBG_PKG_CLIENT_NFM, "NtfMgr"),
	DebugPkgInfo(DBG_PKG_CLIENT_NSM, "clientNSM"),
	DebugPkgInfo(DBG_PKG_CLIENT_SCHEDULER, "clientSch"),
	DebugPkgInfo(DBG_PKG_SURVEY_SCHEDULER, "surveyScheduler"),
	DebugPkgInfo(DBG_PKG_OBS_DEV_TELPOS, "obsDevTelPos"),
	DebugPkgInfo(DBG_PKG_OBS_DEV_CAMERA, "obsDevCam"),
	DebugPkgInfo(DBG_PKG_OBS_DEV_MISC, "obsDevMisc"),
	DebugPkgInfo(DBG_PKG_OBS_DEV_MISC_V, "obsDevMiscV"),
	DebugPkgInfo(DBG_PKG_IMAGE, "image"),
	DebugPkgInfo(DBG_PKG_CLIENT_BE, "clientBE"),
    };
    static bool inited = false;
    if (!inited) {
	inited = true;
	for (unsigned int i = 0; i < NELEM(debugPkgInfo); ++i) {
	    debugPkgInfo[i].debugPkg = i;
	    debugPkgInfo[i].name = NULL;
	}
    }
    addDebugPkgInfo(libDebugPkgInfos, NELEM(libDebugPkgInfos));
}

Log::Log()
    : logOptions(DEFAULT_LOG_OPTIONS), logFacility(DEFAULT_LOG_FACILITY),
      isDefault(true), haveForked(false), logTime(false),
      progName(NULL), progID(0), logTag(NULL),
      logDsts(new LogDsts()), oldLogDsts(NULL)
{
    initPkgInfo();
    setLogDsts(defaultLogDst(), false);
}

Log::Log(const char *progName, const char *debugStateStr)
    : logOptions(DEFAULT_LOG_OPTIONS), logFacility(DEFAULT_LOG_FACILITY),
      isDefault(false), haveForked(false), logTime(false),
      progName(NULL), progID(0), logTag(NULL),
      logDsts(new LogDsts()), oldLogDsts(NULL)
{
    initPkgInfo();
    setProgName(progName);
    setDebuggingState(debugStateStr);
}

Log::Log(const char *progName, unsigned int progID, const char *debugStateStr)
    : logOptions(DEFAULT_LOG_OPTIONS), logFacility(DEFAULT_LOG_FACILITY),
      isDefault(false), haveForked(false), logTime(false),
      progName(NULL), progID(progID), logTag(NULL),
      logDsts(new LogDsts()), oldLogDsts(NULL)
{
    initPkgInfo();
    setProgName(progName);
    setProgID(progID);
    setDebuggingState(debugStateStr);
}

Log::~Log()
{
    cleanupLogDsts(oldLogDsts);
    cleanupLogDsts(logDsts);
}

void
Log::setTimeZone()
{
    setenv("TZ", "UTC", true);
    tzset();
}

void
Log::setOptionsFacility(const char *progName, int logOptions, int logFacility)
{
    setOptionsFacility(progName, 0, logOptions, logFacility);
}

void
Log::setOptionsFacility(const char *progName, unsigned int progID,
	int logOptions, int logFacility)
{
    setProgName(progName);
    setProgID(progID);
    this->logOptions = logOptions;
    this->logFacility = logFacility;
    LogDsts *lds = logDsts; /* copy in case logDsts changes midstream. */
    for (LogDsts::iterator i = lds->begin(); i != lds->end(); ++i) {
	if ((*i)->getType() == LogDst::LDT_SYSLOG) {
	    LogDstSyslog *logDst = static_cast<LogDstSyslog *>(*i);
	    logDst->setOptionsFacility(getProgName(), progID,
		logOptions, logFacility);
	}
    }
}

void
Log::afterFork()
{
    bool firstFork = !haveForked;
    haveForked = true;
    /* Hook to stop logging to stderr. */
    if (firstFork) {
	LogDsts *lds = logDsts;/* copy in case logDsts changes midstream. */
	for (LogDsts::iterator i = lds->begin(); i != lds->end(); ++i) {
	    LogDst *logDst = *i;
	    logDst->afterFork();
	}
    }
}

void
Log::setProgName(const char *progName)
{
    if (this->progName != NULL)
	free(this->progName);
    // null check handled in getProgName()
    this->progName = progName ? strdup(progName) : NULL;

    /* Some systems default the core file size to 0 preventing debugging;
     * don't let it be ignored.
     */
    struct rlimit rlim;
    if (getrlimit(RLIMIT_CORE, &rlim) < 0)
	error("Log: getrlimit(RLIMIT_CORE) failed - %s", strerror(errno));
    else if (rlim.rlim_cur == 0) {
	if (!noZeroCoreWarning)
	    warning("Log: core size limited to 0");
    }
}

void
Log::setTag(const char *logTag)
{
    if (this->logTag != NULL)
	free(this->logTag);
    // null check handled in doLog()
    this->logTag = logTag ? strdup(logTag) : NULL;
}

void
Log::internalError(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    doLog(LOG_ERR, NULL, fmt, args);
    va_end(args);
}

void
Log::error(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    doLog(LOG_ERR, NULL, fmt, args);
    va_end(args);
}

void
Log::warning(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    doLog(LOG_WARNING, NULL, fmt, args);
    va_end(args);
}

void
Log::notice(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    doLog(LOG_NOTICE, NULL, fmt, args);
    va_end(args);
}

void
Log::info(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    doLog(LOG_INFO, NULL, fmt, args);
    va_end(args);
}

void
Log::debug(unsigned int debugPkg, const char *fmt, ...)
{
    va_list args;

    if (!debuggingPackage(debugPkg))
	return;

    /* Code duplicated in Log::DebugHelper::debug() */
    va_start(args, fmt);
    doLog(LOG_DEBUG, getDebugPkgInfoByValue(debugPkg), fmt, args);
    va_end(args);
}

unsigned int
Log::parseProgID(int argc, char *argv[], const char *optStr, char optChar)
{
    unsigned int progID = 0;

    /* No known safe way to reset getopt() state, so use another getopt(): */
    /*
     * Based on a public domain version of getopt(3) by Keith Bostic.
     */

    int	optInd = 1;
    int optOpt;
    const char *optArg;	
    const char *emsg = "";
    bool ok = true;
    bool found = false;
    const char *place = emsg;

    while (true) {
	const char *oli;	

	if (!*place) {			/* update scanning pointer */
	    if (optInd >= argc || *(place = argv[optInd]) != '-' || !*++place)
		break;
	    if (*place == '-') {	/* found "--" */
		++optInd;
		break;
	    }
	}				/* option letter okay? */
	if ((optOpt = (int) *place++) == (int) ':'
	    || !(oli = strchr(optStr, optOpt)))
	{
	    if (!*place)
		++optInd;
	    ok = false;
	    break;
	}
	if (*++oli != ':') {		/* don't need argument */
	    optArg = NULL;
	    if (!*place)
		++optInd;
	} else {				/* need an argument */
	    if (*place)
		optArg = place;	/* no white space */
	    else if (argc <= ++optInd) {	/* no arg */
		place = emsg;
		ok = false;
		break;
	    } else
		optArg = argv[optInd];	/* white space */
	    place = emsg;
	    ++optInd;
	    if (optOpt == optChar) {
		if (parseUInt(optArg, progID, 0, 1))
		    found = true;
		else {
		    ok = false;
		    break;
		}
	    }
	}
    }
    return found && ok ? progID : 0;
}

void
Log::DebugHelper::debug(const char *fmt, ...)
{
    va_list args;

    /* Code duplicated in Log::debug() */
    va_start(args, fmt);
    log->doLog(LOG_DEBUG, log->getDebugPkgInfoByValue(debugPkg), fmt, args);
    va_end(args);
}

void
Log::addDebugPkgInfo(const DebugPkgInfo *info, unsigned int nInfos)
{
    static const char methodName[] = "Log::addDebugPkgInfo";

    for (unsigned int i = 0; i < nInfos; ++i) {
	if (info[i].name == NULL) {
	    continue;
	}
	if (info[i].debugPkg < NELEM(debugPkgInfo)) {
	    if (debugPkgInfo[info[i].debugPkg].name != NULL)
		logmsg.internalError(
		    "%s: duplicate debug packge %d: (%s) vs (%s)",
		    methodName, info[i].debugPkg,
		    debugPkgInfo[info[i].debugPkg].name, info[i].name);
	    debugPkgInfo[info[i].debugPkg] = info[i];
	} else
	    logmsg.internalError("%s: debug packge out of range: %d",
		methodName, info[i].debugPkg);
    }
}

void
Log::setDebuggingState(const char *stateStr)
{
    static const char methodName[] = "Log::setDebuggingState";

    const char *str = stateStr != NULL ? stateStr : "";

    LogDsts *newLogDsts = new LogDsts();
    DebugState newDebugState;

    while (*str && isspace(*str))
	str++;
    if (*str == '+') {
	str++;
	while (*str && isspace(*str))
	    str++;
	for (LogDsts::iterator i = logDsts->begin(); i != logDsts->end(); ++i)
	    newLogDsts->push_back((*i)->clone());
	newDebugState = debugState;
    }

    bool ok = true;
    while (1) {
	bool flag = false;
	for (; isspace(*str) || *str == ',' || *str == '!'; ++str) {
	    if (*str == ',')
		flag = false;
	    else if (*str == '!')
		flag = true;
	}
	if (!*str)
	    break;
	const char *start = str;
	while (*str && *str != ',' && *str != '(' && !isspace(*str))
	    str++;
	std::string name(start, str - start);
	std::string params;
	if (*str == '(') {
	    ++str;
	    const char *pStart = str;
	    while (*str && *str != ')')
		++str;
	    if (*str != ')') {
		ok = false;
		logmsg.error("%s: \"%s\" parameter list not ended",
		    methodName, name.c_str());
		break;
	    }
	    params.append(pStart, str - pStart);
	    ++str;
	}
	if (name == "all") {
	    if (params.length() > 0)
		logmsg.warning("%s: ignoring parameters to \"%s\"",
		    methodName, name.c_str());
	    if (flag)
		newDebugState.reset();
	    else
		newDebugState.set();
	} else if (name == "list") {
	    if (params.length() > 0)
		logmsg.warning("%s: ignoring parameters to \"%s\"",
		    methodName, name.c_str());
	    logmsg.info("debugPackages are:");
	    for (unsigned int i = 0; i < NELEM(debugPkgInfo); ++i) {
		if (debugPkgInfo[i].name != NULL)
		    logmsg.info("    %s (%d)", debugPkgInfo[i].name,
		    	debugPkgInfo[i].debugPkg);
	    }
	    logmsg.info("  special tokens are 'all', 'list' and 'logTo';");
	    logmsg.info("  names may be preceeded by ! and separated by ,");
	    logmsg.info("  format of logTo: logTo(TYPE,PRI[,..]) where");
	    logmsg.info("    Type is syslog, stderr, or file; PRI is debug,");
	    logmsg.info("    info, warning, error, etc.;");
	    logmsg.info("    Examples:");
	    logmsg.info("      logTo(type=syslog,priority=info,facility=daemon)");
	    logmsg.info("      logTo(type=stderr,priority=debug) or logTo(type=stderr,priority=debug,fd=1,logTime=true)");
	    logmsg.info("      logTo(type=file,priority=debug,baseName=/tmp/mylog) [appends]");
	    logmsg.info("      logTo(type=file,priority=debug,baseName=/tmp/mylog.XXXXXX) [Xs made unique]");
	    logmsg.info("      logTo(type=file,priority=debug,baseName=mylog.XXXXXX) [relative to tmpdir]");
	    logmsg.info("      logTo(type=file,priority=debug,baseName=PROG_NAME.XXXXXX) [PROG_NAME,PROG_ID,PROG_NAME_ID replaced]");
	} else if (name == "logTo") {
	    if (params.length() == 0) {
		ok = false;
		logmsg.error("%s: no parameters to \"%s\"",
		    methodName, name.c_str());
		break;
	    }
	    typedef std::map<std::string, std::string> ParamHash;
	    ParamHash paramHash;
	    const char *p = params.c_str();
	    /* Split by comma, tossing empty strings and spaces. */
	    while (1) {
		while (*p == ',' || isspace(*p))
		    ++p;
		if (!*p)
		    break;
		const char *s = p;
		const char *eq = NULL;
		for (; *p && *p != ',' && !isspace(*p); ++p)
		    if (eq == NULL && *p == '=')
			eq = p;
		if (eq == NULL) {
		    ok = false;
		    logmsg.error("%s: \"%s\" parameters [%s] is missing an =",
			methodName, name.c_str(), params.c_str());
		    break;
		}
		paramHash[std::string(s, eq - s)]
		    = std::string(eq + 1, p - eq - 1);
		if (isspace(*p)) {
		    while (*p && isspace(*p))
			++p;
		    if (*p && *p != ',') {
			ok = false;
			logmsg.error("%s: \"%s\" parameters [%s] contains misplaced white-space",
			    methodName, name.c_str(), params.c_str());
			break;
		    }
		}
	    }

	    ParamHash::iterator pi = paramHash.find("type");
	    if (pi == paramHash.end()) {
		ok = false;
		logmsg.error("%s: logTo(%s): no type specified",
		    methodName, params.c_str());
		break;
	    }
	    std::string ldType = pi->second;
	    paramHash.erase(pi);

	    pi = paramHash.find("priority");
	    if (pi == paramHash.end()) {
		ok = false;
		logmsg.error("%s: logTo(%s): no priority specified",
		    methodName, params.c_str());
		break;
	    }
	    std::string priName = pi->second;
	    paramHash.erase(pi);
	    int minPriority = getPriorityByName(priName.c_str());
	    if (minPriority < 0) {
		ok = false;
		logmsg.error("%s: logTo(%s): unknown priority \"%s\"",
		    methodName, params.c_str(), priName.c_str());
		break;
	    }

	    LogDst *newLogDst = NULL;
	    if (ldType == "syslog") {
		int facility = logFacility;
		pi = paramHash.find("facility");
		if (pi != paramHash.end()) {
		    std::string facName = pi->second;
		    paramHash.erase(pi);
		    facility = getFacilityByName(facName.c_str());
		    if (facility < 0) {
			ok = false;
			logmsg.error("%s: logTo(%s): unknown facility \"%s\"",
			    methodName, params.c_str(), facName.c_str());
			break;
		    }
		}
		newLogDst = new LogDstSyslog(minPriority, logOptions, facility);
	    } else if (ldType == "stderr") {
		int fd = LogDstStderr::DEFAULT_FD;
		pi = paramHash.find("fd");
		if (pi != paramHash.end()) {
		    std::string fdStr = pi->second;
		    paramHash.erase(pi);
		    if (!parseInt(fdStr.c_str(), fd, 0, 0)) {
			ok = false;
			logmsg.error("%s: logTo(%s): can't parse fd \"%s\"",
			    methodName, params.c_str(), fdStr.c_str());
			break;
		    }
		}
		bool logTime = LogDstStderr::DEFAULT_LOG_TIME;
		pi = paramHash.find("logTime");
		if (pi != paramHash.end()) {
		    std::string ltStr = pi->second;
		    paramHash.erase(pi);
		    if (!parseBool(ltStr.c_str(), logTime)) {
			ok = false;
			logmsg.error(
			    "%s: logTo(%s): can't parse logTime \"%s\"",
			    methodName, params.c_str(), ltStr.c_str());
			break;
		    }
		}
		bool stopOnFork = LogDstStderr::DEFAULT_STOP_ON_FORK;
		pi = paramHash.find("stopOnFork");
		if (pi != paramHash.end()) {
		    std::string sofStr = pi->second;
		    paramHash.erase(pi);
		    if (!parseBool(sofStr.c_str(), stopOnFork)) {
			ok = false;
			logmsg.error(
			    "%s: logTo(%s): can't parse stopOnFork \"%s\"",
			    methodName, params.c_str(), sofStr.c_str());
			break;
		    }
		}
		newLogDst = new LogDstStderr(minPriority, stopOnFork, fd,
		    logTime);
	    } else if (ldType == "file") {
		pi = paramHash.find("baseName");
		if (pi == paramHash.end()) {
		    ok = false;
		    logmsg.error("%s: logTo(%s): no baseName specified",
			methodName, params.c_str());
		    break;
		}
		std::string baseName = pi->second;
		paramHash.erase(pi);
		bool logTime = LogDstFile::DEFAULT_LOG_TIME;
		pi = paramHash.find("logTime");
		if (pi != paramHash.end()) {
		    std::string ltStr = pi->second;
		    paramHash.erase(pi);
		    if (!parseBool(ltStr.c_str(), logTime)) {
			ok = false;
			logmsg.error(
			    "%s: logTo(%s): can't parse logTime \"%s\"",
			    methodName, params.c_str(), ltStr.c_str());
			break;
		    }
		}
		unsigned long rotateLogSize = 32 * 1024 * 1024;
		pi = paramHash.find("rotateLogSize");
		if (pi != paramHash.end()) {
		    std::string ltStr = pi->second;
		    paramHash.erase(pi);
		    if (!parseULong(ltStr.c_str(), rotateLogSize)) {
			ok = false;
			logmsg.error(
			    "%s: logTo(%s): can't parse rotateLogSize \"%s\"",
			    methodName, params.c_str(), ltStr.c_str());
			break;
		    }
		}
		unsigned int rotateFileMax = 7;
		pi = paramHash.find("rotateFileMax");
		if (pi != paramHash.end()) {
		    std::string ltStr = pi->second;
		    paramHash.erase(pi);
		    if (!parseUInt(ltStr.c_str(), rotateFileMax)) {
			ok = false;
			logmsg.error(
			    "%s: logTo(%s): can't parse rotateFileMax \"%s\"",
			    methodName, params.c_str(), ltStr.c_str());
			break;
		    }
		}
		newLogDst = new LogDstFile(minPriority, baseName, logTime,
		    rotateLogSize, rotateFileMax);
	    } else {
		ok = false;
		logmsg.error("%s: logTo(%s): unknown type \"%s\"",
		    methodName, params.c_str(), ldType.c_str());
		break;
	    }
	    for (pi = paramHash.begin(); pi != paramHash.end(); ++pi)
		logmsg.warning("%s: logTo(%s): unknown parameter \"%s=%s\"",
		    methodName, ldType.c_str(),
		    pi->first.c_str(), pi->second.c_str());
	    /* Eliminate duplicate entries. */
	    for (LogDsts::iterator i = newLogDsts->begin();
		    i != newLogDsts->end(); ++i)
	    {
		LogDst *ld = *i;
		if (ld->mostlyEqual(newLogDst)) {
		    /* Later settings override former. */
		    *i = newLogDst;
		    delete ld;
		    newLogDst = NULL;
		    break;
		}
	    }
	    if (newLogDst != NULL)
		newLogDsts->push_back(newLogDst);
	} else {
	    if (params.length() > 0)
		logmsg.warning("%s: ignoring parameters to \"%s\"",
		    methodName, name.c_str());
	    DebugPkgInfo *dpi = getDebugPkgInfoByName(name.c_str());
	    if (dpi == NULL) {
		/* Don't fail due to this. */
		logmsg.warning("%s: unknown debugging packge: %s", methodName,
		    name.c_str());
	    } else if (flag)
		newDebugState.reset(dpi->debugPkg);
	    else
		newDebugState.set(dpi->debugPkg);
	}
    }

    if (ok) {
	debugState = newDebugState;
	if (newLogDsts->empty()) {
	    delete newLogDsts;
	    newLogDsts = defaultLogDst();
	    isDefault = true;
	} else
	    isDefault = false;
	setLogDsts(newLogDsts, true);
    } else {
	for (LogDsts::iterator i = newLogDsts->begin(); i != newLogDsts->end();
		++i)
	    delete *i;
	delete newLogDsts;
    }
}

Log::LogDsts *
Log::defaultLogDst()
{
    LogDsts *newLogDsts = new LogDsts();

    int defaultMinPriority = LOG_INFO;
    newLogDsts->push_back(
	new LogDstSyslog(defaultMinPriority, logOptions, logFacility));
    if (!haveForked)
	newLogDsts->push_back(new LogDstStderr(defaultMinPriority, true));
    return newLogDsts;
}

void
Log::setLogDsts(LogDsts *newLogDsts, bool logChange)
{
    for (LogDsts::iterator i = newLogDsts->begin(); i != newLogDsts->end(); ) {
	LogDst *newLD = *i;
	bool found = false;
	for (LogDsts::iterator j = logDsts->begin(); j != logDsts->end(); ++j) {
	    LogDst *curLD = *j;
	    if (newLD->mostlyEqual(curLD)) {
		curLD->incInuse();
		*i = curLD;
		curLD->setParams(getProgName(), getProgID(), newLD);
		found = true;
		break;
	    }
	}
	if (!found) {
	    if (newLD->open(getProgName(), getProgID(), haveForked)) {
		newLD->incInuse();
		++i;
	    } else {
		i = newLogDsts->erase(i);
		delete newLD;
	    }
	} else {
	    ++i;
	    delete newLD;
	}
    }

    bool newLogTime = false;
    for (LogDsts::iterator i = newLogDsts->begin(); i != newLogDsts->end(); ++i)
	if ((*i)->getLogTime()) {
	    newLogTime = true;
	    break;
	}

    LogDsts *old = oldLogDsts;
    oldLogDsts = logDsts;
    logDsts = newLogDsts;
    logTime = newLogTime;

    for (LogDsts::iterator i = oldLogDsts->begin(); i != oldLogDsts->end(); ++i)
    {
	LogDst *logDst = *i;
	if (find(newLogDsts->begin(), newLogDsts->end(), logDst)
		== newLogDsts->end())
	    logDst->pseudoClose();
    }

    if (old != NULL)
	cleanupLogDsts(old);

    /* Too annoying. */
    if (0 && logChange) {
	notice("logging changed:");
	for (LogDsts::iterator i = logDsts->begin(); i != logDsts->end(); ++i)
	    notice("  %s.", (*i)->toString().c_str());
    }
}

void
Log::cleanupLogDsts(LogDsts *&lDsts)
{
    if (lDsts != NULL) {
	for (LogDsts::iterator i = lDsts->begin(); i != lDsts->end(); ++i) {
	    LogDst *logDst = *i;
	    logDst->decInuse();
	    if (logDst->getInuse() == 0)
		delete logDst;
	}
	delete lDsts;
	lDsts = NULL;
    }
}

Log::DebugPkgInfo *
Log::getDebugPkgInfoByName(const char *name)
{
    if (name != NULL) {
	for (unsigned int i = 0; i < NELEM(debugPkgInfo); ++i) {
	    if (debugPkgInfo[i].name != NULL
		&& strcasecmp(debugPkgInfo[i].name, name) == 0)
		return &debugPkgInfo[i];
	}
    }
    return NULL;
}

Log::DebugPkgInfo *
Log::getDebugPkgInfoByValue(unsigned int debugPkg)
{
    if (debugPkg < NELEM(debugPkgInfo))
	return &debugPkgInfo[debugPkg];
    return NULL;
}

void
Log::doLog(int priority, const DebugPkgInfo *dpi, const char *fmt, va_list args)
{
    /*
     * Want to produce something like:
     *
    Apr 25 16:42:34.83 sea PAM_unix[17852]: info: (blah) SiteClassSchedule::pruneSchedule: starting before...
    Apr 25 16:42:34.81 sea PAM_unix[17852]: debug<aiow>: SiteClassSchedule::pruneSchedule: starting before...
     *
     * ie,
     *	time host progName[pid]: priority<debugName>: (logTag) msg
     *
     */
    char msgBuf[B_SIZE];
    if (logTag != NULL)
	snprintf(msgBuf, sizeof(msgBuf), "(%s) ", logTag);
    else
	msgBuf[0] = '\0';
    int len = strlen(msgBuf);
    vsnprintf(msgBuf + len, sizeof(msgBuf) - len, fmt, args);

    char timeBuf[64];
    if (logTime) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	snprintf(timeBuf, sizeof(timeBuf), "%.15s.%04d ",
	    ctime(&tv.tv_sec) + 4, (int) (tv.tv_usec / 100));
    } else
	timeBuf[0] = '\0';

    LogDsts *lds = logDsts; /* copy in case logDsts changes midstream. */
    for (LogDsts::iterator i = lds->begin(); i != lds->end(); ++i)
	(*i)->doLog(getProgName(), getProgID(), timeBuf, priority, dpi, msgBuf);
}

static void
getPriorityName(char *buf, int bsize, int priority,
    const Log::DebugPkgInfo *dpi, bool useColor)
{
    char ukBuf[64];
    enum {
	C_NONE = 0,
	C_RED = 31,
	C_GREEN = 32,
	C_YELLOW = 33,
	C_BLUE = 34,
	C_MAGENTA = 35,
	C_CYAN = 36,
	/* Reverse video. */
	C_RED_REV = C_RED + 10,
	C_GREEN_REV = C_GREEN + 10,
	C_YELLOW_REV = C_YELLOW + 10,
	C_BLUE_REV = C_BLUE + 10,
	C_MAGENTA_REV = C_MAGENTA + 10,
	C_CYAN_REV = C_CYAN + 10,
    };
    static int debugColors[] = {
	C_RED, C_GREEN, C_YELLOW, C_BLUE, C_MAGENTA, C_CYAN
	/*
	C_RED_REV, C_GREEN_REV, C_YELLOW_REV, C_BLUE_REV,
	C_MAGENTA_REV, C_CYAN_REV
	*/
    };

    const char *p = NULL;
    int color = C_NONE;
    switch (priority) {
      case LOG_EMERG:	p = "emerg";	color = C_RED_REV;	break;
      case LOG_ALERT:	p = "alert";	color = C_RED_REV;	break;
      case LOG_CRIT:	p = "critical";	color = C_RED_REV;	break;
      case LOG_ERR:	p = "error";	color = C_RED_REV;	break;
      case LOG_WARNING:	p = "warning";	color = C_YELLOW_REV;	break;
      case LOG_NOTICE:	p = "notice";	color = C_MAGENTA_REV;	break;
      case LOG_INFO:	p = "info";	color = C_MAGENTA_REV;	break;
      case LOG_DEBUG:	p = "debug";	color = C_NONE;		break;
      default:
	p = ukBuf;
	color = C_RED_REV;
	snprintf(ukBuf, sizeof(ukBuf), "[unknown priority %d]", priority);
    }
    if (useColor && color != C_NONE) {
	snprintf(buf, bsize, "\033[1;%dm%s\033[0m", color, p);
    } else {
	snprintf(buf, bsize, "%s", p);
    }
    if (dpi != NULL) {
	int len = strlen(buf);
	if (useColor)
	    snprintf(buf + len, sizeof(buf) - len, "<\033[1;%dm%s\033[0m>",
		debugColors[dpi->debugPkg % NELEM(debugColors)], dpi->name);
	else
	    snprintf(buf + len, sizeof(buf) - len, "<%s>", dpi->name);
    }
}

static int
getPriorityByName(const char *name)
{
    for (int i = 0; prioritynames[i].c_name != NULL; ++i)
	if (strcmp(name, prioritynames[i].c_name) == 0)
	    return prioritynames[i].c_val;
    return -1;
}

static const char *
getPriorityName(int priority)
{
    static char buf[64];
    for (int i = 0; prioritynames[i].c_name != NULL; ++i)
	if (prioritynames[i].c_val == priority)
	    return prioritynames[i].c_name;
    snprintf(buf, sizeof(buf), "[unknown-priority:%d]", priority);
    return buf;
}

static int
getFacilityByName(const char *name)
{
    for (int i = 0; facilitynames[i].c_name != NULL; ++i)
	if (strcmp(name, facilitynames[i].c_name) == 0)
	    return facilitynames[i].c_val;
    return -1;
}

static const char *
getFacilityName(int facility)
{
    static char buf[64];
    for (int i = 0; facilitynames[i].c_name != NULL; ++i)
	if (facilitynames[i].c_val == facility)
	    return facilitynames[i].c_name;
    snprintf(buf, sizeof(buf), "[unknown-facility:%d]", facility);
    return buf;
}
