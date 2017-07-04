//-----------------------------------------------------------------//
// Name        | CPco_grab_usb.cpp           | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS 2000/XP, Linux                            //
//-----------------------------------------------------------------//
// Environment | Microsoft Visual C++ 6.0                          //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - libusb Image Grab                    //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       | In this file are all functions and definitions,   //
//             | for grabbing from usb camera                      //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2014 PCO AG * Donaupark 11 *                                //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//

#define NEED_GL_CODING
#include "Cpco_grab_usb.h"
#undef NEED_GL_CODING


CPco_grab_usb::CPco_grab_usb(CPco_com_usb *camera)
{
    clog = NULL;
    hgrabber = (PCO_HANDLE) NULL;

    cam = NULL;
    libusb_ctx = NULL;
    libusb_hdev = NULL;

    ImageInAddr = 0;
    ImageInMaxPacketSize = 0;

    last_buffer_1 = last_buffer_2 = NULL;
    async_transfer_1 = async_transfer_2 = NULL;
    async_transfer_status = 0;
    async_transfer_num = 0;
    async_transfer_size = ASYNC_PACKET_SIZE;
    async_copy_buffer_size = 0;

    bitpix = 0;
    act_width = 0;
    act_height = 0;
    act_line_width = 0;
    act_dmalength = 0;

    DataFormat = 0;
    ImageTimeout=0;
    packet_timeout=0;

    //reset these settings
    camtype=0;
    serialnumber=0;
    cam_pixelrate=0;
    cam_timestampmode=0;
    cam_doublemode=0;
    cam_align=0;
    cam_noisefilter=0;
    cam_colorsensor=0;
    cam_width=cam_height=1000;
    memset(&usbpar,0,sizeof(usbpar));
    memset(&description,0,sizeof(description));

    if (camera != NULL)
        cam = camera;

}


CPco_grab_usb::~CPco_grab_usb()
{
    Close_Grabber();
}


void CPco_grab_usb::SetLog(CPco_Log *elog)
{
    if(elog)
        clog=elog;
}

void CPco_grab_usb::writelog(DWORD lev,PCO_HANDLE hdriver,const char *str,...)
{
    if(clog)
    {
        va_list arg;
        va_start(arg,str);
        clog->writelog(lev,hdriver,str,arg);
        va_end(arg);
    }
}

int CPco_grab_usb::Get_actual_size(unsigned int *width,unsigned int *height,unsigned int *bitpix)
{
    {
        if(width)
            *width=act_width;
        if(height)
            *height=act_height;
        if(bitpix)
            *bitpix=this->bitpix;
    }
    return PCO_NOERROR;
}

int CPco_grab_usb::Get_Async_Packet_Size() {
    return async_transfer_size;
}

void CPco_grab_usb::Set_Async_Packet_Size(int size) {
    if(ImageInMaxPacketSize == 0)
    {
        writelog(ERROR_M,hgrabber,"Could not set async_transfer_size, open grabber first!");
    }
    else
    {
        //Async_transfer_size must be a multiple of ImageInMaxPacketSize, so we round down
        async_transfer_size = (size%ImageInMaxPacketSize)*ImageInMaxPacketSize;
        writelog(PROCESS_M,hgrabber,"Async packet size set to: %d",async_transfer_size);
    }
}

DWORD CPco_grab_usb::Set_Grabber_Size(int width, int height)
{
    return Set_Grabber_Size(width,height,0);

}

DWORD CPco_grab_usb::Set_Grabber_Size(int width,int height, int bitpix)
{
    int dmalength;

    writelog(PROCESS_M,hgrabber,"set_actual_size: start w:%d h:%d",width,height);

    if((bitpix == 0)&&(this->bitpix==0))
    {
        SC2_Camera_Description_Response description;
        cam->PCO_GetCameraDescriptor(&description);
        this->bitpix = description.wDynResDESC;
    }

    dmalength=width*height*((this->bitpix + 7) / 8);

//    if(((DataFormat&PCO_CL_DATAFORMAT_MASK)==PCO_CL_DATAFORMAT_1x16)||((DataFormat&PCO_CL_DATAFORMAT_MASK)==PCO_CL_DATAFORMAT_2x12))
//        dmalength*=2;

    //for async
    if(act_dmalength!=dmalength)
    {
        if((async_transfer_status&(ASYNC_SETUP_1|ASYNC_PENDING_1|ASYNC_SETUP_2|ASYNC_PENDING_2))!=0)
        {
            writelog(ERROR_M,hgrabber,"set_actual_size: async_transfer_status SETUP or PENDING cannot setup for new size");
            return PCO_ERROR_DRIVER_IOFAILURE | PCO_ERROR_DRIVER_USB;
        }
        else
        {
            //reset need last &copy buffer
            async_transfer_status&=~ASYNC_NEED_LAST;
            async_transfer_status&=~ASYNC_USE_COPY_BUFFER;

            //free allocated transfers
            if ((async_transfer_status&ASYNC_ALLOC_1) == ASYNC_ALLOC_1)
            {
                usb_clear_input();
                for (int i = 0; i < async_transfer_num; i++) {
                    writelog(PROCESS_M,hgrabber,"set_grabber_size: free transfer %d",i);
                    libusb_free_transfer(async_transfer_1[i]);

                }
                free(async_transfer_1);
            }
            if(async_transfer_size == 0)
                async_transfer_size=ASYNC_PACKET_SIZE;

            if(async_transfer_size>LARGE_PACKET_SIZE)
                async_transfer_size = LARGE_PACKET_SIZE;

            //async_transfer_size must be a multiple of ImageInMaxPacketSize, so we round down
            if(async_transfer_size%ImageInMaxPacketSize)
                async_transfer_size=(async_transfer_size/ImageInMaxPacketSize)*ImageInMaxPacketSize;

            writelog(PROCESS_M,hgrabber,"set_grabber_size: async packet size %d",async_transfer_size);
            async_transfer_num=dmalength/async_transfer_size;

            if(dmalength%async_transfer_size)
            {
                async_last_size=dmalength-async_transfer_size*async_transfer_num;
                async_transfer_num++;
                async_transfer_status|=ASYNC_NEED_LAST;
                writelog(PROCESS_M,hgrabber,"set_grabber_size: async need last size %d",async_last_size);

                //same check as above for last size, the copy buffer removes excess bytes at the end
                if(async_last_size%ImageInMaxPacketSize)
                {
                    writelog(PROCESS_M,hgrabber,"set_grabber_size: use copy buffer");
                    //effective copy buffer size
                    async_copy_buffer_size = async_last_size-((async_last_size/ImageInMaxPacketSize)*ImageInMaxPacketSize);
                    async_last_size=(async_last_size/ImageInMaxPacketSize)*ImageInMaxPacketSize;
                    async_transfer_status|=ASYNC_USE_COPY_BUFFER;
                    async_transfer_num++;
                }
                writelog(PROCESS_M,hgrabber,"set_grabber_size: async transfer num %d",async_transfer_num);

            }
            //allocate transfer
            async_transfer_1=(libusb_transfer**)malloc(async_transfer_num*sizeof(libusb_transfer*));
            if(async_transfer_1)
            {
                memset(async_transfer_1,0,async_transfer_num*sizeof(libusb_transfer*));
                for(int i=0;i< async_transfer_num;i++)
                    async_transfer_1[i]=libusb_alloc_transfer(0);
                async_transfer_status|=ASYNC_ALLOC_1;
            }
        }
    }

    act_height=height;
    act_width=width;
    act_line_width=width;
    act_dmalength=dmalength;

    writelog(PROCESS_M,hgrabber,"set_actual_size: done w:%d h:%d lw:%d bitpix: %d dmalength:%d",act_width,act_height,act_line_width,this->bitpix,act_dmalength);
    return PCO_NOERROR;
}

