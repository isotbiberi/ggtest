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
#ifndef UTIL_GENERICEXCEPTION_H
# define UTIL_GENERICEXCEPTION_H

# include <string>
# include <stdexcept>		/* to make throw(xxx,exception) easy to write */
# include "util/toString.h"	/* Used for building error messages */

/*
 * All exceptions derived from this.
 *
 * Normally, this would be derived form std::exception... However...
 * The C++ behaviour for methods that are declared to throw() exceptions
 * is that if something else is thrown (run time check), unexpected()
 * is called which terminates the program.  Not nice.
 * To deal with this, all methods that have a throw() clause should
 * be declared as throw(FooException,BarException,exception) (ie,
 * always have "exception" in the list).  This will allow exceptions
 * as a result of run-time checking (eg, bad_alloc) to `pass through'
 * the method without terminating the progam.  Had GenericException
 * been derived from exception, such a throw() clause would be useless.
 * Java avoids this problem by having a cleaner exception class hierarchy
 * (distinction between errors, run-time exceptions, etc.).
 */
class GenericException {
  private:
    string message;

  public:
    /* Note: no default constructor - you should always provide a useful
     * string so uncaught exceptions can be identified.
     */
    GenericException(const string &message);

    virtual ~GenericException();

    virtual const string &getMessage() const;
};

/*
 * All exceptions that aren't expected to be caught are derived from this.
 */
class RuntimeException : public GenericException {
  public:
    RuntimeException(const string &m) : GenericException(m) {
    }
};

class IllegalStateException : public RuntimeException {
  public:
    IllegalStateException(const string &m) : RuntimeException(m) {
    }
};

class IllegalArgumentException : public RuntimeException {
  public:
    IllegalArgumentException(const string &m) : RuntimeException(m) {
    }
};

class NumberFormatException : public RuntimeException {
  public:
    NumberFormatException(const string &m) : RuntimeException(m) {
    }
};

#endif /* UTIL_GENERICEXCEPTION_H */
