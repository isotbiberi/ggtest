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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <iomanip>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/poll.h>

#include "util/toString.h"
#include "util/IntervalTimer.h"
#include "util/Log.h"
#include "util/EnumNamer.h"

#include "SerialConnection.h"

namespace {
    /* Used to reduce size of log file. */
    static const char *
    baseMethod(const char *methodName)
    {
	const char *p = strrchr(methodName, ':');
	return p != NULL ? p + 1 : methodName;
    }
}

SerialConnection::SerialConnection()
    : devFd(-1), serialLog(NULL)
{
}

SerialConnection::SerialConnection( const std::string &device, bool doLogging )
    : deviceName(device), devFd(-1), serialLog(NULL)
{
    setLogging(doLogging);
}

SerialConnection::~SerialConnection()
{
    if (devFd >= 0) {
	try {
	    close();
	} catch (...) {
	    /* ignore. */
	}
    }
    if (serialLog != NULL) {
	delete serialLog; /* after close() */
    }
}

void
SerialConnection::printState(std::ostream &os, int indent, int indentIncr) const
{
    os << std::setw(indent) << "" << "SerialConnection " << this << " {"
        << std::endl;
    indent += indentIncr;

    os << std::setw(indent) << "" << "deviceName\"=" << deviceName << '"'
	<< std::endl;
    os << std::setw(indent) << "" << "devFd=" << devFd << std::endl;
    os << std::setw(indent) << "" << "serialLog=" << serialLog << std::endl;

    indent -= indentIncr;
    os << std::setw(indent) << "" << "}" << std::endl;
}

void
SerialConnection::open()
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::open";

    if (deviceName.size() == 0)
	throw SerialConnectionException(toStringFmt("%s: device name not set",
	    methodName));

    devFd = ::open( deviceName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
    if ( devFd < 0 ) {
	if (serialLog != NULL)
	    serialLog->error("%s: open %s failed - %s", methodName,
		deviceName.c_str(), strerror(errno));
	logmsg.error("%s: open %s failed - %s", methodName,
	    deviceName.c_str(), strerror(errno));
	throw SerialConnectionException(toStringFmt(
	    "%s: Cannot open device - %s", deviceName.c_str(),
	    strerror(errno)) );
    }
    if (serialLog != NULL)
	serialLog->info("%s: (%s)", baseMethod(methodName), deviceName.c_str());
    /* ensure reads are blocking */
    /* to make nonblock set the last argument to FNDELAY */
    fcntl( devFd, F_SETFL, 0 );

    /* ensure there is no outstanding input, XXX */
    discardIO();

    /* set common defaults */
    struct termios options;
    int ret = tcgetattr( devFd, &options );
    checkReturnStatus( methodName, "tcgetattr", ret );

    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;           /* 8 bits */
    options.c_cflag &= ~PARENB;       /* no parity */
    options.c_cflag |= CLOCAL;
    cfsetospeed( &options, B9600);
    cfsetispeed( &options, B9600);

    ret = tcsetattr( devFd, TCSANOW, &options );
    checkReturnStatus( methodName, "tcsetattr(TCSANOW)", ret );
}

void
SerialConnection::checkState(const char *callersName)
    throw (SerialConnectionException)
{
    if ( devFd < 0 ) {
	std::string msg = toStringFmt("%s: %s: called while not open",
	    callersName, deviceName.c_str());
	logmsg.error("%s", msg.c_str());
	if (serialLog != NULL)
	    serialLog->error("%s", msg.c_str());
	throw SerialConnectionException(msg);
    }

}

void
SerialConnection::checkReturnStatus( const char *callersName,
	const char *operation, int ret )
    throw (SerialConnectionException)
{
    if ( ret < 0 ) {
	std::string msg = toStringFmt("%s: %s: %s failed - %s", callersName,
	    deviceName.c_str(), operation, strerror(errno));
	logmsg.error("%s", msg.c_str());
	if (serialLog != NULL)
	    serialLog->error("%s", msg.c_str());
	throw SerialConnectionException(msg);
    }
}

