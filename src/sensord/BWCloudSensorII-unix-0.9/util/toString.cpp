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
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include "util/toString.h"

#define B_SIZE  (8 * 1024)

std::string
toString(short val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%hd", val);
    return std::string(buf);
}

std::string
toString(unsigned short val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%hu", val);
    return std::string(buf);
}

std::string
toString(int val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%d", val);
    return std::string(buf);
}

std::string
toString(unsigned int val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%u", val);
    return std::string(buf);
}

std::string
toString(long val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%ld", val);
    return std::string(buf);
}

std::string
toString(unsigned long val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%lu", val);
    return std::string(buf);
}

std::string
toString(long long val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%Ld", val);
    return std::string(buf);
}

std::string
toString(unsigned long long val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%Lu", val);
    return std::string(buf);
}

std::string
toString(double val)
{
    char buf[128];
    snprintf(buf, sizeof(buf), "%g", val);
    return std::string(buf);
}

std::string
toString(double val, int precision)
{
    char buf[128];
    snprintf(buf, sizeof(buf), "%.*lf", precision, val);
    return std::string(buf);
}

std::string
toString(const char *val)
{
    if (val)
	return std::string(val);
    else
	return std::string("(null)");
}

const char *
toStringVisibleNull(const char *str)
{
    return str == NULL ? "(null)" : str;
}

std::string
toStringDump(const void *val, unsigned int len)
{
    if (val == NULL)
	return "(null)";

    const unsigned char *v = static_cast<const unsigned char *>(val);
    std::string s;
    s.reserve(len * 2 + 1);
    for (unsigned int i = 0; i < len; ++i) {
	if (v[i] == '\\')
	    s.append("\\\\");
	else if (v[i] == '"')
	    s.append("\\\"");
	else if (v[i] == '\'')
	    s.append("\\'");
	else if (isprint(v[i]))
	    s.append(1, v[i]);
	else if (v[i] == '\n')
	    s.append("\\n");
	else if (v[i] == '\r')
	    s.append("\\r");
	else if (v[i] == '\t')
	    s.append("\\t");
	else {
	    char buf[10];
	    snprintf(buf, sizeof(buf), "\\x%02x", v[i]);
	    s.append(buf);
	}
    }
    return s;
}

std::string
toStringHexDump(const void *val, unsigned int len)
{
    if (val == NULL)
	return "(null)";

    const unsigned char *v = static_cast<const unsigned char *>(val);
    std::string s;
    s.reserve(len * 2 + 1);
    char buf[10];
    for (unsigned int i = 0; i < len; ++i) {
	snprintf(buf, sizeof(buf), "%02x", v[i]);
	s.append(buf);
    }
    return s;
}

std::string
toStringAddrHexDump(const void *val, unsigned int len)
{
    if (val == NULL)
	return "(null)";

    const unsigned char *v = static_cast<const unsigned char *>(val);
    const unsigned int BYTES_PER_LINE = 16;
    std::string s;
    s.reserve((len + BYTES_PER_LINE - 1) / BYTES_PER_LINE * 6 + len * 3 + 1);
    char buf[10];
    for (unsigned int i = 0; i < len; i += BYTES_PER_LINE) {
	if (i != 0)
	    s.append("\n");
	snprintf(buf, sizeof(buf), "%04x:", i);
	s.append(buf);
	for (unsigned int j = 0; j < BYTES_PER_LINE && i + j < len; ++j) {
	    snprintf(buf, sizeof(buf), " %02x", v[i + j]);
	    s.append(buf);
	}
    }
    return s;
}

std::string
toStringFmt(const char *fmt, ...)
{
    va_list args;
    char buf[B_SIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    return std::string(buf);
}

const char toStringTimeTDefaultFmt[] = "%b %e %T %Y";
std::string
toStringTimeT(time_t t, const char *fmt)
{
    char buf[256];
    if (t == 0)
	return "0";
    size_t n = strftime(buf, sizeof(buf), fmt, gmtime(&t));
    if (n == 0)
	return toString(t);
    return std::string(buf);
}

/* Note: %/ is local hack for thousandth of seconds. */
const char toStringTimeValDefaultFmt[] = "%b %e %T.%/ %Y";
std::string
toString(const struct timeval &tv, const char *fmt)
{
    char buf[256];
    if (tv.tv_sec == 0 && tv.tv_usec == 0)
	return "0";
    size_t formatSize = strlen(fmt) + 1 + 1 + 10; // null; 3-2; +10 for safety
    char format[formatSize];
    const char *p;

    /* Hack to replace %/ with 3 digit fraction of second. */
    if ((p = strstr(fmt, "%/")) != NULL)
	snprintf(format, formatSize, "%.*s%03d%s",
	    p - fmt, fmt, (int) (tv.tv_usec / 1000), p + 2);
    else
	strcpy(format, fmt);
    size_t n = strftime(buf, sizeof(buf), format, gmtime(&tv.tv_sec));
    if (n == 0)
	return toString(tv.tv_sec);
    return std::string(buf);
}
