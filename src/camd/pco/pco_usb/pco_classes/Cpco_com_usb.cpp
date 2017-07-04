//-----------------------------------------------------------------//
// Name        | Cpco_cam_usb.cpp            | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS 2000/XP, Linux                            //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - libusb Communication                 //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.06                                         //
//-----------------------------------------------------------------//
// Notes       | In this file are all functions and definitions,   //
//             | for communication with USB grabbers               //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
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
//  1.02     | 14.01.2011 | new functions:                         //
//           |            | PCO_GetTemperature                     //
//           |            | PCO_SetLut                             //
//           |            | PCO_ResetSettingsToDefault             //
//           |            | PCO_SetTriggerMode                     //
//           |            | PCO_ForceTrigger                       //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.03     | 24.01.2012 | new functions:                         //
//           |            | PCO_GetCameraType                      //
//           |            | without log class                      //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.04     | 10.05.2012 | new functions:                         //
//           |            | PCO_GetSensorSignalStatus              //
//           |            | PCO_GetROI                             //
//           |            | PCO_SetROI                             //
//           |            | PCO_GetBinning                         //
//           |            | PCO_SetBinning                         //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.05     | 11.09.2012 | telegram functions in own file         //
//           | 16.10.2012 | split common and grabber specific      //
//           |            | functions                              //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.06     | 11.09.2012 | ported for usb cameras                 //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  0.0x     | xx.xx.2009 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//

#include "Cpco_com_usb.h"

CPco_com_usb::CPco_com_usb() : CPco_com()
{
    //  clog=NULL;
    //  hdriver=NULL;

    libusb_ctx=NULL;
    libusb_hdev=NULL;
    libusb_ConfigDescr=NULL;
    alt_set=0;
    transfer_done=0;

    ComInAddr=0;
    ComOutAddr=0;
    ComInMaxPacketSize=0;
    ComOutMaxPacketSize=0;

//usb_param not used at the moment
    usb_param.MaxNumUsb=0;         // defines packet size 
    usb_param.ClockFrequency=0;      // Pixelclock in Hz: 40000000,66000000,80000000
    usb_param.Transmit=0;            // single or continuous transmitting images, 0-single, 1-continuous
    usb_param.UsbConfig=0;           // 0=bulk_image, 1=iso_image
    usb_param.ImgTransMode=0;        // Bit0: 14Bit Image


}

CPco_com_usb::~CPco_com_usb()
{
    writelog(INIT_M,hdriver,"~CPco_com_usb: before Close_Cam()");
    Close_Cam();
    if(libusb_ctx)
        libusb_exit(libusb_ctx);
}

DWORD CPco_com_usb::scan_camera()
{
    SC2_Simple_Telegram com;
    SC2_Camera_Type_Response resp;
    DWORD err=PCO_NOERROR;

    for(int i=0;i<5;i++)
    {
        usb_clear_input();

        writelog(INIT_M,hdriver,"scan_camera: scan with GET_CAMERA_TYPE %d",i);
        com.wCode=GET_CAMERA_TYPE;
        com.wSize=sizeof(SC2_Simple_Telegram);

        err=Control_Command(&com,sizeof(com),&resp,sizeof(SC2_Camera_Type_Response));
        if(err==PCO_NOERROR)
        {
            writelog(INIT_M,hdriver,"scan_camera: successful");
            break;
        }
    }
    return err;
}

DWORD CPco_com_usb::usb_clear_input()
{
    DWORD err = PCO_NOERROR;
    int Len;
    int totallength = 0;
    unsigned char *buf;
    buf=(unsigned char*)malloc(ComInMaxPacketSize);
    Len = ComInMaxPacketSize;
    while(Len!=0) { //while there are packets to read
        err=libusb_bulk_transfer(libusb_hdev, ComInAddr, buf, Len, &Len,25);
        totallength+=Len;
    }
    if(err!=0)
    {
        writelog(PROCESS_M,hdriver,"usb_clear_input: nothing to clear");
    }
    else {
        writelog(COMMAND_M,hdriver,"usb_clear_input: transfer done *buf 0x%04X  bytes read: %d",*((unsigned short*)buf),totallength);
    }

    free(buf);
    return err;
}

// Read Data from USB. This is used by the Control_Command function


