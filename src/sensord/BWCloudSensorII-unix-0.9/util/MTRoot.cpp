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
#include <cstdlib>

#include "util/MTRoot.h"


#define MT_ROOT_DEFAULT		"/opt/mt"
/* Environment variable that can override MT_ROOT_DEFAULT */
#define MT_ROOT_ENV		"MTROOT"

/*
 * Get a string containing the mytelescope root directory.
 * Uses MTROOT environment variable, defaults to "/opt/mt".
 */
const std::string &
getMTRoot()
{
    static std::string mtRoot;
    if (mtRoot.size() == 0) {
	const char *env = getenv(MT_ROOT_ENV);
	mtRoot = env && *env ? env : MT_ROOT_DEFAULT;
    }
    return mtRoot;
}
