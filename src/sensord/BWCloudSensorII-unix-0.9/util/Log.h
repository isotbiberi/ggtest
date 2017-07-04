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
#ifndef UTIL_LOG_H
# define UTIL_LOG_H

# include <stdarg.h>
# include <syslog.h>
# include <cstring>

# include <iostream>
# include <list>

# include "util/toString.h"
# include "util/GCCAttributes.h"

# define DEFAULT_LOG_FACILITY	LOG_DAEMON
# define DEFAULT_LOG_OPTIONS	LOG_PID

/* Additional options to add to syslog LOG_OPTIONS (need to make
 * sure these don't conflict with syslog's).
 */
/* Add "-progId" to the program name in log messages
 * (e.g., lmAgent-1234[pid]: info: something)
 */
# define MT_LOG_PROG_ID		0x10000
/* Used to remove our options before passing them on to syslog. */
# define MT_LOG_OPTIONS_MASK	(MT_LOG_PROG_ID)

/*
 * Class used to log error and debugging messages either to stderr or
 * to syslog (starts by logging to stderr and switches to syslog, if
 * appropriate, when afterFork() is called).
 * Debugging messages are catagorized into `packages' (see DBG_* defines) and
 * can be filtered out based on their package (filtering is efficient -
 * format statement is not even called unless the message is to be reported -
 * see logmsg_DEBUG() for details).
 *
 * Usage of Log class:
 *	- there is a global variable called logmsg.
 *	- programs call
 *	    logmsg.setOptionsFacility(PROG_NAME, progID,
 *		DEFAULT_LOG_OPTIONS, DEFAULT_LOG_FACILITY);
 *	  or
 *	    logmsg.setProgName(PROG_NAME);
 *	    logmsg.setProgID(PROG_ID);	-- optional
 *        in main before doing anything else.
 *	- programs register local debugging packages using
 *	    logmsg.addDebugPkgInfo(...);
 *	- programs can set the debugging message filter using:
 *	    logmsg.setDebuggingState(optarg);
 *	  (use "list" to list all package names).
 *	- there are a number of types of messages that can be logged:
 *	    logmsg.internalError(...)	where you might use assert() - dosen't
 *					dump core, though.
 *	    logmsg.error(...)		for bad things (eg, can't talk to db).
 *	    logmsg.warning(...)		for things that may indicate a problem.
 *	    logmsg.notice(...)		for things of significant interest.
 *	    logmsg.info(...)		for things of passing interest.
 *	    logmsg_DEBUG(DBG_PKG_XXX, (...)) for debugging messages - the odd
 *					calling sequence is to allow efficient
 *					filtering of messages.
 *	- if you want a string to preceed all logged messages, use
 *	  logmsg.setTag(someString) (example use: a process is forked() to
 *	  handle a particular connection - setTag is used to prefix all
 *	  messages with the ip address and port of the connection).
 */

/*
 * Add one of these for each library module/class/package that needs it.
 *
 * These also need to be in Log.c::libDebugPkgInfos[].
 */