DWORD CPco_com_usb::usb_read(void *buf,int *buflen,DWORD timeout)
{
    DWORD err = PCO_NOERROR;
    int Len;
    int done=0;

    if((int)*buflen<=ComInMaxPacketSize)
    {
        Len = *buflen; //ComInMaxPacketSize;

        err=libusb_bulk_transfer(libusb_hdev, ComInAddr, (unsigned char*)buf, Len, &Len, timeout);
        if(err!=0)
        {
            writelog(ERROR_M,hdriver,"usb_read: libusb_bulk_transfer error: %d %s", err,libusb_error_name(err));
            err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
        }
        writelog(INTERNAL_1_M,hdriver,"usb_read: transfer done *buf 0x%04X  bytes read: %d",*((unsigned short*)buf),Len);
        done=Len;
    }
    else
    {
        unsigned char *bufin;
        bufin=(unsigned char*)buf;
        Len = ComInMaxPacketSize;

        while(done<*buflen)
        {
            err=libusb_bulk_transfer(libusb_hdev, ComInAddr, bufin, Len, &Len, timeout);
            if(err!=0)
            {
                writelog(ERROR_M,hdriver,"usb_read: libusb_bulk_transfer failed (%d) %s", err,libusb_error_name(err));
                err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
                break;
            }

            writelog(INTERNAL_1_M,hdriver,"usb_read: transfer done *buf 0x%04X  bytes read: %d",*((unsigned short*)bufin),Len);
            bufin+=Len;
            done+=Len;
            if((done+ComInMaxPacketSize)>*buflen)
                Len=*buflen-done;
            else
                Len=ComInMaxPacketSize;
            writelog(INTERNAL_1_M,hdriver,"usb_read: done %d next Len %d",done,Len);
        }
    }

    if(err==PCO_NOERROR)
        *buflen=done;

    return err;
}

// Write data to USB. Used by Control_Command funtion

DWORD CPco_com_usb::usb_write(void *buf,int *buflen,DWORD timeout)
{
    DWORD err = PCO_NOERROR;
    int Len;

    if(*buflen<=ComOutMaxPacketSize)
    {
        Len = *buflen;

        err=libusb_bulk_transfer(libusb_hdev,ComOutAddr,(unsigned char*)buf,Len,&Len, timeout);
        if(err)
        {
            writelog(ERROR_M,(PCO_HANDLE)1,"write_usb: libusb_bulk_transfer failed (%d)  %s",err,libusb_error_name(err));
            err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
        }
        else
        {
            err = PCO_NOERROR;
            writelog(INTERNAL_1_M,hdriver,"usb_write: transfer done *buf 0x%04X  bytes written: %d",*((unsigned short*)buf),Len);
        }
    }
    else
    {
        int done=*buflen;
        unsigned char *bufout;
        bufout=(unsigned char*)buf;
        Len = ComOutMaxPacketSize;

        while(done>0)
        {
            err=libusb_bulk_transfer(libusb_hdev, ComOutAddr, bufout, Len, &Len, timeout);
            if(err!=0)
            {
                writelog(ERROR_M,hdriver,"usb_write: libusb_bulk_transfer failed (%d) %s", err, libusb_error_name(err));
                err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
                break;
            }
            else
            {
                err = PCO_NOERROR;
                writelog(INTERNAL_1_M,hdriver,"usb_write: transfer done *buf 0x%04X  bytes written: %d",*((unsigned short*)bufout),Len);
                bufout+=Len;
                done-=Len;
                if((done-Len)<0)
                    Len=done;
                writelog(INTERNAL_1_M,hdriver,"usb_write: next done %d Len %d",done,Len);
            }
        }
    }
    return err;
}

/////////////////////////////////////////////////////////////////
//
// Looks for USB devices and gets their endpoints
//
////////////////////////////////////////////////////////////////

