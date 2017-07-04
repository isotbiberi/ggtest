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
#ifndef UTIL_GCC_ATTRIBUTES_H
# define UTIL_GCC_ATTRIBUTES_H

# ifdef __GNUC__
#  define GCC_FUNC_ATTR(x) __attribute__((x))
#  define GCC_VAR_ATTR(x) __attribute__((x))
# else
#  define GCC_FUNC_ATTR(x) 
#  define GCC_VAR_ATTR(x) 
# endif /* __GNUC__ */

#endif /* UTIL_GCC_ATTRIBUTES_H */
