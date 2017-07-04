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
#ifndef UTIL_FILEDESC_H
# define UTIL_FILEDESC_H
#include <unistd.h>

/* Class to represent a file descriptor used by several classes.
 * Keeps track of whether the thing is closed (avoids problems with
 * multiple closes).
 */
class FileDesc {
  private:
    int fd;
    bool closed;

  public:
    FileDesc(int fd = -1) : fd(fd), closed(fd < 0) {
    }
    ~FileDesc() {
	close();
    }
    int release() {
	int tfd = fd;
	fd = -1;
	closed = true;
	return tfd;
    }
    void setFD(int fd) {
	close();
	this->fd = fd;
	closed = fd < 0;
    }
    int getFD() const {
	return closed ? -1 : fd;
    }
    bool isClosed() const {
	return closed;
    }
    int close() {
	/* Someone else already closed - ignore */
	if (closed)
	    return 0;
	closed = true;
	return ::close(fd);
    }
    /* Returns true if operation succeeds, false otherwise. */
    bool setNonBlocking(bool value, bool *oldValuep = NULL);

  private:
    /* No copying or assigning. */
    FileDesc(const FileDesc &f);
    FileDesc &operator=(const FileDesc &f);
};

#endif /* UTIL_FILEDESC_H */
