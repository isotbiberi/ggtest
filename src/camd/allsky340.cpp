/* 
 * Dummy camera for testing purposes.
 * Copyright (C) 2005-2007 Petr Kubanek <petr@kubanek.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "camd.h"
#include "utilsfunc.h"
#include "rts2fits/image.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <sstream>
#include <string>
#include <iomanip>
#include<vector>



namespace rts2camd
{

/**
 * Class for a dummy camera.
 *
 * @author Petr Kubanek <petr@kubanek.net>
 */
class allsky340:public Camera
{
	public:
		allsky340 (int in_argc, char **in_argv);

		virtual ~allsky340 (void);
		
		

		virtual int processOption (int in_opt)
		{
			switch (in_opt)
			{
				default:
					return Camera::processOption (in_opt);
			}
			return 0;
		}
		virtual int initHardware ();

		virtual int initChips ()
		{
			setSize (640,480, 0,0);
			return Camera::initChips ();
		}
		
	
		virtual int startExposure ();

		virtual int stopExposure();

		virtual long isExposing ();

		virtual int doReadout ();

	protected:

		virtual int setValue (rts2core::Value *old_value, rts2core::Value *new_value);


	private:

		int width;
		int height;
		int sendByteCommand(std::vector<unsigned char>byte,int fd,int length);
		std::string readAllSky(int fd);
		unsigned char readOneChar(int fd);
		int set_interface_attribs(int fd, int speed);
		int hexToInt(std::string hex);
		std::string intToHex(int number);
		std::string charToAscii(char c);
		std::bitset<8> readCharByChar(std::string str,std::bitset<8> checksum);
		char calculateChecksum(std::vector<unsigned char> bytes);
		std::string byteToHex(int intValue);
		std::vector<unsigned char> prepareCommand(std::string commandStr);
		std::string convertExptime(double exptime);

		int fd;
		std::string cameraHex;
		//cameraHex=byteToHex(int('T')) +" "+ byteToHex(hexToInt(exp.substr(0,2)))+" " + byteToHex(hexToInt(exp.substr(2,2)))+" " + byteToHex(hexToInt(exp.substr(4,2)))+" " +byteToHex(0)+" " +byteToHex(1);

		std::string startToTransferImage = byteToHex(int('X'));//after exposure done
		std::string goOnDownloading = byteToHex(int('K'));//go on if indeed checksum okay
		std::string stopDownloading = byteToHex(int('S'));//to stop download
		std::string abortCapture = byteToHex(int('A'));//abort capture
};

};

using namespace rts2camd;
using namespace std;
int allsky340::sendByteCommand(std::vector<unsigned char>byte,int fd,int length)
{
	//Create byte array
	  unsigned char *send_bytes = &byte[0];



        int wlen;
		wlen= write(fd, send_bytes, length);  //Send data
		if (wlen != length) {
				logStream(MESSAGE_INFO)<<"Error from write:"<<wlen<<errno<<sendLog;
			}
		printf("Wrote the bytes. \n");

      return 0;
}

std::string allsky340::readAllSky(int fd)
{
 unsigned char buf[8192];
 std::string commandReturn="";
 int rdlen ;


			rdlen = read(fd, buf,8192);
			if (rdlen > 0) {


			buf[rdlen] = 0;

			}
			else if (rdlen < 0)
			{
				printf("Error from read: %d: %s\n", rdlen, strerror(errno));

			}

			 commandReturn.append(reinterpret_cast<const char*>(buf));
			// std::cout<<"Reading till semicolon"<<std::endl;


			 //std::cout<<"Readed "<<commandReturn<<" in readAllSky length is"<<rdlen<<std::endl;
			 return commandReturn;



}

unsigned char allsky340::readOneChar(int fd)
{
	unsigned char firstChar[1];
	//std::cout<<"read one char started "<<std::endl;
	int rdlen = read(fd, firstChar, 1);

		if (rdlen > 0) {
			return firstChar[0];

		}
		else if (rdlen < 0)
		{
		 printf("Error from read: %d: %s\n", rdlen, strerror(errno));
         exit(0);
		}
//std::cout<<" Reading one char "<<firstChar[1]<<std::endl;
return firstChar[0];
}

int allsky340::set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}
int allsky340::hexToInt(std::string hex)
{
	unsigned int x;
	std::stringstream ss;
	ss << std::hex << hex;
	ss >> x;
	//std::cout<<"x is "<<x<<endl;
	return x;
	//cout<<"e is "<<ss._to<<endl;
}
std::string allsky340::intToHex(int number)
{

	std::stringstream stream;
	stream << std::hex << number;
	std::string result( stream.str() );
	//cout<<"result is "<<result<<endl;
	return result;
}
std::string allsky340::charToAscii(char c)
{

	std::stringstream stream;
	stream << std::hex << int(c);
	std::string result( stream.str() );

	return result;
}
bitset<8> allsky340::readCharByChar(std::string str,bitset<8> checksum)//hex string of only one char
{
	std::stringstream strm,strm2;
	std::string buf="0x";
	unsigned n;
	//bitset<8>checksum(int(0));
	for(int i=0;i<str.size();i++)
		{
        buf="0x";
        strm.clear();

        strm.str("");
		//cout<<"char is "<<str[i]<<endl;
		buf+=str[i];
		strm<<hex<<buf;

		strm>>n;
		bitset<4> b(n);

		b=~b;

		strm2<<b;
		}

	bitset<8> reversedBits(strm2.str());

	checksum=checksum^reversedBits;

	checksum.reset(7);//always make msb zero

	return checksum;

}

