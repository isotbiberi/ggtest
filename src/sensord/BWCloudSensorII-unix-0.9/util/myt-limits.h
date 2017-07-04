/*
    Copyright (c) 2007-2011 MyTelescope.com
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
#ifndef MYT_LIMITS_H
# define MYT_LIMITS_H

# include <limits.h>

/* Hack for some versions of gcc (yuck) */
#ifndef LONG_LONG_MAX
# define LONG_LONG_MAX __LONG_LONG_MAX__
#endif
#ifndef LONG_LONG_MIN
# define LONG_LONG_MIN (-__LONG_LONG_MAX__ - 1)
#endif

#endif /* MYT_LIMITS_H */
