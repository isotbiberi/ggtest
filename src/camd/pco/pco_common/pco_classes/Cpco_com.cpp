//-----------------------------------------------------------------//
// Name        | Cpco_com.cpp                | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS , LINUX                                   //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - Communication                        //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.05 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       | In this file are all functions and definitions,   //
//             | for commiunication with CameraLink grabbers       //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2015 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// This program is free software; you can redistribute it and/or   //
// modify it under the terms of the GNU General Public License as  //
// published by the Free Software Foundation; either version 2 of  //
// the License, or (at your option) any later version.             //
//                                                                 //
// This program is distributed in the hope that it will be useful, //
// but WITHOUT ANY WARRANTY; without even the implied warranty of  //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the    //
// GNU General Public License for more details.                    //
//                                                                 //
// You should have received a copy of the GNU General Public       //
// License along with this program; if not, write to the           //
// Free Software Foundation, Inc., 59 Temple Place- Suite 330,     //
// Boston, MA 02111-1307, USA                                      //
//-----------------------------------------------------------------//


//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.01     | 08.10.2010 | ported from Windows SDK                //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.02     | 11.09.2012 | from ME4 class                         //
//           |            | telegram functions in file             //
//           |            | Cpco_cl_func.cpp                       //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.03     | 28.09.2012 | split common and grabber specific      //
//           |            | functions                              //
//           |            |                                        //
//-----------------------------------------------------------------//
//  1.04     | 25.02.2014 | changes for usb cameras                //
//           |            | rename and make cameralink functions   //
//           |            | virtual                                //
//-----------------------------------------------------------------//
//  1.05     | 20.06.2015 | bugfixes                               //
//           |            | comments in header for doxygen         //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.2009 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//


#include "Cpco_com.h"


////////////////////////////////////////////////////////////////////////////////////////////
//
//  LOCAL FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////

/*
#if !defined _byteswap_ulong
inline unsigned long _byteswap_ulong(unsigned long val)
{
 int x;
 unsigned char b[4];
 unsigned long a;

 for(x=0;x<4;x++)
  b[3-x]=*((unsigned char*)&val+x);

 memcpy(&a,b,sizeof(b));
 return a;
// return (unsigned long)(*(unsigned long*)b);
}
#endif
*/

CPco_com::CPco_com()
{
    clog=NULL;
    hdriver = (PCO_HANDLE) NULL;

    initmode=0;
    boardnr=-1;
    internal_open=0;
    connected=0;
}


/*
CPco_com::~CPco_com()
{
    Close_Cam();

    dwInitmode=0;
    dwBoardnr=-1;
    bInternal_open=0;
}
*/


void CPco_com::SetLog(CPco_Log *Log)
{
    if(Log)
     clog=Log;
}



void CPco_com::writelog(DWORD level,PCO_HANDLE hdriver,const char *message,...)
{
    if(clog)
    {
        va_list arg;
        va_start(arg,message);
        clog->writelog(level,hdriver,message,arg);
        va_end(arg);
    }
}

DWORD CPco_com::build_checksum(unsigned char *buf,int *size)
{
    unsigned char cks;
    unsigned short *b;
    int x,packet_size;

    b=(unsigned short *)buf;//size of packet is second WORD
    b++;
    packet_size=*b-1;
    if(packet_size>*size)
        return PCO_ERROR_DRIVER_CHECKSUMERROR | PCO_ERROR_DRIVER_CAMERALINK;
    cks=0;
    for(x=0;x<packet_size;x++)
        cks+=*(buf+x);

    *(buf+x)=cks;
    *size=x+1;
    return PCO_NOERROR;
}

