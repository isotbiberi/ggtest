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

#include <cstdlib>
#include <cstring>

#include "util/FileDesc.h"
#include "util/Log.h"


/* No copying or assigning. */
FileDesc::FileDesc(const FileDesc &f)
{
    abort();
}

FileDesc &
FileDesc::operator=(const FileDesc &f)
{
    abort();
    return *this;
}

bool
FileDesc::setNonBlocking(bool value, bool *oldValuep)
{
    static const char methodName[] = "FileDesc::setNonBlocking";

    int oldFlags;

    if ((oldFlags = fcntl(getFD(), F_GETFL)) < 0) {
	logmsg.error("%s: error getting non-blocking flag - %s",
	    methodName, strerror(errno));
	return false;
    }
    if (oldValuep != NULL)
	*oldValuep = (oldFlags & O_NONBLOCK) ? true : false;

    int newFlags = oldFlags;
    if (value)
	newFlags |= O_NONBLOCK;
    else
	newFlags &= ~O_NONBLOCK;

    if (newFlags != oldFlags && fcntl(getFD(), F_SETFL, newFlags) < 0) {
	logmsg.error("%s: error setting non-blocking flag - %s",
	    methodName, strerror(errno));
	return false;
    }

    return true;
}