void
SerialConnection::setLogging(bool value)
{
    bool logging = serialLog != NULL;
    if (logging != value) {
	if (value) {
	    std::string::size_type p = deviceName.find_last_of('/');
	    std::string basename = p != std::string::npos
		? deviceName.substr(p + 1) : deviceName;
	    serialLog = new Log(basename.c_str(),
		"logTo(type=file,priority=info,baseName=serial-PROG_NAME.log)");
	    serialLog->info("created on (%s)", deviceName.c_str());
	} else {
	    delete serialLog;
	    serialLog = NULL;
	}
    }
}

void
SerialConnection::setDeviceName(const std::string &deviceName)
{
    this->deviceName = deviceName;
}

const std::string &
SerialConnection::getDeviceName() const
{
    return deviceName;
}

int
SerialConnection::write( const void *data, int size )
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::write";

    checkState(methodName);
    if (serialLog != NULL)
	serialLog->info("%s(\"%s\", %d)", baseMethod(methodName),
	    toStringDump(data, size).c_str(), size);
    int ret = ::write( devFd, data, size );
    if (serialLog != NULL && ret != size)
	serialLog->error("  write returned %d (%s)",
	    ret, strerror(errno));
    checkReturnStatus( methodName, "write", ret );
    return ret;
}

void
SerialConnection::writeFully(const void *data, int size )
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::writeFully";

    checkState(methodName);
    if (serialLog != NULL)
	serialLog->info("%s(\"%s\", %d)", baseMethod(methodName),
	    toStringDump(data, size).c_str(), size);
    int nwritten = 0;
    while (nwritten < size) {
	int ret = ::write( devFd, (const char *) data + nwritten,
	    size - nwritten );
	if (serialLog != NULL && ret != size - nwritten)
	    serialLog->error("  write returned %d (%s) nwritten=%d",
		ret, strerror(errno), nwritten);
	checkReturnStatus( methodName, "write", ret );
	nwritten += ret;
	/* Should only happen if non-blocking set, in which case
	 * this method should not have been used.
	 */
	if (ret == 0) {
	    std::string msg = toStringFmt(
		"%s: %s: write wrote nothing (after %d of %d)",
		methodName, deviceName.c_str(), nwritten, size);
	    logmsg.error("%s", msg.c_str());
	    throw SerialConnectionException(msg);
	}
    }
}

int
SerialConnection::read( void *data, int size )
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::read";

    checkState(methodName);
    int ret = ::read( devFd, data, size );
    if (serialLog != NULL) {
	int err = errno;
	if (ret < 0)
	    serialLog->error("%s: size=%d: failed - %s", methodName,
		size, strerror(errno));
	else if (ret == 0 && size != 0)
	    serialLog->warning("%s: size=%d: EOF", methodName, size);
	else
	    serialLog->info("%s(%d) = %d [%s]", baseMethod(methodName),
		size, ret, toStringDump(data, ret).c_str());
	errno = err;
    }
    checkReturnStatus( methodName, "read", ret );
    return ret;
}

int
SerialConnection::readTimeout(void *data, int size, int toMilliSecs)
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::readTimeout";

    checkState(methodName);

    struct pollfd pfd;
    pfd.fd = devFd;
    pfd.events = POLLIN;
    int ret;
    IntervalTimer timer;
    timer.start();
    while (1) {
	int to = static_cast<int>(toMilliSecs
	    - timer.getSeconds() * 1000);
	if (to <= 0)
	    to = 0;
	else if (to < 10) /* the smallest quanta is 10 milliseconds */
	    to = 10;
	ret = ::poll(&pfd, 1, to);
	if (ret >= 0 || errno != EINTR)
	    break;
    }
    checkReturnStatus( methodName, "poll", ret );
    if ( ret == 0 ) {
	if (serialLog != NULL)
	    serialLog->info("%s: size=%d, timeout=%d: TIMEOUT",
		methodName, size, toMilliSecs);
	return 0; /* timed out with no data */
    }

    return read( data, size );
}

