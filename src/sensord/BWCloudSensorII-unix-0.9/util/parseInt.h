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
#ifndef UTIL_PARSE_INT_H
# define UTIL_PARSE_INT_H

# include "util/myt-limits.h"

extern bool parseInt(const char *str, int &val, int base = 0,
	int minVal = INT_MIN, int maxVal = INT_MAX);
extern bool parseUInt(const char *str, unsigned int &val, int base = 0,
	unsigned int minVal = 0, unsigned int maxVal = UINT_MAX);
extern bool parseLong(const char *str, long &val, int base = 0,
	long minVal = LONG_MIN, long maxVal = LONG_MAX);
extern bool parseULong(const char *str, unsigned long &val, int base = 0,
	unsigned long minVal = 0, unsigned long maxVal = ULONG_MAX);
extern bool parseLongLong(const char *str, long long &val, int base = 0,
	long long minVal = LONG_LONG_MIN, long long maxVal = LONG_LONG_MAX);
extern bool parseULongLong(const char *str, unsigned long long &val,
	int base = 0, unsigned long long minVal = LONG_LONG_MIN,
	unsigned long long maxVal = LONG_LONG_MAX);

extern bool parseBool(const char *str, bool &val);

#endif /* UTIL_PARSE_INT_H */
