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
#include <sys/types.h>
#include <errno.h>
#include <linux/unistd.h> /* for __NR_* */
#include <unistd.h> /* for getpid() */
#include "util/getThreadID.h"

/* Yuck.  Another option might be to use pthread_self() but this is
 * hard to relate to numbers that show up in ps(1) output.
 */
#if defined(_syscall0) && defined(__NR_gettid)
_syscall0(pid_t,gettid)
# define GETTID() gettid()
#else /* _syscall0 && __NR_gettid */
# define GETTID() getpid()
#endif /* _syscall0 && __NR_gettid */

int
getThreadID()
{
    return GETTID();
}