int
SerialConnection::readTimeoutDelim(void *data, int size,
	int toMilliSecs, unsigned char delimChar)
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::readTimeoutDelim";

    checkState(methodName);

    IntervalTimer timer;
    timer.start();

    int totalRead = 0;
    while (true) {
	int to = static_cast<int>(toMilliSecs
	    - timer.getSeconds() * 1000);
        int r = readTimeout((char *) data + totalRead, size - totalRead, to);
	totalRead += r;
	if (r == 0 || totalRead == size
	    || (r > 0 && ((unsigned char *) data)[totalRead - 1] == delimChar))
	    break;

	if (serialLog != NULL)
	    serialLog->info("%s: waiting", baseMethod(methodName));
    }
    return totalRead;
}


int
SerialConnection::readFully( void *data, int size )
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::readFully";

    checkState(methodName);
    int ret, amt = 0;
    do {
	if (serialLog != NULL && amt > 0)
	    serialLog->info("%s: waiting", baseMethod(methodName));
	ret = read( (char *) data + amt, size-amt );
	amt += ret;
    } while ( ret > 0 && amt < size );
    return amt;
}

int
SerialConnection::readFullyTimeout(
    void *data,int size, int toMilliSecs )
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::readFullyTimeout";

    checkState(methodName);

    /* this timer ensure that a wait for a maximun of toMilliSecs
     * will occur, plus or minus 10 milliseconds. */
    IntervalTimer timer;
    timer.start();

    int ret, amt = 0;
    do {
	if (serialLog != NULL && amt > 0)
	    serialLog->info("%s: waiting", baseMethod(methodName));
	ret = readTimeout( (char *) data + amt, size-amt,
	    static_cast<int>(toMilliSecs - timer.getSeconds() * 1000) );
	amt += ret;
    } while ( ret > 0 && amt < size );
    return amt;
}

int
SerialConnection::available() throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::available";

    checkState(methodName);
    int bytes = 0;
    int ret = ioctl( devFd, FIONREAD, &bytes );
    checkReturnStatus( methodName, "ioctl(FIONREAD)", ret );
    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), ret);
    return bytes;
}

void
SerialConnection::close() throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::close";

    if ( devFd < 0 )
	/* Silently ignore close() request when not open. */
	return;
    int ret = ::close( devFd );
    devFd = -1;
    if (serialLog != NULL)
	serialLog->info("%s", baseMethod(methodName));
    checkReturnStatus( methodName, "close", ret );
}

void
SerialConnection::discardIO() throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::discardIO";

    checkState(methodName);
    if (serialLog != NULL)
	serialLog->info("%s", baseMethod(methodName));
    if (1) {
	int ret = tcflush( devFd, TCOFLUSH );
	checkReturnStatus( methodName, "tcflush(TCIOFLUSH)", ret );
	/* Flush the input by reading to help with debugging drivers. */
	char buf[128];
	while (readTimeout(buf, sizeof(buf), 0) > 0)
	    ;
    } else {
	int ret = tcflush( devFd, TCIOFLUSH );
	checkReturnStatus( methodName, "tcflush(TCIOFLUSH)", ret );
    }
}

void
SerialConnection::drainOutput() throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::drainOutput";

    checkState(methodName);
    if (serialLog != NULL)
	serialLog->info("%s", baseMethod(methodName));
    int ret = tcdrain( devFd );
    checkReturnStatus( methodName, "tcdrain()", ret );
}

void
SerialConnection::reopen() throw (SerialConnectionException)
{
    if ( devFd != -1 ) {
	close();
    }
    open();
}


void
SerialConnection::setMode( Mode mode )
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setMode";

    checkState(methodName);

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), mode);

    struct termios options;
    int ret = tcgetattr( devFd, &options );
    checkReturnStatus( methodName, "tcgetattr", ret );

    switch( mode ) {
    case M_RAW:
	//options.c_lflag &= ~( ICANON | ECHO | ECHOE | ISIG );
	//options.c_oflag &= ~OPOST;
	 cfmakeraw(  &options );
        break;
    case M_CANONICAL:
	options.c_lflag |= ( ICANON | ECHO | ECHOE );
        break;

    case M_NONE:
    default:
	std::string msg = toStringFmt("%s: unknown input mode %d",
	    deviceName.c_str(), mode);
	logmsg.error("%s: %s", methodName, msg.c_str());
	throw SerialConnectionException(msg);
    }

    ret = tcsetattr( devFd, TCSANOW, &options );
    checkReturnStatus( methodName, "tcsetattr(TCSANOW)", ret );
}


