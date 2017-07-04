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
#ifndef UTIL_MTROOT_H
# define UTIL_MTROOT_H

#include <string>

/*
 * Get a string containing the mytelescope root directory.
 * Uses MTROOT environment variable, defaults to "/opt/mt".
 */
extern const std::string &getMTRoot();

#define MTROOT_BIN_DIR			"/bin/"
#define MTROOT_VAR_DIR			"/var/"
#define MTROOT_TMP_DIR			MTROOT_VAR_DIR "tmp/"
/* Where log files go. */
#define MTROOT_LOG_DIR			MTROOT_VAR_DIR "log/"
/* What directory the config files live in, relative to MTROOT
 * (used by util/ConfigReader_File).
 */
#define MTROOT_CONFIG_DIR		"/config/"
/* What directory the pid files live in, relative to MTROOT
 * (used by various daemons that want to record their pid somewhere).
 */
#define MTROOT_PID_DIR			MTROOT_VAR_DIR "run/"
/* What directory contains subdirectories in which core files are to
 * be dropped, relative to MTROOT
 * (used by various daemons that want to chdir() somewhere unique so
 * core files are kept).
 */
#define MTROOT_CORES_DIR		MTROOT_VAR_DIR "cores/"
/* What directory translation files (message catalogs) live in.
 */
#define MTROOT_LOCALE_DIR		"/locale/"

#endif /* UTIL_MTROOT_H */