DWORD CPco_com_usb::usb_get_endpoints()
{
    int eptCount;
    int ep_contr_in,ep_contr_out;

    struct libusb_device_descriptor desc;
    libusb_device *dev;

    dev=libusb_get_device(libusb_hdev);
    libusb_get_device_descriptor(dev,&desc);
    eptCount = libusb_ConfigDescr->interface[0].altsetting[alt_set].bNumEndpoints;
    if((eptCount==0)&&(libusb_ConfigDescr->interface[0].num_altsetting>1))
    {
        alt_set=1;
        eptCount = libusb_ConfigDescr->interface[0].altsetting[alt_set].bNumEndpoints;
    }

    ep_contr_in=ep_contr_out=0;

    switch(desc.idProduct)
    {
    case USB_PID_IF_GIGEUSB_20:     // 0x0001  FX2 (Cypress 68013a)
    case USB_PID_CAM_PIXFLY_20:     // 0x0002  FX2 (Cypress 68013a)
        ep_contr_in = USB_EP_FX2_CTRL_IN;
        ep_contr_out= USB_EP_FX2_CTRL_OUT;
        iftype=INTERFACE_USB;
        break;

    case USB_PID_IF_GIGEUSB_30:     // 0x0003  FX3 (Cypress CYUSB3014-BZX) Application Code
    case USB_PID_IF_GIGEUSB_30_B1:  // 0x0004  FX3 (Cypress CYUSB3014-BZX) SPI Boot Code (FPGA Update)
    case USB_PID_IF_GIGEUSB_30_B2:  // 0x0005  FX3 (Cypress CYUSB3014-BZX) I2C Boot Code (FX3 Update)
    case USB_PID_CAM_EDGEUSB_30:    // 0x0006  Fx3 (Cypress CYUSB3014-BZX)
        ep_contr_in = USB_EP_FX3_CTRL_IN;
        ep_contr_out= USB_EP_FX3_CTRL_OUT;
        iftype=INTERFACE_USB3;
        break;


    case USB_PID_CAM_FLOW_20:       // 0x0007  AVR32
    case USB_PID_CAM_EDGEHS_20:     // 0x0008  AVR32
        ep_contr_in = USB_EP_AVR32_CTRL_IN;
        ep_contr_out= USB_EP_AVR32_CTRL_OUT;
        iftype=INTERFACE_USB;
        break;

    default:
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    for(int  i=0; i<eptCount;  i++)
    {
        if(libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].bmAttributes == LIBUSB_TRANSFER_TYPE_BULK)
        {
            if(libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].bEndpointAddress==ep_contr_in)
            {
                ComInAddr = libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].bEndpointAddress;
                writelog(INIT_M,hdriver,"get_endpoints: BulkControlIn found (EP 0x%02x) %d", ComInAddr, i);
                ComInMaxPacketSize=libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].wMaxPacketSize;
                writelog(INIT_M,hdriver, "get_endpoints: ComInMaxPacketSize %d",ComInMaxPacketSize);
            }

            if(libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].bEndpointAddress==ep_contr_out)
            {
                ComOutAddr = libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].bEndpointAddress;
                writelog(INIT_M,hdriver, "get_endpoints: BulkControlOut found (EP 0x%02x) %d", ComOutAddr, i);
                ComOutMaxPacketSize=libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].wMaxPacketSize;
                writelog(INIT_M,hdriver, "get_endpoints: ComOutMaxPacketSize %d",ComOutMaxPacketSize);
            }
        }
    }

    if((ComInAddr==0)||(ComOutAddr==0))
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;

    return PCO_NOERROR;
}



////////////////////////////////////////////////////////////////////////////////////////////
//
// OPEN/CLOSE FUNCTIONS
//
//
////////////////////////////////////////////////////////////////////////////////////////////


DWORD CPco_com_usb::Open_Cam(DWORD num)
{

    return Open_Cam_Ext(num,NULL);
}