# define DBG_PKG_UTIL_SERVER_CONTROLLER		1
# define DBG_PKG_UTIL_SERVER_CONTROLLER_V	2	/* verbose */
# define DBG_PKG_UTIL_PTMUTEX			3
# define DBG_PKG_UTIL_PTCONDITION		4
# define DBG_PKG_UTIL_CHILD_SERVICE		5
# define DBG_PKG_UTIL_SIGNAL_MANAGER		6
# define DBG_PKG_UTIL_SELECTOR			7
# define DBG_PKG_UTIL_CONTROLLER_WORK		8
# define DBG_PKG_UTIL_NLS			9
# define DBG_PKG_UTIL_TWORKER			10
# define DBG_PKG_UTIL_TWORKER_V			11
# define DBG_PKG_UTIL_SERVICE_CONNECTION	12
# define DBG_PKG_UTIL_MISC			13
/* Not a package, per say: should fork() be avoided? */
# define DBG_PKG_FLAG_NOFORK			20
# define DBG_PKG_FLAG___UNUSED1			21
# define DBG_PKG_FLAG_NOCHDIR			22
/* Input/Output streams */
# define DBG_PKG_MSG_STREAM			25
# define DBG_PKG_MSG_SEND_RCV			26
# define DBG_PKG_MSG_SEND_RCV_V			27
# define DBG_PKG_SURVEYDB			30
# define DBG_PKG_SURVEYDB_V			31
# define DBG_PKG_DB				32
# define DBG_PKG_DB_V				33
# define DBG_PKG_CLIENT_WAREHOUSE		35
# define DBG_PKG_CLIENT_WSM			36
# define DBG_PKG_CLIENT_OBS_STN			37
# define DBG_PKG_CLIENT_IMAGE_CACHE		38
# define DBG_PKG_CLIENT_NOTIFIER		39
# define DBG_PKG_CLIENT_NFM			40 /* should be NLM */
# define DBG_PKG_CLIENT_NSM			41
# define DBG_PKG_CLIENT_SCHEDULER		42 /* Manager and Connection. */
# define DBG_PKG_SURVEY_SCHEDULER		43
# define DBG_PKG_OBS_DEV_TELPOS			44
# define DBG_PKG_OBS_DEV_CAMERA			45
# define DBG_PKG_OBS_DEV_MISC			46
# define DBG_PKG_OBS_DEV_MISC_V			47
# define DBG_PKG_IMAGE				48
# define DBG_PKG_CLIENT_BE			49

/* 	 ^^^^^^^^^^^^^^^^^^^^^ You're about to add that to Log.c - right? */

/* Application specific packages start here. */
# define DBG_PKG__APP				80
/* Must not exceed this (can be increased as needed). */
# define DBG_PKG__MAX				128


# define logmsg_DEBUG(l, args) \
		    do { \
			if (logmsg.debuggingPackage(l)) { \
			    Log::DebugHelper debugHelper(&logmsg, l); \
			    debugHelper.debug args; \
			} \
		    } while(0)


class LogDst; /* defined in Log.cpp */

class Log
{
  public: /* Types */
    /*
     * Used to map package name to package constant & vice versa.
     */
    struct DebugPkgInfo {
	DebugPkgInfo(unsigned int debugPkg, const char *name)
	    : debugPkg(debugPkg), name(name) { }
	DebugPkgInfo() : debugPkg(0), name(NULL) { }
	unsigned int debugPkg;
	const char *name;
    };
    class DebugHelper {		/* Kludge class for logmsg_DEBUG() */
	Log *log;
	unsigned int debugPkg;
      public:
	DebugHelper(Log *log, unsigned int debugPkg)
	    : log(log), debugPkg(debugPkg) { }
	void debug(const char *fmt, ...)
	    GCC_FUNC_ATTR(format(printf, 2, 3));
    };
    friend class Log::DebugHelper;
    class DebugState {
	enum { UNIT_SIZE = 8 };
	char bits[DBG_PKG__MAX / UNIT_SIZE];
      public:
	DebugState() { reset(); }
	/* biti == bit index. */
	bool isSet(unsigned int biti) const {
	    return biti < DBG_PKG__MAX
		&& (bits[biti / UNIT_SIZE] & (1 << (biti % UNIT_SIZE))) != 0;
	}
	void reset() {
	    memset(bits, 0, sizeof(bits));
	}
	void set() {
	    memset(bits, -1, sizeof(bits));
	}
	void reset(unsigned int biti) {
	    if (biti < DBG_PKG__MAX)
		bits[biti / UNIT_SIZE] &= (char) ~(1 << (biti % UNIT_SIZE));
	}
	void set(unsigned int biti) {
	    if (biti < DBG_PKG__MAX)
		bits[biti / UNIT_SIZE] |= (char) (1 << (biti % UNIT_SIZE));
	}
	void set(unsigned int biti, bool val) {
	    if (val)
		set(biti);
	    else
		reset(biti);
	}
	bool any() const {
	    for (unsigned int i = 0; i < sizeof(bits) / sizeof(bits[0]); ++i)
		if (bits[i] != 0)
		    return true;
	    return false;
	}
    };