void
SerialConnection::setTxRxBitRate( BitRate txR, BitRate rxR )
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setTxRxBitRate";

    checkState(methodName);

    if (serialLog != NULL)
	serialLog->info("%s: %s %s", baseMethod(methodName),
	    getBitRateName(txR), getBitRateName(rxR));

    struct termios options;
    int ret = tcgetattr( devFd, &options );
    checkReturnStatus( methodName, "tcgetattr", ret );

    switch( txR ) {
    case BR_NONE:
        break;
    case BR0:
	cfsetospeed( &options, B0 );
        break;
    case BR50:
	cfsetospeed( &options, B50 );
        break;
    case BR75:
	cfsetospeed( &options, B75 );
        break;
    case BR110:
	cfsetospeed( &options, B110 );
        break;
    case BR150:
	cfsetospeed( &options, B150 );
        break;
    case BR200:
	cfsetospeed( &options, B200 );
        break;
    case BR300:
	cfsetospeed( &options, B300 );
        break;
    case BR600:
	cfsetospeed( &options, B600 );
        break;
    case BR1200:
	cfsetospeed( &options, B1200 );
        break;
    case BR1800:
	cfsetospeed( &options, B1800 );
        break;
    case BR2400:
	cfsetospeed( &options, B2400 );
        break;
    case BR4800:
	cfsetospeed( &options, B4800 );
        break;
    case BR9600:
	cfsetospeed( &options, B9600 );
        break;
    case BR19200:
	cfsetospeed( &options, B19200 );
        break;
    case BR38400:
	cfsetospeed( &options, B38400 );
        break;
    case BR57600:
	cfsetospeed( &options, B57600 );
        break;
    case BR115200:
	cfsetospeed( &options, B115200 );
        break;

    default:
	std::string msg = toStringFmt("%s: unknown tx speed %d",
	    deviceName.c_str(), txR);
	logmsg.error("%s: %s", methodName, msg.c_str());
	throw SerialConnectionException(msg);
    }
    switch( rxR ) {
    case BR_NONE:
        break;
    case BR0:
	cfsetispeed( &options, B0 );
        break;
    case BR50:
	cfsetispeed( &options, B50 );
        break;
    case BR75:
	cfsetispeed( &options, B75 );
        break;
    case BR110:
	cfsetispeed( &options, B110 );
        break;
    case BR150:
	cfsetispeed( &options, B150 );
        break;
    case BR200:
	cfsetispeed( &options, B200 );
        break;
    case BR300:
	cfsetispeed( &options, B300 );
        break;
    case BR600:
	cfsetispeed( &options, B600 );
        break;
    case BR1200:
	cfsetispeed( &options, B1200 );
        break;
    case BR1800:
	cfsetispeed( &options, B1800 );
        break;
    case BR2400:
	cfsetispeed( &options, B2400 );
        break;
    case BR4800:
	cfsetispeed( &options, B4800 );
        break;
    case BR9600:
	cfsetispeed( &options, B9600 );
        break;
    case BR19200:
	cfsetispeed( &options, B19200 );
        break;
    case BR38400:
	cfsetispeed( &options, B38400 );
        break;
    case BR57600:
	cfsetispeed( &options, B57600 );
        break;
    case BR115200:
	cfsetispeed( &options, B115200 );
        break;

    default:
	std::string msg = toStringFmt("%s: unknown rx speed %d",
	    deviceName.c_str(), rxR);
	logmsg.error("%s: %s", methodName, msg.c_str());
	throw SerialConnectionException(msg);
    }
    ret = tcsetattr( devFd, TCSANOW, &options );
    checkReturnStatus( methodName, "tcsetattr(TCSANOW)", ret );
}