DWORD CPco_com::test_checksum(unsigned char *buf,int *size)
{
    unsigned char cks;
    unsigned short *b;
    int x,packet_size;

    cks=0;
    b=(unsigned short *)buf; //size of packet is second WORD
    b++;
    packet_size=(int)*b;
    packet_size--;
    if(packet_size>*size)
    {
        writelog(ERROR_M,(PCO_HANDLE)1,"test_checksum size error");
        return PCO_ERROR_DRIVER_CHECKSUMERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    //  writelog(ERROR_M,(HANDLE)1,"test_checksum bsize %d *b 0x%x",bsize,*b);

    for(x=0;x<packet_size;x++)
        cks+=*(buf+x);

    if(*(buf+x)!=cks)
    {
        writelog(ERROR_M,(PCO_HANDLE)1,"test_checksum size error");
        return PCO_ERROR_DRIVER_CHECKSUMERROR | PCO_ERROR_DRIVER_CAMERALINK;
    }

    *size=x+1;
    //  writelog(ERROR_M,(HANDLE)1,"test_checksum *size %d x %d",*size,x);

    return PCO_NOERROR;
}

DWORD CPco_com::get_description()
{
    SC2_Simple_Telegram com;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CAMERA_DESCRIPTION;
    com.wSize=sizeof(SC2_Simple_Telegram);
    err=Control_Command(&com,sizeof(com),
                        &description,sizeof(SC2_Camera_Description_Response));

    if(err!=PCO_NOERROR)
        writelog(ERROR_M,hdriver,"get_description: GET_CAMERA_DESCRIPTION failed with 0x%x",err);

    return err;
}


DWORD CPco_com::get_firmwarerev()
{
    SC2_Simple_Telegram com;
    SC2_Firmware_Versions_Response resp;
    DWORD err;


    tab_timeout.command=400;
    com.wCode=GET_FIRMWARE_VERSIONS;
    com.wSize=0x0005;

    err=Control_Command(&com,sizeof(SC2_Simple_Telegram),
                        &resp,sizeof(SC2_Firmware_Versions_Response));

    if(err==PCO_NOERROR)
    {
        int x,y;
        y=0;
        for(x=0;x<resp.DeviceNum;x++)
        {
            if(!strncmp(resp.Device[x].szName,"Main uP",7))
            {
                writelog(INFO_M,hdriver,"camera mainproz= 0x%02x%02x",resp.Device[x].bMajorRev,resp.Device[x].bMinorRev);
                camerarev|= ((resp.Device[x].bMajorRev<<8)|resp.Device[x].bMinorRev);
                y|=0x01;
            }
            if(!strncmp(resp.Device[x].szName,"Main FPGA",7))
            {
                writelog(INFO_M,hdriver,"camera mainfpga= 0x%02x%02x",resp.Device[x].bMajorRev,resp.Device[x].bMinorRev);
                camerarev|=(((resp.Device[x].bMajorRev<<8)|resp.Device[x].bMinorRev)<<16);
                y|=0x02;
            }
            if(y==0x03)
                break;
        }
    }
    else
        writelog(ERROR_M,hdriver,"get_firmwarerev: err 0x%x",err);

    tab_timeout.command=PCO_SC2_COMMAND_TIMEOUT;
    return err;
}



DWORD CPco_com::get_lut_info()
{
    SC2_Get_Lookuptable_Info_Response resp;
    SC2_Simple_Telegram com;
    DWORD err=PCO_NOERROR;
    int x;

    num_lut=0;
    for(x=0;x<10;x++)
        memset(&cam_lut[x],0,sizeof(SC2_LUT_DESC));

    com.wCode=GET_LOOKUPTABLE_INFO;
    com.wSize=sizeof(SC2_Simple_Telegram);
    err=Control_Command(&com,sizeof(com),
                        &resp,sizeof(SC2_Get_Lookuptable_Info_Response));

    if(err!=PCO_NOERROR)
    {
        writelog(ERROR_M,hdriver,"get_lut_info: GET_LOOKUPTABLE_INFO failed with 0x%x",err);
        writelog(ERROR_M,hdriver,"get_lut_info: GET_LOOKUPTABLE_INFO failed with 0x%x",err&~PCO_ERROR_DEVICE_MASK);
        if((err&~PCO_ERROR_DEVICE_MASK)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)
        {
            err=PCO_NOERROR;
            writelog(ERROR_M,hdriver,"get_lut_info: PCO_ERROR_FIRMWARE_NOT_SUPPORTED return 0x%x",err);
        }
    }
    else
    {
        num_lut=resp.wLutNumber;
        writelog(INIT_M,hdriver,"get_lut_info: camera has %d luts installed",num_lut);

        for(x=0;x<num_lut;x++)
            memcpy(&cam_lut[x],&resp.LutDesc[x],sizeof(SC2_LUT_DESC));
    }
    return err;
}


void CPco_com::gettimeouts(PCO_SC2_TIMEOUTS *strtimeouts)
{
    memcpy(strtimeouts,&tab_timeout,sizeof(PCO_SC2_TIMEOUTS));
}


void CPco_com::Set_Timeouts(void *timetable,DWORD length)
{
    PCO_SC2_TIMEOUTS* tab;
    tab=(PCO_SC2_TIMEOUTS*)timetable;
    if(length>=4)
        tab_timeout.command=tab->command;
    if(length>=8)
        tab_timeout.image=tab->image;
    if(length>=12)
        tab_timeout.transfer=tab->transfer;

}


int CPco_com::GetConnectionStatus()
{
    return connected;
}

void CPco_com::SetConnectionStatus(DWORD status)
{
    connected = status;
}



void CPco_com::Sleep_ms(DWORD time_ms) //time in ms
{
#ifdef __linux__
    int ret_val;
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(0,&rfds);
    tv.tv_sec=time_ms/1000;
    tv.tv_usec=(time_ms%1000)*1000;
    ret_val=select(1,NULL,NULL,NULL,&tv);
    if(ret_val<0)
        writelog(ERROR_M,hdriver,"Sleep: error in select");
#endif
#ifdef WIN32
    Sleep(time_ms);
#endif



}

