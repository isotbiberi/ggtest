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
#include "EnumNamer.h"
#include <string.h>
#include <string>

const char *
EnumNamer::getName(enum_t val) const
{
    /* If they happen to be sorted... */
    if ((size_t) val < nentries && entries[val].value == val)
	return entries[val].name;

    for (size_t i = 0; i < nentries; ++i)
	if (entries[i].value == val)
	    return entries[i].name;

    /* Note: change buffer size of message changed. */
    /* 17 chars. */
    char *buf = bufs[bufIndex];
    bufIndex = (bufIndex + 1) % NBUFS;
    snprintf(buf, sizeof(bufs[0]), "[unknown value %u]", (size_t) val);
    return buf;
}

std::string
EnumNamer::getBitSetNames(enum_t val) const
{
    std::string s;

    if (val == 0)
	return "0";

    for (size_t i = 0; i < nentries; ++i)
	if (entries[i].value & val) {
	    if (s.length() > 0)
		s += ',';
	    s += entries[i].name;
	    val &= ~entries[i].value;
	    if (val == 0)
	    	break;
	}
    if (val != 0) {
	if (s.length() > 0)
	    s += ',';
	char buf[32];
	snprintf(buf, sizeof(buf), "0x%x", val);
	s += buf;
    }
    return s;
}

std::string
EnumNamer::getBitVectorNames(unsigned int val) const
{
    std::string s;

    if (val == 0)
	return "0";

    for (size_t i = 0; i < nentries; ++i)
	if ((1 << entries[i].value) & val) {
	    if (s.length() > 0)
		s += ',';
	    s += entries[i].name;
	    val &= ~(1 << entries[i].value);
	    if (val == 0)
	    	break;
	}
    if (val != 0) {
	if (s.length() > 0)
	    s += ',';
	char buf[32];
	snprintf(buf, sizeof(buf), "0x%x", val);
	s += buf;
    }
    return s;
}

bool
EnumNamer::getValue_(const char *name, enum_t &val) const
{
    for (size_t i = 0; i < nentries; ++i) {
	if (strcmp(entries[i].name, name) == 0) {
	    val = entries[i].value;
	    return true;
	}
    }
    val = 0;
    return false;
}