char allsky340::calculateChecksum(std::vector<unsigned char> bytes)
{
	//cout<<"deneme "<<bytes.size()<<endl;
	std::string asc;
	bitset<8> checksum=0;
	for(auto i : bytes)
	{
	  //cout << "byte is " <<std::hex<<int(i) <<endl;
	  asc =byteToHex(int(i));//burası yanlış
	  //cout<<"asc is "<<asc<<endl;
	  checksum=readCharByChar(asc,checksum);

	}
	unsigned long ul1 = checksum.to_ulong();
		std::string checksumHex=intToHex(ul1);
		//cout<<"checksum is "<<checksumHex<<" char is "<<char(hexToInt(checksumHex))<<endl;
	    return char(hexToInt(checksumHex));
}
std::string allsky340::byteToHex(int intValue)
{
	std::stringstream stream;

	stream << std::setfill ('0') << std::setw(sizeof(char)*2)
	       << std::hex << intValue;
	std::string result=stream.str();

    return result;
}
std::vector<unsigned char> allsky340::prepareCommand(std::string commandStr)
{
	std::istringstream hex_chars_stream(commandStr);
	std::vector<unsigned char> bytes;

	unsigned int c;
	while (hex_chars_stream >> std::hex >> c)
	{
	    bytes.push_back(c);
	    //cout<<"ekledi"<<endl;
	}

	bytes.push_back(int(calculateChecksum(bytes)));

	return bytes;
}
std::string allsky340::convertExptime(double exptime)//3 bytes conversion
{
	   //int second = 1;
	    long microSec = exptime *10000;//100microsecs resolution
	    //cout<<"microsec is "<<microSec<<endl;
	    ostringstream oss;
	    oss <<std::setfill ('0') << std::setw(6)<<hex<< microSec;
	    //cout<<oss.str()<<endl;
	    std::string str = oss.str();
	    //cout<<"ilk"<<str.substr(0,2)<<"ikinci"<<str.substr(2,2)<<"ucuncu"<<str.substr(4,2)<<endl;
	    //cout<<"ilk"<<hexToInt(str.substr(0,2))<<"ikinci"<<hexToInt(str.substr(2,2))<<"ucuncu"<<hexToInt(str.substr(4,2));
	    return str;
}


allsky340::allsky340 (int in_argc, char **in_argv):Camera (in_argc, in_argv)
{

}
allsky340::~allsky340()
{
	close(fd);

}

int allsky340::initHardware()
{

	const char *portname = "/dev/ttyUSB0";




	 fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	    if (fd < 0) {
	        logStream(MESSAGE_INFO)<<"Error opening "<< portname<<strerror(errno)<<sendLog;
	        return -1;
	    }
	    /*baudrate 19200, 8 bits, no parity, 1 stop bit */
	    set_interface_attribs(fd, B460800);

	    return initChips();
}
int allsky340::startExposure()

{
	std::string exp =convertExptime(getExposure());
	cameraHex=byteToHex(int('T')) +" "+ byteToHex(hexToInt(exp.substr(0,2)))+" " + byteToHex(hexToInt(exp.substr(2,2)))+" " + byteToHex(hexToInt(exp.substr(4,2)))+" " +byteToHex(0)+" " +byteToHex(1);
	sendByteCommand(prepareCommand(cameraHex),fd,7);
	return 0;
}

int allsky340::stopExposure()

{
	sendByteCommand(prepareCommand(abortCapture),fd,2);
	return 0;
}

long allsky340::isExposing()
{
	clearTelMove();
	//logStream(MESSAGE_INFO)<<"isExposing calisti"<<sendLog;
	long ret;
	ret = Camera::isExposing();
	if(ret>0)
	return ret;
unsigned char testChar=readOneChar(fd);
if(testChar!='D')
{
//	logStream(MESSAGE_INFO)<<"test char is "<<testChar<<sendLog; 
	return 200;
}
return -2;
}
int allsky340::setValue (rts2core::Value *old_value, rts2core::Value *new_value)
{
	return Camera::setValue (old_value, new_value);
}



int allsky340::doReadout ()
{

	//logStream(MESSAGE_INFO)<<"doReadout calisti"<<sendLog;
	sendByteCommand(prepareCommand(startToTransferImage),fd,2);
	unsigned char image[8192*75];
	long sayac=0;
	unsigned char testChar;
	for(int i=0;i<75;i++)//307200 pixel read 4096 for each time one pixe is two bytes so read 4096*2=8192 bytes
	{
	//image+=readAllSky(fd);
		for(int k=0;k<4096;k++)
			{
			testChar=readOneChar(fd);
			//std::cout<<"test return is "<<(int)testChar<<" k is " <<k<<std::endl;
			//image[k]=testChar;
			image[sayac+1]=testChar;
			testChar=readOneChar(fd);
			image[sayac]=testChar;
			sayac=sayac+2;

			}
	readOneChar(fd);//this is checksum and it should be checked
	sendByteCommand(prepareCommand(goOnDownloading),fd,2);
	//logStream(MESSAGE_INFO)<<i<<" part completed"<<sendLog;
	}

	 unsigned char * imageBuffer;
	 imageBuffer = &image[0];
	 //logStream(MESSAGE_INFO)<<"sendReadout oncesi"<<sendLog;
	 
	  int ret =  sendReadoutData((char*)imageBuffer,614400);//getWriteBinaryDataSize());
	    if(ret<0)
	    {

	        logStream (MESSAGE_ERROR) << "Send read out data yapilamadi " << sendLog;
	        return -1;
	    }
	  
	    logStream (MESSAGE_INFO) << " Allsky data postalandi  " <<sendLog;

	    return -2;


}


int main (int argc, char **argv)
{
	allsky340 device (argc, argv);
	return device.run ();
}
