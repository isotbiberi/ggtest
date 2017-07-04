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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "util/parseInt.h"

bool
parseInt(const char *str, int &val, int base, int minVal, int maxVal)
{
    char *eptr;
    long lval;

    errno = 0;
    lval = strtol(str, &eptr, base);
    val = lval;
    if (errno != 0 || eptr == str || *eptr || lval < minVal || lval > maxVal)
	return false;
    return true;
}

bool
parseUInt(const char *str, unsigned int &val, int base,
	    unsigned int minVal, unsigned int maxVal)
{
    char *eptr;
    unsigned long lval;

    errno = 0;
    lval = strtoul(str, &eptr, base);
    val = lval;
    if (errno != 0 || eptr == str || *eptr || lval < minVal || lval > maxVal)
	return false;
    return true;
}

bool
parseLong(const char *str, long &val, int base, long minVal, long maxVal)
{
    char *eptr;

    errno = 0;
    val = strtol(str, &eptr, base);
    if (errno != 0 || eptr == str || *eptr || val < minVal || val > maxVal)
	return false;
    return true;
}

bool
parseULong(const char *str, unsigned long &val, int base,
	    unsigned long minVal, unsigned long maxVal)
{
    char *eptr;

    errno = 0;
    val = strtoul(str, &eptr, base);
    if (errno != 0 || eptr == str || *eptr || val < minVal || val > maxVal)
	return false;
    return true;
}

bool
parseLongLong(const char *str, long long &val, int base,
    long long minVal, long long maxVal)
{
    char *eptr;

    errno = 0;
    val = strtoll(str, &eptr, base);
    if (errno != 0 || eptr == str || *eptr || val < minVal || val > maxVal)
	return false;
    return true;
}

bool
parseULongLong(const char *str, unsigned long long &val, int base,
    unsigned long long minVal, unsigned long long maxVal)
{
    char *eptr;

    errno = 0;
    val = strtoull(str, &eptr, base);
    if (errno != 0 || eptr == str || *eptr || val < minVal || val > maxVal)
	return false;
    return true;
}

bool
parseBool(const char *str, bool &val)
{
    if (strcmp(str, "false") == 0 || strcmp(str, "no") == 0
	|| strcmp(str, "off") == 0 || strcmp(str, "0") == 0)
    {
	val = false;
	return true;
    }
    if (strcmp(str, "true") == 0 || strcmp(str, "yes") == 0
	|| strcmp(str, "on") == 0 || strcmp(str, "1") == 0)
    {
	val = true;
	return true;
    }
    val = false;
    return false;
}