SerialConnection::BitRate
SerialConnection::getTxBitRate()
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::getRxBitRate";

    checkState(methodName);

    struct termios options;
    int ret = tcgetattr( devFd, &options );
    checkReturnStatus( methodName, "tcgetattr", ret );

    switch( cfgetospeed(&options) ) {
      case B0: return BR0;
      case B50: return BR50;
      case B75: return BR75;
      case B110: return BR110;
      case B150: return BR150;
      case B200: return BR200;
      case B300: return BR300;
      case B600: return BR600;
      case B1200: return BR1200;
      case B1800: return BR1800;
      case B2400: return BR2400;
      case B4800: return BR4800;
      case B9600: return BR9600;
      case B19200: return BR19200;
      case B38400: return BR38400;
      case B57600: return BR57600;
      case B115200: return BR115200;

      default:
	std::string msg = toStringFmt("%s: unknown tx speed %d",
	    deviceName.c_str(), cfgetospeed(&options));
	logmsg.error("%s: %s", methodName, msg.c_str());
	throw SerialConnectionException(msg);
    }
}

unsigned int
SerialConnection::getTxTimeUSecs(unsigned int nChars)
    throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::getTxTimeUSecs";

    checkState(methodName);

    struct termios options;
    int ret = tcgetattr( devFd, &options );
    checkReturnStatus( methodName, "tcgetattr", ret );

    unsigned int br = 0;
    switch( cfgetospeed(&options) ) {
      case B0: br = 0; break;
      case B50: br = 50; break;
      case B75: br = 75; break;
      case B110: br = 110; break;
      case B150: br = 150; break;
      case B200: br = 200; break;
      case B300: br = 300; break;
      case B600: br = 600; break;
      case B1200: br = 1200; break;
      case B1800: br = 1800; break;
      case B2400: br = 2400; break;
      case B4800: br = 4800; break;
      case B9600: br = 9600; break;
      case B19200: br = 19200; break;
      case B38400: br = 38400; break;
      case B57600: br = 57600; break;
      case B115200: br = 115200; break;

      default:
	std::string msg = toStringFmt("%s: unknown speed %d",
	    deviceName.c_str(), cfgetospeed(&options));
	logmsg.error("%s: %s", methodName, msg.c_str());
	throw SerialConnectionException(msg);
    }
    unsigned int nBits = 0;
    switch (options.c_cflag & CSIZE) {
      case CS5: nBits = 5; break;
      case CS6: nBits = 6; break;
      case CS7: nBits = 7; break;
      case CS8: nBits = 8; break;
      default:
	std::string msg = toStringFmt("%s: unknown char size %d",
	    deviceName.c_str(), options.c_cflag & CSIZE);
	logmsg.error("%s: %s", methodName, msg.c_str());
	throw SerialConnectionException(msg);
    }
    /* Stop bits. */
    nBits += (options.c_cflag & CSTOPB) ? 2 : 1;
    /* XXX Parity? included in char size or not? */

    return (unsigned int) (nBits * nChars * 1000000. / br + 0.5);
}

// number of bits in each transmission
void
SerialConnection::setBitsInData( int size )  throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setBitsInData";

    checkState(methodName);

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), size);

    struct termios options;
    int ret = tcgetattr( devFd, &options );
    checkReturnStatus( methodName, "tcgetattr", ret );

    options.c_cflag &= ~CSIZE;
    if ( size == 5 ) {
	options.c_cflag |= CS5;
    }
    else if ( size == 6 ) {
	options.c_cflag |= CS6;
    }
    else if ( size == 7 ) {
	options.c_cflag |= CS7;
    }
    else if ( size == 8 ) {
	options.c_cflag |= CS8;
    }
    else {
	std::string msg = toStringFmt("%s: bit size not valid %d",
	    deviceName.c_str(), size);
	logmsg.error("%s: %s", methodName, msg.c_str());
	throw SerialConnectionException(msg);
    }

    ret = tcsetattr( devFd, TCSANOW, &options );
    checkReturnStatus( methodName, "tcsetattr(TCSANOW)", ret );
}

