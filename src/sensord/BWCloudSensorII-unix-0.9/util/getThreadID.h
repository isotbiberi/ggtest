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
#ifndef UTIL_GET_THREAD_ID_H
# define UTIL_GET_THREAD_ID_H
 
/* For linux kernels 2.4 (and below?), getpid() would return a unique
 * number for each thread.  In 2.6 (and above), all threads in the
 * same process have the same PID, but there is a system call,
 * gettid(), that returns the thread-id.
 * The thread-id is more useful in the log messages, so use it if available.
 */
extern int getThreadID();

#endif /* UTIL_GET_THREAD_ID_H */