DWORD CPco_com_usb::Open_Cam_Ext(DWORD num,SC2_OpenStruct *open ATTRIBUTE_UNUSED)
{
    int err;


    if (sem_init(&sMutex, 0, 1) == -1)
        writelog(PROCESS_M, (PCO_HANDLE)1, "Could not get semaphore sMutex.");


    initmode=num & ~0xFF;
    num=num&0xFF;

    if(num>MAXNUM_DEVICES)
    {
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: No more entries left return NODRIVER");
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    if(hdriver)
    {
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: camera is already connected");
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    err = libusb_init(&libusb_ctx);
    if(err<0)
    {
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: libusb_init failed (%d) %s",err,libusb_error_name(err));
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    writelog(INIT_M,hdriver,"Open_Cam_Ext: libusb_init done libusb_ctx:%p",libusb_ctx);


    const struct libusb_version *libvers=libusb_get_version ();
    writelog(INIT_M,hdriver,"Open_Cam_Ext: libusb version: %02d.%02d.%02d",libvers->major,libvers->minor,libvers->micro);



    //search for devices avaiable
    int cnt,pco_cnt;
    libusb_device **devs;
    cnt = (int)libusb_get_device_list(libusb_ctx,&devs);
    if(cnt<0)
    {
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: libusb_get_device_list failed (%d) %s",err,libusb_error_name(err));
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    libusb_device *dev=NULL;
    libusb_device *pco_dev[MAXNUM_DEVICES];
    struct libusb_device_descriptor desc;
    for(int i=0;i<MAXNUM_DEVICES;i++)
        pco_dev[i]=NULL;

    pco_cnt=0;
    for(int i=0;i<cnt;i++)
    {
        err=libusb_get_device_descriptor(devs[i], &desc);
        if(err<0)
            writelog(ERROR_M,hdriver,"Open_Cam_Ext: libusb_get_device_descriptor dev[%d] failed (%d) %s",i,err,libusb_error_name(err));
        else
        {
            writelog(INTERNAL_3_M,(PCO_HANDLE)1,"Open_Cam_Ext: found device 0x%x",desc.idVendor);
            if(desc.idVendor==USB_VID)
            {
                writelog(INIT_M,hdriver,"Open_Cam_Ext: PCO USB device found 0x%x",desc.idProduct);
                pco_dev[pco_cnt]=devs[i];
                pco_cnt++;
                /*
        uint8_t path[8];
        err = libusb_get_port_numbers(devs[i], path, sizeof(path));
        if(err<0)
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: libusb_get_port_numbers failed (%d) %s",err,libusb_error_name(err));
        else
        {
        writelog(INIT_M,hdriver,"Open_Cam_Ext: libusb_get_port_numbers %d",path[0]);
        if(path[0]>=3)
        {
        pco_cnt++;
        if(num==pco_cnt-1)
        dev=devs[i];
        }
        }
        */
            }
        }
    }

    writelog(INIT_M,hdriver,"Open_Cam_Ext: %d valid usb devices found",pco_cnt);

    //  if(((int)num>pco_cnt-1)||(dev==NULL))
    if(pco_dev[num]==NULL)
    {
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: requested usb device 0x%04x number %d not found",USB_VID,num);
        libusb_free_device_list(devs, 1);
        libusb_exit(libusb_ctx);
        libusb_ctx=NULL;
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    dev=pco_dev[num];


    err=libusb_open(dev,&libusb_hdev);

    libusb_free_device_list(devs, 1);
    if(err<0)
    {
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: libusb_open failed (%d) %s",err,libusb_error_name(err));
        libusb_exit(libusb_ctx);
        libusb_ctx=NULL;
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    err=libusb_get_device_descriptor(dev,&desc);
    if((desc.idProduct==USB_PID_IF_GIGEUSB_20)||(desc.idProduct==USB_PID_CAM_PIXFLY_20)||(desc.idProduct==USB_PID_CAM_EDGEUSB_30))
    {
        err=libusb_reset_device(libusb_hdev);
        if(err<0)
        {
            writelog(ERROR_M,hdriver,"Open_Cam_Ext: libusb_reset_device failed (%d) %s",err,libusb_error_name(err));
            libusb_close(libusb_hdev);
            libusb_exit(libusb_ctx);
            libusb_ctx=NULL;
            return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
        }
        writelog(INIT_M,hdriver,"Open_Cam_Ext: GIGEUSB_20 or CAM_PIXFLY_20 or USB_PID_CAM_EDGEUSB_30 libusb_reset_device done");
    }

    char speed[20];
    switch(libusb_get_device_speed(dev)) {
    case LIBUSB_SPEED_HIGH:
        sprintf(speed,"HIGH_SPEED");
        break;
    case LIBUSB_SPEED_FULL:
        sprintf(speed,"FULL_SPEED");
        break;
    case LIBUSB_SPEED_SUPER:
        sprintf(speed,"SUPER_SPEED");
        break;
    case LIBUSB_SPEED_LOW:
        sprintf(speed,"LOW_SPEED");
        break;
    default:
        sprintf(speed,"UNKNOWN_SPEED");
        break;
    }

    writelog(PROCESS_M,hdriver,"Open_Cam_Ext: device has %s capability",speed);


    err=libusb_get_config_descriptor(dev, 0, &libusb_ConfigDescr);
    if (err<0)
    {
        writelog(ERROR_M,hdriver,"Get config descriptor failed (%d) %s",err,libusb_error_name(err));
        libusb_close(libusb_hdev);
        libusb_hdev = NULL;
        libusb_exit(libusb_ctx);
        libusb_ctx=NULL;
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    writelog(INIT_M,hdriver,"Open_Cam_Ext: libusb_open done libusb_hdev: %p libusb_ConfigDescr: %p",libusb_hdev,libusb_ConfigDescr);


    libusb_set_auto_detach_kernel_driver(libusb_hdev,1);


    err = libusb_claim_interface(libusb_hdev, 0);
    if(err<0)
    {
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: libusb_claim_interface failed (%d) %s",err,libusb_error_name(err));
        libusb_free_config_descriptor(libusb_ConfigDescr);
        libusb_ConfigDescr = NULL;
        libusb_close(libusb_hdev);
        libusb_hdev = NULL;
        libusb_exit(libusb_ctx);
        libusb_ctx=NULL;
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    writelog(INIT_M,hdriver,"Open_Cam_Ext: libusb_claim_interface done");

    if((desc.idProduct==USB_PID_CAM_FLOW_20)||(desc.idProduct==USB_PID_CAM_EDGEHS_20))
    {
        writelog(INIT_M,hdriver,"Open_Cam_Ext: FLOW_20 or EDGEHS_20 use alt_set 1");
        alt_set=1;
    }

    err = libusb_set_interface_alt_setting(libusb_hdev, 0,alt_set);
    if(err<0)
    {
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: libusb_set_interface_alt_setting failed (%d) %s",err,libusb_error_name(err));
        libusb_free_config_descriptor(libusb_ConfigDescr);
        libusb_ConfigDescr = NULL;
        libusb_close(libusb_hdev);
        libusb_hdev = NULL;
        libusb_exit(libusb_ctx);
        libusb_ctx=NULL;
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    err=usb_get_endpoints();
    if(err!=PCO_NOERROR)
    {
        writelog(ERROR_M,hdriver,"Get endpoints failed (%d) %s",err,libusb_error_name(err));
        libusb_free_config_descriptor(libusb_ConfigDescr);
        libusb_ConfigDescr = NULL;
        libusb_close(libusb_hdev);
        libusb_hdev = NULL;
        libusb_exit(libusb_ctx);
        libusb_ctx=NULL;
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }



    hdriver=(PCO_HANDLE)(0x100+num);
    camerarev=0;

    tab_timeout.command=PCO_SC2_COMMAND_TIMEOUT;
    tab_timeout.image=PCO_SC2_IMAGE_TIMEOUT_L;
    tab_timeout.transfer=PCO_SC2_COMMAND_TIMEOUT;

    boardnr=num;

    //check if camera is connected, error should be timeout
    //get camera descriptor to get maximal size of ccd
    //scan for other baudrates than default 9600baud
    err=PCO_NOERROR;
    err=scan_camera();
    if(err!=PCO_NOERROR)
    {
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: Control command failed with 0x%x, no camera connected",err);

        Close_Cam();
        boardnr=-1;
        hdriver=(PCO_HANDLE)NULL;
        return PCO_ERROR_DRIVER_NOTINIT | PCO_ERROR_DRIVER_USB;
    }

#ifndef BASE_FUNC_ONLY
    err=get_description();
    if(err!=PCO_NOERROR)
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: get_desription() failed with 0x%x",err);

    if(err==PCO_NOERROR)
    {
        err=get_firmwarerev();
        if(err!=PCO_NOERROR)
            writelog(ERROR_M,hdriver,"Open_Cam_Ext: get_firmwarerev() failed with 0x%x",err);
    }

    struct tm st;

#ifdef WIN32
    time_t curtime;
    curtime=time(NULL);
    localtime_s(&st,&curtime);
#else
    int timeoff;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    timeoff=(1000000-tv.tv_usec)/1000;
    Sleep_ms(timeoff);
    gettimeofday(&tv,NULL);

    for(int i=0;i<10;i++)
    {
        gettimeofday(&tv,NULL);
        if(tv.tv_usec<100)
            break;
        usleep(20);
    }
    localtime_r(&tv.tv_sec,&st);
    writelog(INIT_M,hdriver,"Open_Cam_Ext: wait done set camera time to %02d.%02d.%04d %02d:%02d:%02d.%06d",
             st.tm_mday,st.tm_mon+1,st.tm_year+1900,st.tm_hour,st.tm_min,st.tm_sec,tv.tv_usec);
#endif

    err=PCO_SetDateTime(&st);
    if(err!=PCO_NOERROR)
        writelog(ERROR_M,hdriver,"Open_Cam_Ext: PCO_SetDateTime() failed with 0x%x",err);
#endif


    //  connected|=(1<<boardnr);

//set interface output format to coding
    WORD wFormat = 4;
    err=PCO_SetInterfaceOutputFormat(SET_INTERFACE_USB,wFormat);
    if(err==PCO_NOERROR)
    {
        err=PCO_GetInterfaceOutputFormat(SET_INTERFACE_USB,&wFormat);
        if(wFormat != 4)
        {
            err=PCO_SetInterfaceOutputFormat(SET_INTERFACE_USB,0);
            writelog(INIT_M,hdriver,"Open_Cam_Ext: Old FW without coding!");
            gl_Coding = false;
        }
        else
        {
            gl_Coding = true;
            writelog(INIT_M,hdriver,"Open_Cam_Ext: New FW with coding! Format: %x", wFormat);

        }
    }
    else
    {
        writelog(INIT_M,hdriver,"Open_Cam_Ext: camera without coding");
        gl_Coding = false;
    }

    EnableHandshake();

    return PCO_NOERROR;
}

// gl_Coding is set by the Open_Camera function.
bool CPco_com_usb::getCoding()
{
    return gl_Coding;
}

int CPco_com_usb::IsOpen()
{
    if(hdriver!=(PCO_HANDLE)NULL)
        return true;
    else
        return false;
}


DWORD CPco_com_usb::Close_Cam()
{
    int err;
    if(hdriver==(PCO_HANDLE)NULL)
    {
        writelog(INIT_M,hdriver,"Close_Cam: driver was closed before");
        return PCO_NOERROR;
    }

    //delete semaphor
    sem_close(&sMutex);
    sem_destroy(&sMutex);
    writelog(INIT_M, hdriver, "Close_Cam: Close Mutex 0x%x", sMutex);

    libusb_reset_device(libusb_hdev);

    if(libusb_hdev)
    {
        writelog(INIT_M,hdriver,"Close_Cam: libusb_release_interface 0x%x",libusb_hdev);
        err = libusb_release_interface(libusb_hdev, 0);
        if(err<0)
        {
            writelog(ERROR_M,hdriver,"Release usb interface failed (%d)", err);
        }

        if(libusb_ConfigDescr)
        {
            libusb_free_config_descriptor(libusb_ConfigDescr);
            libusb_ConfigDescr = NULL;
        }

        writelog(INIT_M,hdriver,"Close_Cam: libusb_close libusb_hdev %p",libusb_hdev);
        libusb_close(libusb_hdev);

        libusb_hdev = NULL;
    }

    if(libusb_ctx)
    {
        writelog(INIT_M,hdriver,"Close_Cam: libusb_exit libusb_ctx: %p",libusb_ctx);
        libusb_exit(libusb_ctx);
        libusb_ctx=NULL;
    }

    boardnr=-1;
    hdriver=(PCO_HANDLE)NULL;
    //  connected&=(1<<boardnr);

    return PCO_NOERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// CAMERA IO FUNCTION
//
//
////////////////////////////////////////////////////////////////////////////////////////////
DWORD CPco_com_usb::Control_Command(void *buf_in,DWORD size_in,
                                    void *buf_out,DWORD size_out)
{
    DWORD err=PCO_NOERROR;
    unsigned char buffer[PCO_SC2_DEF_BLOCK_SIZE];
    unsigned char *buf;
    int size;
    WORD com_in,com_out;

    //wait for semaphore
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    ts.tv_sec += 1;
    //sem_timedwait(&sMutex, &ts);
    int s;
    while ((s = sem_timedwait(&sMutex, &ts)) == -1 && errno == EINTR)
        continue;
    if (s == -1) {
        if (errno == ETIMEDOUT)
            writelog(COMMAND_M,hdriver,"mutex timed out");
        else
            writelog(COMMAND_M,hdriver,"some other error");
    }

    com_in=*((WORD*)buf_in);
    com_out=0x0000;

    {
        char buffer[50] = "\0";
        ComToString(com_in,buffer);
        writelog(COMMAND_M,hdriver,"Control_Command: start com_in  %s timeout %d",buffer,tab_timeout.command);
    }


    /*
  if(size_out > sizeof(SC2_Failure_Response))
  size=size_out;
  else
  size=sizeof(SC2_Failure_Response)
  max(size_out,sizeof(SC2_Failure_Response));
  */

    size=PCO_SC2_DEF_BLOCK_SIZE;
    memset(buffer,0,PCO_SC2_DEF_BLOCK_SIZE);

    size=size_in;
    err=build_checksum((unsigned char*)buf_in,(int*)&size);

    err=usb_write(buf_in,&size,tab_timeout.command);
    if(err!=PCO_NOERROR) {
        usbcom_out(com_out,size_out,err);
        return err;
    }

//@@@mbl for reboot command
    if((buf_out==NULL)&&(size_out==0))
    {
        usb_clear_input();
        return PCO_NOERROR;
    }
    



    while(com_out==0x0000) {
        size=PCO_SC2_DEF_BLOCK_SIZE; //sizeof(WORD)*2;
        buf=buffer;
        err=usb_read(buf,&size,tab_timeout.command*2);
        if(err!=PCO_NOERROR) {
            usbcom_out(com_out,size_out,err);
            return err;
        }

        com_out=*((WORD*)buf);

        if((com_out&0xFF3F)==IMAGE_TRANSFER_DONE_MSG)
        {
            int siz;
            SC2_Image_Transfer_Done_Message resp;
            SC2_Simple_Telegram resp1;

            siz=size;
            err = test_checksum(buf,(int*)&siz);

            if(siz==sizeof(resp))
            {
                memcpy(&resp,buf,sizeof(resp));
                transfer_done=resp.dwImageDataCount;
                writelog(PROCESS_M,hdriver,"Control_Command: end   com_out 0x%04x IMAGE_TRANSFER_DONE size_out siz %d",com_out,siz);
            }
            else if(siz==sizeof(resp1))
            {
                memcpy(&resp1,buf,sizeof(resp1));
                transfer_done=1;
                writelog(PROCESS_M,hdriver,"Control_Command: end   com_out 0x%04x IMAGE_TRANSFER_DONE size_out siz %d",com_out,siz);
            }
            else if((com_out|RESPONSE_OK_CODE)==FATAL_ERROR_MSG)
            {
                writelog(ERROR_M,hdriver,"Control_Command: comin 0x%04x FATAL_ERROR",com_in,siz);
                return err;
            }
            buf+=size;
            com_out=*((WORD*)buf);
        }
    }


    if(com_in!=(com_out&0xFF3F))
    {
        writelog(ERROR_M,hdriver,"Control_Command: comin  0x%04x != comout&0xFF3F 0x%04x",com_in,com_out&0xFF3F);
        err=PCO_ERROR_DRIVER_IOFAILURE | PCO_ERROR_DRIVER_USB;
        usbcom_out(com_out,size_out,err);
        return err;
    }

    if((com_out&RESPONSE_ERROR_CODE)==RESPONSE_ERROR_CODE)
    {
        SC2_Failure_Response resp;
        memcpy(&resp,buf,sizeof(SC2_Failure_Response));
        err=resp.dwerrmess;
        if((err&0xC000FFFF)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)
            writelog(INTERNAL_1_M,hdriver,"Control_Command: com 0x%x FIRMWARE_NOT_SUPPORTED",com_in);
        else
            writelog(ERROR_M,hdriver,"Control_Command: com 0x%x RESPONSE_ERROR_CODE error 0x%x",com_in,err);
    }

    if(err==PCO_NOERROR)
    {
        if(com_out!=(com_in|RESPONSE_OK_CODE))
        {
            err=PCO_ERROR_DRIVER_DATAERROR | PCO_ERROR_DRIVER_USB;
            writelog(ERROR_M,hdriver,"Control_Command: Data error com_out 0x%04x should be 0x%04x",com_out,com_in|RESPONSE_OK_CODE);
        }
    }

    if(size_out > sizeof(SC2_Failure_Response))
        size=size_out;
    else
        size=sizeof(SC2_Failure_Response);

    //  size=max(size_out,sizeof(SC2_Failure_Response));


    writelog(INTERNAL_1_M,hdriver,"Control_Command: before test_checksum read=0x%04x size %d",com_out,size);
    if(test_checksum(buf,&size)==PCO_NOERROR)
    {
        size-=1;
        if(size<(int)size_out)
            size_out=size;
        memcpy(buf_out,buf,size_out);
    }
    else
    {
        err=test_checksum(buf,&size);
    }
    usbcom_out(com_out,size_out,err);
    return err;
}

void CPco_com_usb::usbcom_out(WORD com_out,DWORD size_out,DWORD err)
{

    //release semaphore
    sem_post(&sMutex);
    writelog(COMMAND_M,hdriver,"Control_Command: end   com_out 0x%04x size_out %d err 0x%04X",com_out,size_out,err);
}



//some workarounds to decrease readout-time 
DWORD CPco_com_usb::EnableHandshake()
{
    SC2_Get_CL_Configuration_Response resp;
    SC2_Simple_Telegram com;
    DWORD err;

    com.wCode=GET_CL_CONFIGURATION;
    com.wSize=sizeof(SC2_Simple_Telegram);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
    {
     writelog(INIT_M,hdriver,"EnableHandshake: found F:%dMhz,L:0x%x,D:%d,T:0x%x",
                            resp.dwClockFrequency/1000000,resp.bCCline,resp.bDataFormat,resp.bTransmit);

     if(((resp.bCCline&0x04)==0)||(resp.dwClockFrequency<80000000))
     {
      SC2_Set_CL_Configuration cl_com;

      cl_com.wCode=SET_CL_CONFIGURATION;
      cl_com.wSize=sizeof(SC2_Set_CL_Configuration);
      if(iftype==INTERFACE_USB3)
       cl_com.dwClockFrequency=80000000;
      else
       cl_com.dwClockFrequency=resp.dwClockFrequency; //?entry->usb_param.ClockFrequency
      cl_com.bCCline=(resp.bCCline|0x04);
      cl_com.bDataFormat=resp.bDataFormat&0xFF;
      cl_com.bTransmit=resp.bTransmit;

      err=Control_Command(&cl_com,sizeof(SC2_Set_CL_Configuration),
                         &resp,sizeof(SC2_Get_CL_Configuration_Response));
      if(err==PCO_NOERROR)
      {
       usb_param.ClockFrequency=resp.dwClockFrequency;
       usb_param.Transmit=resp.bTransmit;
       writelog(INIT_M,hdriver,"EnableHandshake: done F:%dMhz,L:0x%x,D:%d,T:0x%x",
                            resp.dwClockFrequency/1000000,resp.bCCline,resp.bDataFormat,resp.bTransmit);
      }
      else
       writelog(INIT_M,hdriver,"EnableHandshake: failed");
     }

    }
    else 
     writelog(INIT_M,hdriver,"EnableHandshake: not supported");


/*
    SC2_Get_CL_Baudrate_Response resp_b;
    com.wCode=GET_CL_BAUDRATE;
    com.wSize=sizeof(SC2_Simple_Telegram);
    err=Control_Command(entry->hDevice,&com,sizeof(com), &resp_b,sizeof(resp_b));
    if(err == PCO_NOERROR)
    {
    writelog(INIT_M,(void*)1,"Open_Cam_Ext: cl_baud %d",resp_b.dwBaudrate);
    }
   }
*/
   return err;
}




int CPco_com_usb::transfer_msgwait() {return transfer_done;}

void CPco_com_usb::set_transfer_msgwait(int wait) { transfer_done = wait; }