const char *
SerialConnection::getBitRateName(unsigned int val)
{
    const static EnumNamer::entry_t names[] = {
	ENUM_NAMER_ENTRY(BR_NONE),
	ENUM_NAMER_ENTRY(BR0),
	ENUM_NAMER_ENTRY(BR50),
	ENUM_NAMER_ENTRY(BR75),
	ENUM_NAMER_ENTRY(BR110),
	ENUM_NAMER_ENTRY(BR150),
	ENUM_NAMER_ENTRY(BR200),
	ENUM_NAMER_ENTRY(BR300),
	ENUM_NAMER_ENTRY(BR600),
	ENUM_NAMER_ENTRY(BR1200),
	ENUM_NAMER_ENTRY(BR1800),
	ENUM_NAMER_ENTRY(BR2400),
	ENUM_NAMER_ENTRY(BR4800),
	ENUM_NAMER_ENTRY(BR9600),
	ENUM_NAMER_ENTRY(BR19200),
	ENUM_NAMER_ENTRY(BR38400),
	ENUM_NAMER_ENTRY(BR57600),
	ENUM_NAMER_ENTRY(BR115200),
    };
    static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
    return namer.getName(val);
}

SerialConnection::BitRate
SerialConnection::getBitRateByName(const char *bitRateName)
{
    const static EnumNamer::entry_t names[] = {
	ENUM_NAMER_ENTRY(BR_NONE),
	ENUM_NAMER_ENTRY(BR0),
	ENUM_NAMER_ENTRY(BR50),
	ENUM_NAMER_ENTRY(BR75),
	ENUM_NAMER_ENTRY(BR110),
	ENUM_NAMER_ENTRY(BR150),
	ENUM_NAMER_ENTRY(BR200),
	ENUM_NAMER_ENTRY(BR300),
	ENUM_NAMER_ENTRY(BR600),
	ENUM_NAMER_ENTRY(BR1200),
	ENUM_NAMER_ENTRY(BR1800),
	ENUM_NAMER_ENTRY(BR2400),
	ENUM_NAMER_ENTRY(BR4800),
	ENUM_NAMER_ENTRY(BR9600),
	ENUM_NAMER_ENTRY(BR19200),
	ENUM_NAMER_ENTRY(BR38400),
	ENUM_NAMER_ENTRY(BR57600),
	ENUM_NAMER_ENTRY(BR115200),
    };
    static EnumNamer namer(names, sizeof(names) / sizeof(names[0]));
    BitRate val;
    if (!namer.getValue(bitRateName, val)) {
	std::string s("BR");
	s.append(bitRateName);
	if (!namer.getValue(s.c_str(), val))
	    return BR_NONE;
    }
    return val;
}

void
SerialConnection::setParity( Parity p ) throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setParity";

    checkState(methodName);

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), p);

    struct termios options;
    int ret = tcgetattr( devFd, &options );
    checkReturnStatus( methodName, "tcgetattr", ret );

    switch( p ) {
    case P_EVEN:
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        break;
    case P_ODD:
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
        break;

    case P_NONE:
	options.c_cflag &= ~PARENB;
        break;

    default:
	std::string msg = toStringFmt("%s: unknown parity option %d",
	    deviceName.c_str(), p);
	logmsg.error("%s: %s", methodName, msg.c_str());
	throw SerialConnectionException(msg);
    }

    ret = tcsetattr( devFd, TCSANOW, &options );
    checkReturnStatus( methodName, "tcsetattr(TCSANOW)", ret );
}

