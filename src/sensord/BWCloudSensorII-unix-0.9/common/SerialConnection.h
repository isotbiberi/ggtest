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
#ifndef SERIAL_CONNECTION_H
#define SERIAL_CONNECTION_H

#include <string>
#include <iostream>
#include "util/GenericException.h"
#include "util/Log.h"

class SerialConnectionException : public GenericException {
public:
    SerialConnectionException( const std::string &msg )
      : GenericException(msg) {}
};

class SerialConnection {
private:
    std::string deviceName;
    int devFd;
    Log *serialLog;

    void open() throw (SerialConnectionException);
    void checkState(const char *callersName)
	throw (SerialConnectionException);
    void checkReturnStatus( const char *callersName,
	    const char *operation, int ret )
	throw (SerialConnectionException);
public:
    SerialConnection();
    SerialConnection( const std::string &device, bool doLogging = true );
    ~SerialConnection();

    void printState(std::ostream &os, int indent, int indentIncr) const;

    /* Change whether logging is enabled/disabled. */
    void setLogging(bool value);

    const std::string &getDeviceName() const;
    /* Effects following open. */
    void setDeviceName(const std::string &deviceName);

    int getFD() const 
    {
        return devFd;
    }
    bool isOpen() const {
	return devFd >= 0;
    }

    int write( const void *data, int size )
	throw (SerialConnectionException);
    void writeFully( const void *data, int size )
	throw (SerialConnectionException);

    int read( void *data, int size )
	throw (SerialConnectionException);
    int readTimeout( void *data, int size, int toMilliSecs)
	throw (SerialConnectionException);
    int readTimeoutDelim(void *data, int size,
	    int toMilliSecs, unsigned char delimChar)
	throw (SerialConnectionException);
    int readFully( void *data, int size )
	throw (SerialConnectionException);
    int readFullyTimeout( void *data, int size, int toMilliSecs)
	throw (SerialConnectionException);

    int available() throw (SerialConnectionException);

    void close() throw (SerialConnectionException);
    void reopen() throw (SerialConnectionException);

    void discardIO() throw (SerialConnectionException);  
    void drainOutput() throw (SerialConnectionException);  

    enum Mode { M_NONE, M_RAW, M_CANONICAL };
    void setMode( Mode mode ) throw (SerialConnectionException);

    enum BitRate { BR_NONE, BR0, BR50, BR75, BR110, BR150, BR200, BR300, BR600,
                   BR1200, BR1800, BR2400, BR4800, BR9600, BR19200, BR38400,
		   BR57600, BR115200 };
				                                    
    void setTxRxBitRate( BitRate txR, BitRate rxR )
	throw (SerialConnectionException);
    void setBitRate( BitRate r ) throw (SerialConnectionException) {
	setTxRxBitRate(r, r);
    }
    BitRate getTxBitRate() throw (SerialConnectionException);
    unsigned int getTxTimeUSecs(unsigned int nChars)
	throw (SerialConnectionException);
    static const char *getBitRateName(unsigned int val);
    static BitRate getBitRateByName(const char *bitRateName);

    // number of bits in each transmission
    void setBitsInData( int size )  throw (SerialConnectionException);

    enum Parity { P_NONE, P_EVEN, P_ODD, /* P_MARK, P_SPACE */};
    void setParity( Parity p ) throw (SerialConnectionException);

    enum StopBits { S_NONE, S_1, S_2 };
    void setStopBits( StopBits s ) throw (SerialConnectionException);

    void enableHardwareFlowControl( bool enable )
	throw (SerialConnectionException);

    int getDSR() throw (SerialConnectionException);
    void setDSR( int b ) throw (SerialConnectionException);

    int getDTR() throw (SerialConnectionException);
    void setDTR( int b ) throw (SerialConnectionException);

    int getRTS() throw (SerialConnectionException);
    void setRTS( int b ) throw (SerialConnectionException);

    int getCTS() throw (SerialConnectionException);
    void setCTS( int b ) throw (SerialConnectionException);

    int getDCD() throw (SerialConnectionException);
    void setDCD( int b ) throw (SerialConnectionException);

    int getRNG() throw (SerialConnectionException);
    void setRNG( int b ) throw (SerialConnectionException);
};

#endif
