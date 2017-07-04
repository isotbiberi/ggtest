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
#ifndef UTIL_TOSTRING_H
# define UTIL_TOSTRING_H

#include <string>
#include <sstream>
#include <time.h>
#include <sys/time.h> /* struct timeval */
#include <cstring>

# include "util/GCCAttributes.h"

using namespace std;

/*
 * Handy routines for building error messages in strings.
 */
std::string toString(short val);
std::string toString(unsigned short val);
std::string toString(int val);
std::string toString(unsigned int val);
std::string toString(long val);
std::string toString(unsigned long val);
std::string toString(long long val);
std::string toString(unsigned long long val);
std::string toString(double val);
std::string toString(double val, int precision);
std::string toString(const char *val);
const char *toStringVisibleNull(const char *str);
/* Convert a block of data to a printable string. */
std::string toStringDump(const void *val, unsigned int len);
extern inline
std::string toStringDump(const void *val) {
    return toStringDump(val, val != NULL ? strlen((const char *) val) : 0);
}
extern inline
std::string toStringDump(const std::string &str) {
    return toStringDump(str.c_str(), str.size());
}
/* straight hex dump - no addresses, no spaces between bytes. */
std::string toStringHexDump(const void *val, unsigned int len);
/* Pretty(er) hex dump: addresses and spaces between bytes, 16 bytes/line. */
std::string toStringAddrHexDump(const void *val, unsigned int len);
std::string toStringFmt(const char *fmt, ...)
	GCC_FUNC_ATTR(format(printf, 1, 2));
extern const char toStringTimeTDefaultFmt[];
std::string toStringTimeT(time_t t, const char *fmt = toStringTimeTDefaultFmt);
extern const char toStringTimeValDefaultFmt[];
std::string toString(const struct timeval &tv,
	const char *fmt = toStringTimeValDefaultFmt);

template<class C>
std::string toStringPrintState(const C *c)
{
    std::ostringstream os;
    c->printState(os, 4, 4);
    return os.str();
}

#endif /* UTIL_TOSTRING_H */