void
SerialConnection::setStopBits( StopBits s ) throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setStopBits";

    checkState(methodName);

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), s);

    struct termios options;
    int ret = tcgetattr( devFd, &options );
    checkReturnStatus( methodName, "tcgetattr", ret );

    switch( s ) {
    case S_1:
	options.c_cflag &= ~CSTOPB;
        break;
    case S_2:
	options.c_cflag |= CSTOPB;
        break;

    case S_NONE:
    default:
	std::string msg = toStringFmt("%s: unknown stopbits %d",
	    deviceName.c_str(), s);
	logmsg.error("%s: %s", methodName, msg.c_str());
	throw SerialConnectionException(msg);
    }

    ret = tcsetattr( devFd, TCSANOW, &options );
    checkReturnStatus( methodName, "tcsetattr(TCSANOW)", ret );
}

void
SerialConnection::enableHardwareFlowControl( bool enable )
    throw (SerialConnectionException)
{
    static const char methodName[]
	 = "SerialConnection::enableHardwareFlowControl";

    checkState(methodName);

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), enable);

    struct termios options;
    int ret = tcgetattr( devFd, &options );
    checkReturnStatus( methodName, "tcgetattr", ret );

    if ( enable ) {
	options.c_cflag |= CRTSCTS;
    }
    else {
	options.c_cflag &= ~CRTSCTS;
    }

    ret = tcsetattr( devFd, TCSANOW, &options );
    checkReturnStatus( methodName, "tcsetattr(TCSANOW)", ret );
}

int
SerialConnection::getDSR() throw (SerialConnectionException )
{
    static const char methodName[] = "SerialConnection::getDSR";

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );
    return (status & TIOCM_LE) != 0;
}

void
SerialConnection::setDSR( int b ) throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setDSR";

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), b);

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );

    if ( b ) {
	status |= TIOCM_LE;
    }
    else {
	status &= ~TIOCM_LE;
    }

    ret = ioctl( devFd, TIOCMSET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMSET)", ret );
}

int
SerialConnection::getDTR() throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::getDTR";

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );
    return (status & TIOCM_DTR) != 0;
}

void
SerialConnection::setDTR( int b ) throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setDTR";

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), b);

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );

    if ( b ) {
	status |= TIOCM_DTR;
    }
    else {
	status &= ~TIOCM_DTR;
    }

    ret = ioctl( devFd, TIOCMSET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMSET)", ret );
}

int
SerialConnection::getRTS() throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::getRTS";

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );
    return (status & TIOCM_RTS) != 0;
}


void
SerialConnection::setRTS( int b ) throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setRTS";

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), b);

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );

    if ( b ) {
	status |= TIOCM_RTS;
    }
    else {
	status &= ~TIOCM_RTS;
    }

    ret = ioctl( devFd, TIOCMSET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMSET)", ret );
}

int
SerialConnection::getCTS() throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::getCTS";

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );
    return (status & TIOCM_CTS) != 0;
}

void
SerialConnection::setCTS( int b ) throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setCTS";

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), b);

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );

    if ( b ) {
	status |= TIOCM_CTS;
    }
    else {
	status &= ~TIOCM_CTS;
    }

    ret = ioctl( devFd, TIOCMSET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMSET)", ret );
}

int
SerialConnection::getDCD() throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::getDCD";

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );
    return (status & TIOCM_CAR) != 0;
}

void
SerialConnection::setDCD( int b ) throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setDCD";

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), b);

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );

    if ( b ) {
	status |= TIOCM_CAR;
    }
    else {
	status &= ~TIOCM_CAR;
    }

    ret = ioctl( devFd, TIOCMSET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMSET)", ret );
}

int
SerialConnection::getRNG() throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::getRNG";

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );
    return (status & TIOCM_RNG) != 0;
}

void
SerialConnection::setRNG( int b ) throw (SerialConnectionException)
{
    static const char methodName[] = "SerialConnection::setRNG";

    if (serialLog != NULL)
	serialLog->info("%s: %d", baseMethod(methodName), b);

    int status = 0;
    int ret = ioctl( devFd, TIOCMGET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMGET)", ret );

    if ( b ) {
	status |= TIOCM_RNG;
    }
    else {
	status &= ~TIOCM_RNG;
    }

    ret = ioctl( devFd, TIOCMSET, &status );
    checkReturnStatus( methodName, "ioctl(TIOCMSET)", ret );
}