DWORD CPco_grab_usb::Open_Grabber(int board)
{
    return Open_Grabber(board,0);
}

void CPco_grab_usb::SetBitAlignment(int align)
{
    act_align = align;
}

DWORD CPco_grab_usb::Open_Grabber(int board,int initmode ATTRIBUTE_UNUSED)
{
    DWORD err=PCO_NOERROR;

    if(hgrabber!=(PCO_HANDLE) NULL)
    {
        writelog(INIT_M,(PCO_HANDLE)1,"Open_Grabber: grabber was opened before");
        return PCO_NOERROR;
    }

    if(!cam->IsOpen())
    {
        writelog(INIT_M,(PCO_HANDLE)1,"Open_Grabber: camera com interface must be opened and initialized");
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    libusb_hdev=cam->get_device_handle();
    if(libusb_hdev==NULL)
    {
        writelog(INIT_M,(PCO_HANDLE)1,"Open_Grabber: usb_get_endpoints failed");
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    err=usb_get_endpoints();
    if(err!=PCO_NOERROR)
    {
        writelog(INIT_M,(PCO_HANDLE)1,"Open_Grabber: usb_get_endpoints failed");
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }
    if(clog)
    {
        clog->start_time_mess();
    }

    gl_Coding = cam->getCoding();
    writelog(PROCESS_M,(PCO_HANDLE)1,"Open_Grabber: GetCoding returned %d",gl_Coding);

    hgrabber=(PCO_HANDLE)0x200+board;

    ImageTimeout=10000;
    packet_timeout=200;

    DataFormat=PCO_CL_DATAFORMAT_1x16;

    return err;
}

DWORD CPco_grab_usb::Close_Grabber()
{
    writelog(INIT_M,hgrabber,"Close_Grabber: ");

    if(hgrabber==(PCO_HANDLE)NULL)
    {
        writelog(INIT_M,hgrabber,"Close_Grabber: driver was closed before");
        return PCO_NOERROR;
    }
    hgrabber=(PCO_HANDLE)NULL;

    if(last_buffer_1)
    {
        free(last_buffer_1);
        last_buffer_1=NULL;
    }
    if(last_buffer_2)
    {
        free(last_buffer_2);
        last_buffer_2=0;
    }
    if((async_transfer_status&ASYNC_ALLOC_1)==ASYNC_ALLOC_1)
    {
        for(int i=0;i< async_transfer_num;i++)
            libusb_free_transfer(async_transfer_1[i]);
        free(async_transfer_1);
        async_transfer_1=NULL;
        writelog(INIT_M,hgrabber,"Close_Grabber: free async_transfer_1 done");
    }
    if((async_transfer_status&ASYNC_ALLOC_2)==ASYNC_ALLOC_2)
    {
        for(int i=0;i< async_transfer_num;i++)
            libusb_free_transfer(*async_transfer_2);
        free(async_transfer_2);
        writelog(INIT_M,hgrabber,"Close_Grabber: free async_transfer_2 done");
    }

    libusb_reset_device(libusb_hdev);

    if(libusb_ctx)
    {
        libusb_close(libusb_hdev);
        libusb_hdev = NULL;
        libusb_exit(libusb_ctx);
        libusb_ctx=NULL;
    }

    ImageInAddr = 0;
    ImageInMaxPacketSize = 0;

    last_buffer_1 = last_buffer_2 = NULL;
    async_transfer_1 = async_transfer_2 = NULL;
    async_transfer_status = 0;
    async_transfer_num = 0;
    async_last_size = 0;

    act_width = 0;
    act_height = 0;
    act_line_width = 0;
    act_dmalength = 0;
    bitpix = 0;
    DataFormat = 0;

    return PCO_NOERROR;
}

BOOL CPco_grab_usb::IsOpen()
{
    if(hgrabber!=(PCO_HANDLE)NULL)
        return true;
    else
        return false;
}

DWORD CPco_grab_usb::Set_DataFormat(DWORD dataformat)
{
    DataFormat=dataformat;
    return PCO_NOERROR;
}

DWORD CPco_grab_usb::Set_Grabber_Timeout(int timeout)
{
    ImageTimeout=timeout;
    return PCO_NOERROR;
}

DWORD CPco_grab_usb::Get_Grabber_Timeout(int *timeout)
{
    *timeout=ImageTimeout;
    return PCO_NOERROR;
}


DWORD CPco_grab_usb::Get_Camera_Settings()
{
  DWORD err=PCO_NOERROR;

  if(cam&&cam->IsOpen())
  {
   cam->PCO_GetTransferParameter(&usbpar,sizeof(usbpar));
   writelog(INFO_M,hgrabber,"Get_Camera_Settings: usbpar.MaxNumUsb       %d",usbpar.MaxNumUsb);
   writelog(INFO_M,hgrabber,"Get_Camera_Settings: usbpar.Clockfrequency  %d",usbpar.ClockFrequency);
   writelog(INFO_M,hgrabber,"Get_Camera_Settings: usbpar.Transmit        0x%x",usbpar.Transmit);
   writelog(INFO_M,hgrabber,"Get_Camera_Settings: usbpar.UsbConfig       0x%x",usbpar.UsbConfig);
   writelog(INFO_M,hgrabber,"Get_Camera_Settings: usbpar.ImgTransMode    0x%x",usbpar.ImgTransMode);

   if(description.wSensorTypeDESC==0)
   {
    cam->PCO_GetCameraDescriptor(&description);
    cam_colorsensor=description.wColorPatternTyp ? 1 : 0;
    writelog(INFO_M,hgrabber,"Get_Camera_Settings: cam_colorsensor       %d",cam_colorsensor);
   }

   err=cam->PCO_GetTimestampMode(&cam_timestampmode);
   if(err==PCO_NOERROR)
   {
    writelog(INFO_M,hgrabber,"Get_Camera_Settings: cam_timestampmode     %d",cam_timestampmode);
    err=cam->PCO_GetPixelRate(&cam_pixelrate);
   }
   if(err==PCO_NOERROR)
   {
    writelog(INFO_M,hgrabber,"Get_Camera_Settings: cam_pixelrate         %d",cam_pixelrate);
    err=cam->PCO_GetDoubleImageMode(&cam_doublemode);
    if((err&0xC000FFFF)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)
     err=PCO_NOERROR;
   }

   if(err==PCO_NOERROR)
   {
    writelog(INFO_M,hgrabber,"Get_Camera_Settings: cam_doublemode        %d",cam_doublemode);
    err=cam->PCO_GetNoiseFilterMode(&cam_noisefilter);
    if((err&0xC000FFFF)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)
     err=PCO_NOERROR;
   }

   if(err==PCO_NOERROR)
   {
    writelog(INFO_M,hgrabber,"Get_Camera_Settings: cam_noisefilter       %d",cam_noisefilter);
    err=cam->PCO_GetBitAlignment(&cam_align);
   }

   if(err==PCO_NOERROR)
   {
    writelog(INFO_M,hgrabber,"Get_Camera_Settings: cam_align             %d",cam_align);
    act_align=cam_align;
    err=cam->PCO_GetActualSize(&cam_width,&cam_height);
   }
   if(err==PCO_NOERROR)
   {
    writelog(INFO_M,hgrabber,"Get_Camera_Settings: cam_width             %d",cam_width);
    writelog(INFO_M,hgrabber,"Get_Camera_Settings: cam_height            %d",cam_height);
   }
  }
  return err;
}


DWORD CPco_grab_usb::PostArm(int userset)
{
    DWORD err=PCO_NOERROR;
    writelog(PROCESS_M,hgrabber,"%s(%d)",__FUNCTION__,userset);

    if(err==PCO_NOERROR)
     err=Get_Camera_Settings();

    if((err==PCO_NOERROR)&&(userset==0))
    {
//     writelog(PROCESS_M,hgrabber,"PostArm: call Set_DataFormat(0x%x)",clpar.DataFormat);
//     err=Set_DataFormat(DataFormat);
     if(err==PCO_NOERROR)
     {
      writelog(PROCESS_M,hgrabber,"PostArm: call Set_Grabber_Size(%d,%d)",cam_width,cam_height);
      err=Set_Grabber_Size(cam_width,cam_height);
     }

/*
     if(err==PCO_NOERROR)
     {
      if((nr_of_buffer>0)&&(size_alloc!=act_dmalength))
      {
       int bufnum=nr_of_buffer;
       writelog(PROCESS_M,hgrabber,"PostArm: reallocate %d buffers",bufnum);
       Free_Framebuffer();
       err=Allocate_Framebuffer(bufnum);
      }
     }
*/
    }
    return err;
}


DWORD  CPco_grab_usb::Allocate_Framebuffer(int nr_of_buffer ATTRIBUTE_UNUSED)
{
    DWORD err=PCO_NOERROR;
    return err;
}


DWORD  CPco_grab_usb::Free_Framebuffer()
{
    DWORD err=PCO_NOERROR;
    return err;
}



///////////////////////////////////////////////////////////////////////
//
//  Use these functions if you want simple image transfers
//
// Acquire_Image: Get a single image, timeout setting optional
//
// Acquire_Image_Async: Get single/multiple images asynchronous.
//                      If a preview is needed, use Acquire_Image_Async_wait (this ensures that decoding of the image is complete)
//                      If a preview is not needed, use Acquire_Image_Async for the first X-1 images and
//                      Acquire_Image_Async_wait for the last image.
//                      See usb_grabthreadworker.cpp if you need an example
//
//////////////////////////////////////////////////////////////////////

DWORD CPco_grab_usb::Acquire_Image(void *adr)
{
    DWORD err=PCO_NOERROR;
    err=Acquire_Image(adr,ImageTimeout);
    return err;
}

DWORD CPco_grab_usb::Acquire_Image(void *adr,int timeout)
{
    DWORD err=PCO_NOERROR;
    err=usb_read_image(adr,act_dmalength,timeout,0,0);
    return err;
}

DWORD CPco_grab_usb::Get_Image(WORD Segment,DWORD ImageNr,void *adr)
{
    DWORD err=PCO_NOERROR;
    err=usb_read_image(adr,act_dmalength,1000,Segment,ImageNr);
    return err;
}


DWORD CPco_grab_usb::Acquire_Image_Async(void *adr)
{
    DWORD err = PCO_NOERROR;
    err = Acquire_Image_Async(adr, ImageTimeout, false);
    return err;
}


DWORD CPco_grab_usb::Acquire_Image_Async(void *adr, int timeout)
{
    DWORD err = PCO_NOERROR;
    err = Acquire_Image_Async(adr, timeout, false);
    return err;
}

DWORD CPco_grab_usb::Acquire_Image_Async_wait(void *adr)
{
    DWORD err = PCO_NOERROR;
    err = Acquire_Image_Async(adr, ImageTimeout, true);
    return err;
}

DWORD CPco_grab_usb::Acquire_Image_Async_wait(void *adr, int timeout)
{
    DWORD err = PCO_NOERROR;
    err = Acquire_Image_Async(adr, timeout, true);
    return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main Async acquire function (you can use the overloaded functions above to help you fill in the parameters)
// adr: adress of the buffer to write the current image to
// timeout: the timeout of the command
// cancel: pointer for the cancel option. If you want to stop grabbing, set this to TRUE. If you just want one picture, a NULL pointer is just fine (or use the Aquire image without async)
// WARNING: If you don't use CANCEL on the last image, the images might not be valid right away!
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD CPco_grab_usb::Acquire_Image_Async(void *adr, int timeout,BOOL waitforimage)
{
    DWORD err = PCO_NOERROR;
    err = usb_async_image(adr, act_dmalength, timeout,waitforimage);
    return err;

}

/////////////////////////////////////////////////////////////////
//
// Looks for USB devices and gets their endpoints for the grabber
//
////////////////////////////////////////////////////////////////


DWORD CPco_grab_usb::usb_get_endpoints()
{
    DWORD err;
    int eptCount;
    int ep_contr_in;

    int alt_set=cam->get_altsetting();

    struct libusb_device_descriptor desc;
    libusb_config_descriptor *libusb_ConfigDescr;
    libusb_device *dev;

    dev=libusb_get_device(libusb_hdev);
    err=libusb_get_device_descriptor(dev,&desc);
    if (err!=0)
    {
        writelog(ERROR_M,(PCO_HANDLE)1,"usb_get_endpoints: Get device descriptor failed (%d) %s",err,libusb_error_name(err));
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    err=libusb_get_config_descriptor(dev, 0, &libusb_ConfigDescr);
    if (err!=0)
    {
        writelog(ERROR_M,(PCO_HANDLE)1,"usb_get_endpoints: Get config descriptor failed (%d) %s",err,libusb_error_name(err));
        return PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    }

    eptCount = libusb_ConfigDescr->interface[0].altsetting[alt_set].bNumEndpoints;
    ep_contr_in=0;

    err=PCO_NOERROR;

    switch(desc.idProduct)
    {
    case USB_PID_IF_GIGEUSB_20:     // 0x0001  FX2 (Cypress 68013a)
    case USB_PID_CAM_PIXFLY_20:     // 0x0002  FX2 (Cypress 68013a)
        ep_contr_in = USB_EP_FX2_IMG_IN;
        break;

    case USB_PID_IF_GIGEUSB_30:     // 0x0003  FX3 (Cypress CYUSB3014-BZX) Application Code
    case USB_PID_IF_GIGEUSB_30_B1:  // 0x0004  FX3 (Cypress CYUSB3014-BZX) SPI Boot Code (FPGA Update)
    case USB_PID_IF_GIGEUSB_30_B2:  // 0x0005  FX3 (Cypress CYUSB3014-BZX) I2C Boot Code (FX3 Update)
    case USB_PID_CAM_EDGEUSB_30:    // 0x0006  Fx3 (Cypress CYUSB3014-BZX)
        ep_contr_in = USB_EP_FX3_IMG_IN;
        break;


    case USB_PID_CAM_FLOW_20:       // 0x0007  AVR32
    case USB_PID_CAM_EDGEHS_20:     // 0x0008  AVR32
    default:
        writelog(ERROR_M,(PCO_HANDLE)1,"usb_get_endpoints: No image endpoint avaiable");
        err=PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
        break;
    }

    for(int  i=0; (i<eptCount)&&(err==PCO_NOERROR);  i++)
    {
        if(libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].bmAttributes == LIBUSB_TRANSFER_TYPE_BULK)
        {
            if(libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].bEndpointAddress==ep_contr_in)
            {
                ImageInAddr = libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].bEndpointAddress;
                writelog(INIT_M,hgrabber,"usb_get_endpoints: BulkControlIn found (EP 0x%02x) %d", ImageInAddr, i);
                ImageInMaxPacketSize=libusb_ConfigDescr->interface[0].altsetting[alt_set].endpoint[i].wMaxPacketSize;
                writelog(INIT_M,hgrabber, "get_endpoints: ImageInMaxPacketSize %d",ImageInMaxPacketSize);
            }
        }
    }
    if(libusb_ConfigDescr)
    {
        libusb_free_config_descriptor(libusb_ConfigDescr);
        libusb_ConfigDescr = NULL;
    }


    if(ImageInAddr==0)
        err=PCO_ERROR_DRIVER_NODRIVER | PCO_ERROR_DRIVER_USB;
    else
    {
        last_buffer_1=(unsigned char*)malloc(ImageInMaxPacketSize*2);
        last_buffer_2=(unsigned char*)malloc(ImageInMaxPacketSize*2);
    }



    return err;
}


///////////////////////////////////////////////////
//
// Tries to clear any pending USB inputs
//
///////////////////////////////////////////////////

DWORD CPco_grab_usb::usb_clear_input()
{
    DWORD err = PCO_NOERROR;
    int Len;
    int totallength = 0;
    unsigned char *buf;
    buf=(unsigned char*)malloc(ImageInMaxPacketSize);
    Len = ImageInMaxPacketSize;
    while(Len!=0)
    { //while there are packets to read
        err=libusb_bulk_transfer(libusb_hdev, ImageInAddr, buf, Len, &Len,1);
        totallength+=Len;
    }
    if(err!=0)
    {
        writelog(PROCESS_M,hgrabber,"usb_clear_input: nothing to clear");
        err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
    }
    else
    {
        writelog(COMMAND_M,hgrabber,"usb_clear_input: transfer done *buf 0x%04X  bytes read: %d",*((unsigned short*)buf),totallength);
    }
    free(buf);

    return err;
}

///////////////////////////////////////////////////////////////////
//
// Get a single image from USB where timing isn't critical
// If you want to get multiple images, take a look at the
// asynchronous functions
//
////////////////////////////////////////////////////////////////////



DWORD CPco_grab_usb::usb_read_image(void *buf,int buflen,DWORD ima_timeout,WORD Segment,DWORD ImageNr)
{
    DWORD err;
    int done,Len,tdone;
    unsigned char *bufin;
    int timeout;
    DWORD tc1,tc2;
    int transfer_size,copy_size;
    pthread_t* pthreadarray = NULL;
    int thread_count;
    unsigned char *copybuffer;

    bufin=(unsigned char*)buf;

    // Use async_transfer_sizec if set else LARGE_PACKET_SIZE
    if(async_transfer_size>0)
     transfer_size=async_transfer_size;   
    else
     transfer_size=LARGE_PACKET_SIZE;

    if(transfer_size > buflen)
    {
     transfer_size = buflen;
    }

    //transfer size has to be a multiple of ImageInMaxPacketSize
    copy_size = 0;
    thread_count=0;

    if(transfer_size%ImageInMaxPacketSize)
    {
     transfer_size=(transfer_size/ImageInMaxPacketSize)*ImageInMaxPacketSize;
     copy_size=ImageInMaxPacketSize*4;
     copybuffer = (unsigned char*)malloc(copy_size);
     memset(copybuffer,0,copy_size);
    }

    Len = transfer_size;
    done=0;   //counts transferred bytes
    timeout=ima_timeout; //wait for first transfer with image timeout
    tdone=0;
    tc2=tc1=GetTickCount();

    writelog(PROCESS_M,hgrabber,"usb_read_image: buflen %d first Len %d copy_size %d timeout %d",buflen,Len,copy_size,timeout);

    cam->set_transfer_msgwait(0);

    if((Segment==0)||(ImageNr==0))
     err=cam->PCO_RequestImage();
    else
     err=cam->PCO_ReadImagesFromSegment(Segment,ImageNr,ImageNr);

    if(err!=PCO_NOERROR)
    {
        writelog(ERROR_M,hgrabber,"usb_read_image: request_image returned 0x%x",err);
        return err;
    }

    //in single image mode, using a seperate thread for each block is still the fastest method.
    //If speed isn't relevant, you can decode the whole image at the end (with Pixelfly_decode() )
    //calculate the amount of threads needed and assign an array to hold the thread handles

    if(gl_Coding)
    {
        pthreadarray = (pthread_t*) malloc(sizeof(pthread_t*)*((buflen/transfer_size)+1));
        if(!pthreadarray)
        {
            writelog(INTERNAL_3_M,hgrabber,"Error assigning memory for threads, switching to non-threaded decoding.");
        }
    }


    //while we are not done ...
    while(done<((buflen/ImageInMaxPacketSize)*ImageInMaxPacketSize)) //remove partial packet length at the end
    {
        err=libusb_bulk_transfer(libusb_hdev, ImageInAddr, bufin, Len, &Len, timeout);
        if(err!=0)
        {
            writelog(ERROR_M,hgrabber,"usb_read_image: libusb_bulk_transfer failed (%d) %s", err,libusb_error_name(err));
            err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
            break;
        }
        else
        {
            writelog(INTERNAL_3_M,hgrabber,"usb_read_image: transfer done *buf 0x%04X  bytes read: %d",*((unsigned short*)bufin),Len);
            //start gl_coding thread on the first block
            if (gl_Coding && pthreadarray)
            {
                GL_CODING_PARAMS* params = new GL_CODING_PARAMS;
                params->buffer = bufin;
                params->len = Len;
                params->align = act_align;
                err = pthread_create(&pthreadarray[thread_count], NULL, GL_CodingpThread, (void*)params);
                if(err != 0)
                {
                    writelog(INTERNAL_3_M,hgrabber,"Error creating pthread %d, using non threaded decoding.",thread_count);
                    Pixelfly_decode((WORD*)bufin,Len,act_align);
                }
                thread_count++;
            }
            //if threaded decoding fails, use non-threaded decoding fallback
            if(gl_Coding && !pthreadarray) {
                Pixelfly_decode((WORD*)bufin,Len,act_align);
            }
            // end threading

            bufin+=Len;
            done+=Len;

            if((done+transfer_size)>buflen)
            {
                Len=buflen-done;
//transfer size has to be a multiple of ImageInMaxPacketSize
                if(Len%ImageInMaxPacketSize)
                {
                 Len=(Len/ImageInMaxPacketSize)*ImageInMaxPacketSize;
                }
            }
            else
                Len=transfer_size;

            timeout=packet_timeout;
            writelog(INTERNAL_3_M,hgrabber,"usb_read_image: done %d remaining %d Len %d",done,buflen-done,Len);
        }
        tc2=GetTickCount();
        if((tc2-tc1)>ima_timeout)
        {
            writelog(ERROR_M,hgrabber,"usb_read_image: done %d buflen %d  tc2-tc1 %d > ima_timeout %d",done,buflen,(tc2-tc1),ima_timeout);
            err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
            break;
        }
    }

    if(copy_size)
    {
        Len = copy_size;
        err=libusb_bulk_transfer(libusb_hdev, ImageInAddr, copybuffer, Len, &Len, timeout);
        if(gl_Coding)
            Pixelfly_decode((WORD*)copybuffer,Len,act_align);
        memcpy(bufin,copybuffer,Len);
        done+=Len; 
        writelog(INTERNAL_3_M,hgrabber,"usb_read_image: copy done %d remaining %d Len %d",done,buflen-done,Len);
        if(err!=0)
        {
            writelog(ERROR_M,hgrabber,"usb_read_image: Copy buffer: libusb_bulk_transfer failed (%d) %s", err,libusb_error_name(err));
            err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
        }
    }

    writelog(PROCESS_M,hgrabber,"usb_read_image: all packages done %d time %dms",done,tc2-tc1);

    while(err==PCO_NOERROR)
    {
        WORD recstate;
        writelog(PROCESS_M,hgrabber,"usb_read_image: wait for IMAGE_TRANSFER_DONE message");

        err=cam->PCO_GetRecordingState(&recstate);
        if((tdone=cam->transfer_msgwait())!=0)
            break;
        Sleep_ms(5);
        tc2=GetTickCount();
        if((tc2-tc1)>ima_timeout)
        {
            writelog(ERROR_M,hgrabber,"usb_read_image: wait transfer msg tc2-tc1 %d > ima_timeout %d",(tc2-tc1),ima_timeout);
            err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
            break;
        }
    }

    if(err!=PCO_NOERROR)
    {   
        DWORD erri;
        cam->PCO_CancelImage();
        timeout=packet_timeout;
        Len = ImageInMaxPacketSize;
        erri=libusb_bulk_transfer(libusb_hdev, ImageInAddr, bufin, Len, &Len, timeout);
        writelog(INTERNAL_2_M,hgrabber,"usb_read_image: after cancel libusb transfer done bytes read: %d erri 0x%x",Len,erri);
        writelog(INTERNAL_2_M,hgrabber,"usb_read_image: bufin 0x%04x",*((unsigned short*)bufin));
    }

    if(ImageInAddr==USB_EP_FX3_IMG_IN)
        usb_clear_input();

    // wait for all decoding threads to finish
    if(gl_Coding && pthreadarray)
    {
        for (int i = 0;i<thread_count;i++)
        {
            pthread_join(pthreadarray[i],NULL);
        }
        free(pthreadarray);
    }
    writelog(PROCESS_M,hgrabber,"usb_read_image: transfer bytes read: %d, transfer_done %d return 0x%x",done,tdone,err);

    return err;
}

////////////////////////////////////////////////////////////
//
// Asynchronous image grabbing
// *buf = address to the receive buffer
//  buflen = length of the buffer
// ima_timeout = time in ms after which the transfer stops with a timeout error
// waitforimage = if gl_coding is on (should be on if possible for pco.pixelfly) the images are only immediately valid if this flag is set to TRUE!
// if you are grabbing a larger number of images, you only have to set the flag for the last image (the function waits until ALL acquired images up to this point are valid!)
//
/////////////////////////////////////////////////////////////

DWORD CPco_grab_usb::usb_async_image(void *buf,int buflen,DWORD ima_timeout, BOOL waitforimage)
{
    unsigned char *tadr;
    int timeout;
    DWORD tc1,tc2;
    DWORD err=PCO_NOERROR;
    int request_send=0;
    int async_bytes_setup,last_done;

    writelog(PROCESS_M,hgrabber,"usb_async_image: start");
    if((async_transfer_status&ASYNC_ALLOC_1)==0)
    {
        writelog(ERROR_M,hgrabber,"usb_async_image: async_transfer_status not alloc");
        return PCO_ERROR_DRIVER_IOFAILURE | PCO_ERROR_DRIVER_USB;
    }

    if(buflen>act_dmalength)
    {
        writelog(ERROR_M,hgrabber,"usb_async_image: buflen %d > act_dmalength %d ",buflen,act_dmalength);
        return PCO_ERROR_BUFFERSIZE | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
    }

    tadr=(unsigned char *)buf;
    async_buffer_adr=tadr;
    async_bytes_done=async_packets_done=async_bytes_setup=0;
    last_done=0;

    async_transfer_status|=ASYNC_PENDING_1;
    async_transfer_status&=~ASYNC_ERROR;

    int i;
    int num=async_transfer_num;
    if(async_transfer_status&ASYNC_NEED_LAST) {
        num--;
    }
    if(async_transfer_status&ASYNC_USE_COPY_BUFFER)
        num--;

    timeout=ima_timeout; //set timeout

    //Generate the request packets and submit them all at once
    //the function async_callback() is invoked as soon as a partial transfer is completed
    //the image request is only sent to the camera after the first request has been submitted (speeds up the process a bit)

    tc2=tc1=GetTickCount();
    for(i=0;i<num;)
    {

//only 16Mbyte of Data can be setup in libusb, if image size is larger we have to wait until the first packages are done
     if(async_bytes_setup+async_transfer_size<16*1024*1024)
     {

//do not use timeout for the transfer, we have to handle triggered images too
//we will cancel the transfers if the image timeout timed out
        libusb_fill_bulk_transfer(async_transfer_1[i],libusb_hdev,ImageInAddr,tadr,async_transfer_size,&this->async_callback,(void*)this,0);

        err=libusb_submit_transfer(async_transfer_1[i]);
        if(err!=0)
        {
         writelog(ERROR_M,hgrabber,"usb_async_image: submit transfer failed. Error: %s",libusb_error_name((err)));
         err=PCO_NOERROR;
        }

//        timeout=packet_timeout;
        tadr+=async_transfer_size;
        writelog(PROCESS_M,hgrabber,"usb_async_image: submit %d done request_send %d",i,request_send);
        if((num>1)&&(i>=0)&&(request_send==0))
        {
            err=cam->PCO_RequestImage();
            cam->set_transfer_msgwait(0);
            request_send=1;
        }
        async_bytes_setup+=async_transfer_size;
        i++;
      }
      else
      {
        struct timeval tv;
        if(async_packets_done==0)
         tv.tv_usec=500000;
        else
         tv.tv_usec=20000;
        tv.tv_sec=0;
        libusb_handle_events_timeout(cam->libusb_ctx,&tv);

        tc2=GetTickCount();
        writelog(PROCESS_M,hgrabber,"usb_async_image: packets_done %d bytes_done %d  tc2-tc1 %d transfer_num %d",async_packets_done,async_bytes_done,(tc2-tc1),async_transfer_num);
        if((tc2-tc1)>ima_timeout)
        {
            writelog(ERROR_M,hgrabber,"usb_async_image: done %d buflen %d  tc2-tc1 %d > ima_timeout %d",async_bytes_done,buflen,(tc2-tc1),ima_timeout);
            err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
            break;
        }
        if(async_packets_done>last_done)
        {
         async_bytes_setup-=(async_transfer_size*(async_packets_done-last_done));
         last_done=async_packets_done;
        }
      }
    }


    if(async_transfer_status&ASYNC_NEED_LAST)
    {
        libusb_fill_bulk_transfer(async_transfer_1[i],libusb_hdev,ImageInAddr,tadr,async_last_size,&this->async_callback,(void*)this,0);
        libusb_submit_transfer(async_transfer_1[i]);
        i++;
        tadr+=async_last_size;
        writelog(PROCESS_M,hgrabber,"usb_async_image: submit last done request_send %d",request_send);
    }


    if(async_transfer_status&ASYNC_USE_COPY_BUFFER)
    {
        libusb_fill_bulk_transfer(async_transfer_1[i],libusb_hdev,ImageInAddr,last_buffer_1,ImageInMaxPacketSize*2,&this->async_callback,(void*)this,0);
        libusb_submit_transfer(async_transfer_1[i]);
        writelog(PROCESS_M,hgrabber,"usb_async_image: submit copy buffer done request_send %d",request_send);
    }


    tc2=tc1=GetTickCount();
    async_transfer_status|=ASYNC_SETUP_1;
    if(request_send==0)
    {
        err=cam->PCO_RequestImage();
        cam->set_transfer_msgwait(0);
        request_send=1;
    }
    if(err!=PCO_NOERROR)
    {
        writelog(ERROR_M,hgrabber,"usb_async_image: request_image returned 0x%x",err);
        //cancel all transfers
        for(i=0;i<async_transfer_num;i++)
        {
            libusb_cancel_transfer(async_transfer_1[i]);
        }
    }

    // Wait for all packets to arrive

    while(async_packets_done<async_transfer_num)
    {
        struct timeval tv;

        if(async_packets_done==0)
         tv.tv_usec=500000; //check every 500ms when waiting for first package
        else
         tv.tv_usec=30000;  //check every 30ms if transfer is running
        tv.tv_sec=0;
        libusb_handle_events_timeout(cam->libusb_ctx,&tv);

        tc2=GetTickCount();
        if(async_packets_done>last_done)
        {
         writelog(PROCESS_M,hgrabber,"usb_async_image: packets_done %d bytes_done %d  tc2-tc1 %d transfer_num %d",async_packets_done,async_bytes_done,(tc2-tc1),async_transfer_num);
         last_done=async_packets_done;
        }
        else
        {
         writelog(PROCESS_M,hgrabber,"usb_async_image: no additional packets arrived yet packets_done %d",async_packets_done);
        }

        if((tc2-tc1)>ima_timeout)
        {
            writelog(ERROR_M,hgrabber,"usb_async_image: done %d buflen %d  tc2-tc1 %d > ima_timeout %d",async_bytes_done,buflen,(tc2-tc1),ima_timeout);
            err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
            break;
        }
    }


    if((err!=PCO_NOERROR)||(async_transfer_status&ASYNC_ERROR))
    {
        struct timeval tv;
        tv.tv_sec=0;
        tv.tv_usec=10000;

        writelog(ERROR_M,hgrabber,"usb_async_image: err 0x%x  async_transfer_status 0x%x & 0x%x",err,async_transfer_status,ASYNC_ERROR);

        for(i=async_packets_done;i<async_transfer_num;i++)
        {
            libusb_cancel_transfer(async_transfer_1[i]);
            libusb_handle_events_timeout(cam->libusb_ctx,&tv);
        }
        writelog(ERROR_M,hgrabber,"usb_async_image: found error after cancel");
        if(async_transfer_status&ASYNC_ERROR)
         err= PCO_ERROR_DRIVER_IOFAILURE | PCO_ERROR_DRIVER_USB;
    }
    tc2=GetTickCount();

    async_transfer_status&=~(ASYNC_SETUP_1|ASYNC_PENDING_1);
    if(err==PCO_NOERROR)
    {
        writelog(PROCESS_M,hgrabber,"usb_async_image: all packages done %d time %dms async_transfer_status 0x%x",async_bytes_done,tc2-tc1,async_transfer_status);

        if(async_transfer_status&ASYNC_USE_COPY_BUFFER)
        {
            //int s=(int)(tadr-(unsigned char*)buf+buflen);
            memcpy(tadr,last_buffer_1,async_copy_buffer_size);
        }
        writelog(PROCESS_M,hgrabber,"usb_read_image: wait for IMAGE_TRANSFER_DONE message");

//Catch the "image done" telegram with a GetRecordingState loop
       while(err==PCO_NOERROR)
       {
        WORD recstate;
        int tdone = 0;
        DWORD erri;

        erri=cam->PCO_GetRecordingState(&recstate);
        if(erri!=PCO_NOERROR)
         writelog(ERROR_M,hgrabber,"usb_async_image: wait transfer PCO_GetRecordingState() erri 0x%x",erri);

        if((tdone=cam->transfer_msgwait())!=0)
         break;

        Sleep_ms(5);
        tc2=GetTickCount();
        if((tc2-tc1)>ima_timeout)
        {
            writelog(ERROR_M,hgrabber,"usb_async_image: wait transfer msg tc2-tc1 %d > ima_timeout %d",(tc2-tc1),ima_timeout);
            err=PCO_ERROR_TIMEOUT | PCO_ERROR_DRIVER | PCO_ERROR_DRIVER_USB;
            break;
        }
       }
       writelog(PROCESS_M,hgrabber,"usb_read_image: IMAGE_TRANSFER_DONE");
    }

    if(err!=PCO_NOERROR)
    {
        int Len;
        DWORD erri;

        cam->PCO_CancelImage();
        timeout=packet_timeout;
        Len = ImageInMaxPacketSize;
        erri=libusb_bulk_transfer(libusb_hdev, ImageInAddr, last_buffer_1, Len, &Len, timeout);
        writelog(INTERNAL_2_M, hgrabber, "usb_async_image: after cancel transfer done bytes read: %d erri %d", Len,erri);
    }

//this call takes about 1.5ms, even if no data is in
//next revision test with async call
    if(ImageInAddr==USB_EP_FX3_IMG_IN)
        usb_clear_input();

    //wait option set
    if (waitforimage & gl_Coding)
    {
        //wait for coding thread to finish
        for (unsigned int i = 0;i<gl_codingthreads.size();i++)
        {
            pthread_join(*gl_codingthreads[i],NULL);
            delete gl_codingthreads[i];
        }
        gl_codingthreads.clear();
        writelog(PROCESS_M,hgrabber,"Finished decoding threads.");
    }
    return err;
}

//Callback function that is invoked when a packet arrives
//The function starts a gl_coding thread to decode the incoming image (despite the overhead for threading this is significantly faster than any other solution)

void CPco_grab_usb::async_callback(struct libusb_transfer *transfer)
{
    CPco_grab_usb* mygrab;
    mygrab=(CPco_grab_usb*)transfer->user_data;
    if(mygrab->gl_Coding)
    {
        //threaded image decoding, create new thread
        pthread_t* thread = new pthread_t;
        mygrab->gl_codingthreads.push_back(thread);
        int threadret;
        //extract parameters from transfer packet into gl_coding_params struct
        GL_CODING_PARAMS* params = new GL_CODING_PARAMS;
        params->buffer=transfer->buffer;
        params->len=transfer->actual_length;
        params->align=mygrab->act_align;
        threadret = pthread_create(thread,NULL,GL_CodingpThread,(void*) params);
        if(threadret)
        {
            mygrab->writelog(ERROR_M,mygrab->hgrabber,"Error - pthread_create() return code: %d\n, using non-threaded decoding (slow!)",threadret);
            Pixelfly_decode((WORD*)transfer->buffer,transfer->actual_length,mygrab->act_align);
        }
        else
        {
            mygrab->writelog(INTERNAL_3_M,mygrab->hgrabber,"Started decoding thread...");
        }
        //not threaded image decoding
        //GL_Coding(transfer->buffer,transfer->actual_length,act_align);
    }

    if(transfer->status==LIBUSB_TRANSFER_COMPLETED)
    {
        mygrab->async_packets_done++;
        mygrab->async_bytes_done+=transfer->length;
        mygrab->writelog(PROCESS_M,mygrab->hgrabber,"async callback: transfer complete adr %p",transfer->buffer);
    }
    else if(transfer->status==LIBUSB_TRANSFER_CANCELLED)
    {
        mygrab->writelog(PROCESS_M,mygrab->hgrabber,"async callback: transfer cancel adr %p",transfer->buffer);
    }
    else
    {
        mygrab->async_transfer_status|=ASYNC_ERROR;
    }

    if(transfer->status==LIBUSB_TRANSFER_ERROR)
        mygrab->writelog(ERROR_M,mygrab->hgrabber,"async callback: transfer error");
    if(transfer->status==LIBUSB_TRANSFER_TIMED_OUT)
        mygrab->writelog(ERROR_M,mygrab->hgrabber,"async callback: transfer timeout");
    if(transfer->status==LIBUSB_TRANSFER_STALL)
        mygrab->writelog(ERROR_M,mygrab->hgrabber,"async callback: transfer stall");
    if(transfer->status==LIBUSB_TRANSFER_NO_DEVICE)
        mygrab->writelog(ERROR_M,mygrab->hgrabber,"async callback: transfer no device");
    if(transfer->status==LIBUSB_TRANSFER_OVERFLOW)
        mygrab->writelog(ERROR_M,mygrab->hgrabber,"async callback: transfer overflow");
}

//This is used by the threaded decoding, we can only pass a void* parameter to a thread, so some additional functionality is needed
void *GL_CodingpThread(void* param)
{
    GL_CODING_PARAMS* params = reinterpret_cast<GL_CODING_PARAMS*>(param);
    Pixelfly_decode((WORD*) params->buffer,(int)params->len,params->align);
    delete params;
    return 0;
}


void CPco_grab_usb::Sleep_ms(int time) //time in ms
{
#ifdef __linux__
    int ret_val;
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(0,&rfds);
    tv.tv_sec=time/1000;
    tv.tv_usec=(time%1000)*1000;
    ret_val=select(1,NULL,NULL,NULL,&tv);
    if(ret_val<0)
        writelog(ERROR_M,hgrabber,"Sleep: error in select");
#endif
#ifdef WIN32
    if(time<25)
     time=25;
    Sleep(time);
#endif
}

#ifndef WIN32
DWORD CPco_grab_usb::GetTickCount(void)
{
    struct timeval t;
    gettimeofday(&t,NULL);
    return(t.tv_usec/1000+t.tv_sec*1000);
}
#endif