  private: /* Data */
    int logOptions;
    int logFacility;
    bool isDefault;
    bool haveForked;
    bool logTime;
    char *progName;
    unsigned int progID;
    char *logTag;
    DebugState debugState;
    DebugPkgInfo debugPkgInfo[DBG_PKG__MAX];
    typedef std::list<LogDst *> LogDsts;
    LogDsts *logDsts;
    LogDsts *oldLogDsts; /* previous list. */

  public: /* Interface */
    Log();
    /* Used for specialized logs. */
    Log(const char *progName, const char *debugStateStr);
    Log(const char *progName, unsigned int progID, const char *debugStateStr);
    ~Log();

    /* Called to set timezone to UTC/GMT (so times of log messages in various
     * sites can be matched).
     */
    void setTimeZone();
    /* Called when program starts up initially. */
    void setOptionsFacility(const char *progName, int logOptions,
	int logFacility);
    void setOptionsFacility(const char *progName, unsigned int progID,
	int logOptions, int logFacility);

    /* Called after any forks - first call causes a switch to
     * syslog unless overriden by debugging flags.
     */
    void afterFork();

    /* Add name/value pairs to the set of known debugging packages. */
    void addDebugPkgInfo(const DebugPkgInfo *info, unsigned int nInfos);
    /* Set the debugging state according to the string:  eg, "all",
     * "server_controller,ptmutex", "all,!ptmutex".
     * Current settings are thrown away unless stateStr starts with "+".
     */
    void setDebuggingState(const char *stateStr);
    bool debuggingPackage(unsigned int debugPkg) const {
	return debugState.isSet(debugPkg);
    }

    const char *getProgName() const {
    	return progName ? progName : "(unknown)";
    }
    void setProgName(const char *progName);
    unsigned int getProgID() const {
        return progID;
    }
    void setProgID(unsigned int progID) {
        this->progID = progID;
    }
    /* Additional string prepended to each message (useful for identifing
     * child processes).  A copy of tag is saved.
     */

    /* Note: can return null. */
    const char *getTag() const {
    	return logTag;
    }
    void setTag(const char *tag);

    /* Just like error(...) but different self documenting name. */
    void internalError(const char *fmt, ...)
	GCC_FUNC_ATTR(format(printf, 2, 3));
    void error(const char *fmt, ...)
	GCC_FUNC_ATTR(format(printf, 2, 3));
    void warning(const char *fmt, ...)
	GCC_FUNC_ATTR(format(printf, 2, 3));
    void notice(const char *fmt, ...)
	GCC_FUNC_ATTR(format(printf, 2, 3));
    void info(const char *fmt, ...)
	GCC_FUNC_ATTR(format(printf, 2, 3));
    void debug(unsigned int debugPkg, const char *fmt, ...)
	GCC_FUNC_ATTR(format(printf, 3, 4));

    /* Hack: used to get program id, used by many programs, before
     * real option processing, etc. starts, so it can be integrated
     * into the progName used to name the log file.
     * optChar is the getopt() option flag (e.g., 'I' in "-I 12").
     * Returns the program ID if it was found an no parsing errors
     * occurred; 0 if there were problems of any kind.
     */
    static unsigned int parseProgID(int argc, char *argv[],
	const char *optStr, char optChar);

    /* Suppress warning about rlimit coresize of 0. */
    static bool noZeroCoreWarning;

  private:
    void doLog(int priority, const DebugPkgInfo *dpi, const char *fmt,
	va_list args);
    DebugPkgInfo *getDebugPkgInfoByName(const char *name);
    DebugPkgInfo *getDebugPkgInfoByValue(unsigned int debugPkg);
    LogDsts *defaultLogDst();
    /* NOTE: given list must not contain duplicates (wrt equals method). */
    void setLogDsts(LogDsts *newLogDsts, bool logChange);
    static void cleanupLogDsts(LogDsts *&lDsts);
    void initPkgInfo();
};

extern Log logmsg;

#endif /* UTIL_LOG_H */
