/*
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
#include<sstream>
#include"zyla/FitsOperations.h"
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "libflipro.h"
/*
#include <map>
#include "stdint.h"
#include "stdlib.h"
#include <stdio.h>
#include <iostream>
*/
#include <ctime>
#include<thread>
#define FLI_TEST_MAX_SUPPORTED_CAMERAS   (4)



namespace rts2camd
{


class FliKepler:public Camera
{
public:

    FliKepler (int in_argc, char **in_argv);
    virtual ~FliKepler (void)
    {
    }


    virtual int commandAuthorized(rts2core::Connection * conn);


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

        setSize(2048,2048,0,0);
        return Camera::initChips ();

    }
    virtual int info ()
    {
        return Camera::info ();
    }
    virtual int startExposure ();
    virtual int stopExposure ();

    virtual long isExposing ()
    {

        //logStream(MESSAGE_INFO)<<"is exposing calisti"<<sendLog;
	bool isAvailable = false;
        iResult=FPROFrame_IsAvailable(s_iDeviceHandle,&isAvailable);
        //logStream (MESSAGE_ERROR) << "value of is available is  " << isAvailable << sendLog;

	if(!isAvailable)return 200;
	return -2;

    }
    virtual int doReadout ();

protected:
    virtual void initBinnings ()
    {
        Camera::initBinnings ();

        addBinning2D (2, 2);
        addBinning2D (3, 3);
        addBinning2D (4, 4);
        addBinning2D (1, 2);
        addBinning2D (2, 1);
        addBinning2D (4, 1);
    }


    virtual int setValue (rts2core::Value *old_value, rts2core::Value *new_value);


private:


    //  Function declarations
    int32_t SetFrameInfo(int32_t iDeviceHandle,double exposureTime);
    long int unix_timestamp();
    int setMode(uint32_t setModeTo );
    int setModeIndex();
    int setGainLDR(uint32_t setGainTo);
    int setGainHDR(uint32_t setGainTo);
    int setGainIndexLDR();
    int setGainIndexHDR();
    //  Data declarations
    int32_t        s_iDeviceHandle;
    uint32_t              uiNumDetectedDevices;
    FPRODEVICEINFO s_camDeviceInfo[FLI_TEST_MAX_SUPPORTED_CAMERAS];

    int32_t  iResult;
    int32_t  iGrabResult;
    uint32_t i,k;
    uint8_t  *pFrame;

    uint8_t  pixBuff1;
    uint8_t pixBuff2;
    uint8_t pixBuff3;
    uint32_t uiSizeGrabbed;

    double pSetPoint=0;
    int32_t actTemp =0;

    // Make sure we have space for the image frame
    // A couple things to note about the frame size.
    //    1.) The macros we use here are supplied by the FPRO API but they
    //        only work if you are using 12 bit pixels.  If you change the pixel
    //        size you will need to do your own calculations to allocate memory.
    //    2.) ALL image frames come prepended with meta data.  The FPRO_METADATA_SIZE macro
    //        is supplied by the FPRO API and returns the number of bytes in the meta data.
    //        the format of the meta data is documented in your users manual.



    uint32_t uiFramSizeInBytes = FPRO_IMAGE_DIMENSIONS_TO_FRAMEBYTES(2048, 2048) + FPRO_METADATA_SIZE;



    rts2core::ValueSelection * mode;
//     Choose LDR mode for required frame rate greater than 24 FPS (exposures <42 ms).
//     Choose HDR mode for a dynamic range greater than 0 – 4095 counts
//    Choose LDC when your exposures are sufficiently long that dark current growth uses a significant percentage of full well capacity. (Also cool sensor to lowest possible operating temp.)
//Do not choose LDC for short exposures.
    rts2core::ValueSelection *gainLDR;
    rts2core::ValueSelection *gainHDR;

    std::vector<std::map<std::string,std::pair<std::string,std::string>>> headerVector;
    //std::pair<std::string,std::string> dueTimerVector;
    std::map<std::string,std::pair<std::string,std::string>> header;

    std::vector<unsigned short *> fitsBuffer;
    std::vector<unsigned char*>bufferVector;
    std::vector<unsigned short*> fitsThreadVector;
    std::vector<std::map<std::string,std::pair<std::string,std::string>>> headerThreadVector;
    std::vector<std::pair<std::string,std::string>> dueTimerThreadVector;
    std::vector<std::pair<std::string,std::string>> dueTimerVector;

     void writeToFitsThread(int *p);

      unsigned short *img;

};//end of class

};//end of namespace

using namespace rts2camd;

int FliKepler::setValue (rts2core::Value *old_value, rts2core::Value *new_value)
{

    if (old_value == mode)
        return setMode (new_value->getValueInteger ()) == 0 ? 0 : -2;
    if (old_value == gainLDR)
        return setGainLDR (new_value->getValueInteger ()) == 0 ? 0 : -2;
    if (old_value == gainHDR)
        return setGainHDR (new_value->getValueInteger ()) == 0 ? 0 : -2;


    return Camera::setValue (old_value, new_value);
}

int FliKepler::initHardware()
{


    // first get the list of available devices
    uiNumDetectedDevices = FLI_TEST_MAX_SUPPORTED_CAMERAS;
    iResult = FPROCam_GetCameraList(s_camDeviceInfo, &uiNumDetectedDevices);
    if ((iResult >= 0) && (uiNumDetectedDevices > 0))
    {
        logStream(MESSAGE_INFO)<<" there is a camera...."<<sendLog;

        // Open the first device in the list
        s_iDeviceHandle = -1;
        iResult = FPROCam_Open(&s_camDeviceInfo[0], &s_iDeviceHandle);
        if ((iResult >= 0) && (s_iDeviceHandle >= 0))
        {
            logStream(MESSAGE_INFO)<<" device handled..."<<sendLog;

        }
        else {
            logStream(MESSAGE_INFO)<<" can not handle camera...."<<sendLog;
            return -1;
        }

    }

    else {
        logStream(MESSAGE_INFO)<<"No cameras found...."<<sendLog;
        return -1;
    }



    iResult=FPROCtrl_SetTemperatureSetPoint(s_iDeviceHandle,-15);
    logStream(MESSAGE_INFO)<<"result after set temp set point"<<iResult<<sendLog;
    iResult=FPROCtrl_GetTemperatureSetPoint	(s_iDeviceHandle,&pSetPoint);
    logStream(MESSAGE_INFO)<<"result after get temp set point"<<iResult<<sendLog;

    logStream(MESSAGE_INFO)<<"temperature set point is " <<pSetPoint<<sendLog;
    iResult=FPROCtrl_GetSensorTemperature(s_iDeviceHandle,&actTemp);
    logStream(MESSAGE_INFO)<<"sensor temperature is " <<actTemp<<" iresult is "<<iResult<<sendLog;


    uint32_t uiCurrentMode;
    uint32_t uiGainIndex;
    uint32_t uiModeCount;
    setModeIndex();
    iResult= FPROSensor_GetModeCount(s_iDeviceHandle, &uiModeCount, &uiCurrentMode);
    mode->setValueInteger(uiCurrentMode);
    
   
    setGainIndexLDR();
    setGainIndexHDR();


    FPROSensor_GetGainIndex(s_iDeviceHandle, FPRO_GAIN_TABLE_LDR, &uiGainIndex);
    gainLDR->setValueInteger(uiGainIndex);
    FPROSensor_GetGainIndex(s_iDeviceHandle, FPRO_GAIN_TABLE_HDR, &uiGainIndex);
    gainLDR->setValueInteger(uiGainIndex);


    logStream(MESSAGE_INFO)<<"inithardware bitti"<<sendLog;


    return initChips();
}
int FliKepler::startExposure()
{

   

        // Set up your exposure and frame parameters
        iResult = SetFrameInfo(s_iDeviceHandle,exposure->getValueDouble());
        if (iResult >= 0)
        {
            // all is well - now you can start grabbing image frames
            // This loop will grab numberOfExposures of frames.
            iGrabResult = 0;


            FPROCtrl_SetShutterOverride (s_iDeviceHandle,true);
            FPROCtrl_SetShutterOpen (s_iDeviceHandle,false);
            //shutter open
            FPROCtrl_SetShutterOpen	(s_iDeviceHandle,true);
                logStream(MESSAGE_INFO)<<"time is " <<unix_timestamp()<<sendLog;

                
		
		//Result=  FPROSensor_SetBinning       (s_iDeviceHandle,binningHorizontal(),binningVertical() );       
		//iResult=  FPROSensor_SetBinning       (s_iDeviceHandle,2,2);
		//logStream(MESSAGE_INFO)<<"result is " << iResult <<" binning is " <<binningHorizontal()<<sendLog;
		
		
		// Start the capture and tell it to get 1 frame
                iResult = FPROFrame_CaptureStart(s_iDeviceHandle, 1);

                if(iResult<0)
                {
		logStream(MESSAGE_INFO)<<"Can not start exposure "<<sendLog;
		return -1;

		}
 
                /*
                if (iResult >= 0)
                {
                    // Grab  the frame- Here you can save the requested image size if you like as
                    // the FPROFrame_GetVideoFrame() will return the actual number of bytes received.
                    // Whatever you decide, make sure you pass the correct size into this API in order
                    // to get the correct number of bytes for your frame.
                    uiSizeGrabbed = uiFramSizeInBytes;
                    iGrabResult = FPROFrame_GetVideoFrame(s_iDeviceHandle, pFrame, &uiSizeGrabbed, 0);
                    iResult= FPROFrame_CaptureStop(s_iDeviceHandle);

                    // If the FPROFrame_GetVideoFrame() succeeded- then process it
                    if (iGrabResult >= 0)
                    {
                        logStream(MESSAGE_INFO)<<"Got a frame"<<sendLog;

                        // here you can do whatever you want with the frame data.
                        if (uiSizeGrabbed ==uiFramSizeInBytes )
                        {
                            // got the correct number of bytes
                            k=0;
                            logStream(MESSAGE_INFO)<<"got the correct number of bytes which is "<<uiSizeGrabbed<<sendLog;
                            for(int i=156; i<6291612; i+=3)

                            {
                                //std::cout<<"k is " <<k<<std::endl;
                                pixBuff1=pFrame[i];
                                pixBuff2=pFrame[i+1];
                                pixBuff3=pFrame[i+2];
                                pFrame2[k]=((pixBuff1&0xFFFF)<<4)|((pixBuff2&0xFFFF)>>4);
                                pFrame2[k+1]=((pixBuff2&0xFF0F)<<8)|((pixBuff3&0xFFFF));
                                k=k+2;
                            }

                            createFits(pFrame2,header,dueTimerVector);

                        }
                        else
                        {
                            // something bad happened with the USB image transfer
                            logStream(MESSAGE_INFO)<<"something bad happened with the USB image transfer"<<sendLog;
                        }
                    }
                }
            
        */
         }
        // Close up shop
        //FPROCtrl_SetShutterOpen (s_iDeviceHandle,false);

        // iResult = FPROCam_Close(s_iDeviceHandle);//we opended handle in inithardware we should not need to close or open it




    return 0;
}
int FliKepler::doReadout ()
{

        pFrame = (uint8_t *)malloc(uiFramSizeInBytes);
        logStream(MESSAGE_INFO)<<"image size byte is " <<getWriteBinaryDataSize()<<sendLog;
  	
	uint32_t pframe2size =((FPRO_IMAGE_DIMENSIONS_TO_FRAMEBYTES(2048*binningHorizontal(), 2048*binningVertical()))/3)*2; 
	logStream(MESSAGE_INFO)<<"pframe size is  " <<pframe2size<<sendLog;

	
	//uint32_t pframe2size = 4194304;

  
        unsigned short *pFrame2= new unsigned short[pframe2size];
        if (NULL == pFrame)
        {
            logStream(MESSAGE_INFO)<<"ERROR getting frame memory.... leaving\n"<<sendLog;
            return -1;
        }

    		 // Grab  the frame- Here you can save the requested image size if you like as
                    // the FPROFrame_GetVideoFrame() will return the actual number of bytes received.
                    // Whatever you decide, make sure you pass the correct size into this API in order
                    // to get the correct number of bytes for your frame.
                    uiSizeGrabbed = uiFramSizeInBytes;
                    iGrabResult = FPROFrame_GetVideoFrame(s_iDeviceHandle, pFrame, &uiSizeGrabbed, 0);
                    iResult= FPROFrame_CaptureStop(s_iDeviceHandle);
                    // Close up shop
		     FPROCtrl_SetShutterOpen (s_iDeviceHandle,false);
		    //         
                    // If the FPROFrame_GetVideoFrame() succeeded- then process it
                    if (iGrabResult >= 0)
                    {
                        logStream(MESSAGE_INFO)<<"Got a frame"<<sendLog;

                        // here you can do whatever you want with the frame data.
                        if (uiSizeGrabbed ==uiFramSizeInBytes )
                        {
                            // got the correct number of bytes
                            k=0;
                            logStream(MESSAGE_INFO)<<"got the correct number of bytes which is "<<uiSizeGrabbed<<sendLog;
                            //for(int i=156; i<6291612; i+=3)
			    for(int i=156;i<((getWriteBinaryDataSize()/4)*3)+156;i+=3)

                            {
                                //std::cout<<"k is " <<k<<std::endl;
                                pixBuff1=pFrame[i];
                                pixBuff2=pFrame[i+1];
                                pixBuff3=pFrame[i+2];
                                pFrame2[k]=((pixBuff1&0xFFFF)<<4)|((pixBuff2&0xFFFF)>>4);
                                pFrame2[k+1]=((pixBuff2&0xFF0F)<<8)|((pixBuff3&0xFFFF));
                                k=k+2;
                            }

                          

                        }
                        else
                        {
                            // something bad happened with the USB image transfer
                            logStream(MESSAGE_INFO)<<"something bad happened with the USB image transfer"<<sendLog;
                        }
                    }

      int ret =  sendReadoutData((char*)pFrame2,getWriteBinaryDataSize());//GetImageSizeBytes());  //getWriteBinaryDataSize());
    if(ret<0)
    {

        logStream (MESSAGE_ERROR) << "Send read out data yapilamadi " << sendLog;
        // Give our memory back
        if (pFrame)
            free((uint8_t*)pFrame);
        if(pFrame2)
            delete pFrame2;
        return -1;
    }


   // Give our memory back
        if (pFrame)
            free((uint8_t*)pFrame);
        if(pFrame2)
            delete pFrame2;


    logStream(MESSAGE_INFO)<<"readout bitti"<<sendLog;



    return -2;	 // imediately send new data
}


int FliKepler::stopExposure ()
{
    logStream(MESSAGE_INFO)<<"stop exposure calisti"<<sendLog;
    return Camera::stopExposure ();

}


int FliKepler::commandAuthorized(rts2core::Connection * conn)
{

    if(conn->isCommand("single"))
    {


    }

    if(conn->isCommand("fast"))
    {

        logStream(MESSAGE_INFO)<<"Beginning of fast  "<<sendLog;

        double exptime;
        long numberOfExposures;
        if (conn->paramNextDouble (&exptime) || conn->paramNextLong (&numberOfExposures) || !conn->paramEnd ())
            return -2;


        pFrame = (uint8_t *)malloc(uiFramSizeInBytes);
        uint32_t pframe2size = 4194304;

        unsigned short *pFrame2= new unsigned short[pframe2size];
        if (NULL == pFrame)
        {
            logStream(MESSAGE_INFO)<<"ERROR getting frame memory.... leaving\n"<<sendLog;
            return -1;
        }

        // Set up your exposure and frame parameters
        iResult = SetFrameInfo(s_iDeviceHandle,exptime);
        if (iResult >= 0)
        {
            // all is well - now you can start grabbing image frames
            // This loop will grab numberOfExposures of frames.
            iGrabResult = 0;


            FPROCtrl_SetShutterOverride (s_iDeviceHandle,true);
            FPROCtrl_SetShutterOpen (s_iDeviceHandle,false);
            //shutter open
            FPROCtrl_SetShutterOpen	(s_iDeviceHandle,true);


            for (i = 0; (i < numberOfExposures) && (iResult >= 0) && (iGrabResult >= 0); ++i)
            {

                header["TIMESTMP"]=std::make_pair(std::to_string(unix_timestamp()),"unix time stamp");
                logStream(MESSAGE_INFO)<<"time is " <<unix_timestamp()<<sendLog;

                // Start the capture and tell it to get 1 frame
                iResult = FPROFrame_CaptureStart(s_iDeviceHandle, 1);
                if (iResult >= 0)
                {
                    // Grab  the frame- Here you can save the requested image size if you like as
                    // the FPROFrame_GetVideoFrame() will return the actual number of bytes received.
                    // Whatever you decide, make sure you pass the correct size into this API in order
                    // to get the correct number of bytes for your frame.
                    uiSizeGrabbed = uiFramSizeInBytes;
                    iGrabResult = FPROFrame_GetVideoFrame(s_iDeviceHandle, pFrame, &uiSizeGrabbed, 0);
                    iResult= FPROFrame_CaptureStop(s_iDeviceHandle);

                    // If the FPROFrame_GetVideoFrame() succeeded- then process it
                    if (iGrabResult >= 0)
                    {
                        logStream(MESSAGE_INFO)<<"Got a frame"<<sendLog;

                        // here you can do whatever you want with the frame data.
                        if (uiSizeGrabbed ==uiFramSizeInBytes )
                        {
                            // got the correct number of bytes
                            k=0;
                            logStream(MESSAGE_INFO)<<"got the correct number of bytes which is "<<uiSizeGrabbed<<sendLog;
                            for(int i=156; i<6291612; i+=3)

                            {
                                //std::cout<<"k is " <<k<<std::endl;
                                pixBuff1=pFrame[i];
                                pixBuff2=pFrame[i+1];
                                pixBuff3=pFrame[i+2];
                                pFrame2[k]=((pixBuff1&0xFFFF)<<4)|((pixBuff2&0xFFFF)>>4);
                                pFrame2[k+1]=((pixBuff2&0xFF0F)<<8)|((pixBuff3&0xFFFF));
                                k=k+2;
                            }
		            logStream(MESSAGE_INFO)<<"Starting create fits "<<sendLog;
                            //createFits(pFrame2,header,std::make_pair("virtual","virtual"));
			    fitsBuffer.push_back(pFrame2);
			    headerVector.push_back(header);

                        }
                        else
                        {
                            // something bad happened with the USB image transfer
                            logStream(MESSAGE_INFO)<<"something bad happened with the USB image transfer"<<sendLog;
                        }
                    }
                }
            
	      
	    
	    }



	    
     int sayac = 0;
     for (unsigned short *imgs : fitsBuffer)
        {

         img = (unsigned short*)malloc( 4194304);
	 memcpy(img, imgs,  4194304);
         fitsThreadVector.push_back(img);
         headerThreadVector.push_back(headerVector[sayac]);
        /*
        rbuf=new char[50];
        sleep(0.1);
        int ret = arduinoSerial->readPort (rbuf, blen, "\r\n");//read exp start
        if(ret)logStream(MESSAGE_INFO)<<"time captures  "<<rbuf<<sendLog;
        else logStream(MESSAGE_INFO)<<"No time captures  "<<sendLog;
        arduinoCapture +=std::string(rbuf);
        sleep(0.1);
        ret = arduinoSerial->readPort (rbuf, blen, "\r\n");//read exp end
        if(ret)logStream(MESSAGE_INFO)<<"time captures  "<<rbuf<<sendLog;
        else logStream(MESSAGE_INFO)<<"No time captures  "<<sendLog;
        arduinoCapture +=" " + std::string(rbuf);


        dueTimerVector.push_back(std::pair<std::string,std::string>(std::string("arduino time capture"),arduinoCapture));
        delete rbuf;
        arduinoCapture="";
        */
	dueTimerVector.push_back(std::pair<std::string,std::string>(std::string("arduino time capture"),std::string("arduino time capture"))); 
	sayac++;
    }

     createFits(fitsBuffer[0],header,std::make_pair("virtual","virtual"));

        }
        // Close up shop
        FPROCtrl_SetShutterOpen (s_iDeviceHandle,false);

        // iResult = FPROCam_Close(s_iDeviceHandle);//we opended handle in inithardware we should not need to close or open it

        dueTimerThreadVector=dueTimerVector;//copy time stamp to fits thread vector

       logStream (MESSAGE_INFO) << "thread oncesi " <<sendLog;

        
       
       /*thread kullanmak icin*/
     /* 
        int ithreadNum =1;

        std::thread t(&FliKepler::writeToFitsThread,this,&ithreadNum);
	
	t.detach();
     */
       /*thread kullanmak icin*/
        logStream (MESSAGE_INFO) << "thread sonrasi " <<sendLog;

        fitsBuffer.clear();
        headerVector.clear();
        dueTimerVector.clear();


// Give our memory back
        
	
	
	if (pFrame)
            free((uint8_t*)pFrame);
        if(pFrame2)
            delete pFrame2;
        
 

    }



    return Camera::commandAuthorized(conn);

}

int32_t FliKepler::SetFrameInfo(int32_t iDeviceHandle,double exposureTime)
{

    int32_t iResult;
    // assume success
    iResult = 0;
    // Enable/disable image data
    // The power on default of the camera is to have image data enabled.
    // This is just shown here in case you are working with test frames and
    // need to set the camera back up to get you real image data.
    iResult = FPROFrame_SetImageDataEnable(iDeviceHandle, true);
    // Set the exposure time
    // The default camera exposure time is 50msecs (for the GSENSE 400)
    // The FPROCtrl_SetExposureTime() API expects the exposure time in
    // nano seconds. The frameDelay paramter is also in nanoseconds
    uint64_t exptime = (uint64_t)(exposureTime*1000000000);
    logStream(MESSAGE_INFO)<<"exposure time is in ns"<<exptime<<sendLog;


    if (iResult >= 0)
        iResult = FPROCtrl_SetExposure(iDeviceHandle, exptime,10,false);
    // Set the Image area
    // By default, the camera sets its image area to its maximum values.
    // For the GSENSE 400 model, that is 2048 columns x 2048 rows
    // But if you were to change the values this is how you would do it.
    if (iResult >= 0)
        iResult = FPROFrame_SetImageArea(iDeviceHandle, 0, 0, 2048, 2048);
    // return our result

      //iResult=  FPROSensor_SetBinning       (s_iDeviceHandle,binningHorizontal(),binningVertical() );       
      iResult==FPROSensor_SetBinning (s_iDeviceHandle,uint32_t(binningHorizontal()),uint32_t(binningVertical()));
      logStream(MESSAGE_INFO)<<"result is " << iResult <<" binning is " <<binningHorizontal()<<sendLog;
      //
      //

    return(iResult);
}

long int FliKepler::unix_timestamp()

{

    time_t t = std::time(0);

    long int now = static_cast<long int> (t);

    return now;

}
int FliKepler::setMode( uint32_t setModeTo )
{
//do not open camera we think it is allready opened. should be tested with camera

// get each of the modes- and process
    uint32_t     uiCurrentMode;
    FPROSENSMODE modeInfo;
    uint32_t     uiModeCount;

    // Get the numer of available modes and the current mode setting (index)
    iResult= FPROSensor_GetModeCount(s_iDeviceHandle, &uiModeCount, &uiCurrentMode);
    if(iResult<0)
    {
        logStream(MESSAGE_INFO)<<" Can not get mode count "<<sendLog;
        return  -1;
    }

    for (i = 0; (i < uiModeCount) && (iResult >= 0); ++i)
    {
        iResult= FPROSensor_GetMode(s_iDeviceHandle, i, &modeInfo);
        if (iResult >= 0)
        {
            // You now have the name of the mode for index 'i'
            // For convenience, the index of the mode is also returned in the
            // FPROSENSMODE structure.  It is this index you will use in FPROSensor_SetMode()
        }
        else  logStream(MESSAGE_INFO)<<" Can not get mode GetMode "<<sendLog;
    }
    // Set the current mode to the second mode in the list
    if (uiModeCount >= 2)
        iResult= FPROSensor_SetMode(s_iDeviceHandle, setModeTo);
    if(iResult<0) {
        logStream(MESSAGE_INFO)<<" Can not get mode SetMode "<<sendLog;
        return -1;
    }
    return 0;

}
int FliKepler::setModeIndex()
{
//do not open camera we think it is allready opened. should be tested with camera

// get each of the modes- and process
    uint32_t     uiCurrentMode;
    FPROSENSMODE modeInfo;
    uint32_t     uiModeCount;

    // Get the numer of available modes and the current mode setting (index)
    iResult= FPROSensor_GetModeCount(s_iDeviceHandle, &uiModeCount, &uiCurrentMode);
    if(iResult<0)
    {
        logStream(MESSAGE_INFO)<<" Can not get mode count "<<sendLog;
        return  -1;
    }

    for (i = 0; (i < uiModeCount) && (iResult >= 0); ++i)
    {
        iResult= FPROSensor_GetMode(s_iDeviceHandle, i, &modeInfo);
        if (iResult >= 0)
        {
            // You now have the name of the mode for index 'i'
            // 		   For convenience, the index of the mode is also returned in the
            // 	      FPROSENSMODE structure.  It is this index you will use in FPROSensor_SetMode()

            std::wstring ws(modeInfo.wcModeName);
            std::string str(ws.begin(), ws.end());
            mode->addSelVal (str.c_str());

            logStream(MESSAGE_INFO)<<"mode number "<<i<<" mode info " <<str.c_str()<<sendLog;

	}
        else  logStream(MESSAGE_INFO)<<" Can not get mode GetMode "<<sendLog;
    }


    mode->setValueInteger(uiCurrentMode);

    return 0;
}

int FliKepler::setGainHDR( uint32_t setGainTo )
{

    uint32_t uiGainIndex;
    // Set the gain index for the table
    FPROSensor_SetGainIndex(s_iDeviceHandle, FPRO_GAIN_TABLE_HDR, setGainTo);
    // Read it back- should be the same as what you set..
    // This is not required for proper operation, just being shown
    // for example purposes
    FPROSensor_GetGainIndex(s_iDeviceHandle, FPRO_GAIN_TABLE_HDR, &uiGainIndex);
    logStream(MESSAGE_INFO)<<"HDR gain is set to "<<uiGainIndex<<sendLog;
    return 0;


}


int FliKepler::setGainLDR( uint32_t setGainTo )
{
    uint32_t uiGainIndex;
    // Set the gain index for the table
    FPROSensor_SetGainIndex(s_iDeviceHandle, FPRO_GAIN_TABLE_LDR, setGainTo);
    // Read it back- should be the same as what you set..
    // This is not required for proper operation, just being shown
    // for example purposes
    FPROSensor_GetGainIndex(s_iDeviceHandle, FPRO_GAIN_TABLE_LDR, &uiGainIndex);
    logStream(MESSAGE_INFO)<<"HDR gain is set to "<<uiGainIndex<<sendLog;

    return 0;


}

int FliKepler::setGainIndexLDR()
{
// get each of the modes- and process
    FPROSENSMODE modeInfo;
    uint32_t uiCapSize;
    uint32_t *pNewTable;
    uint32_t uiGainEntries;
    uint32_t uiGainIndex;
    float    fGainValue;
    static FPROCAP        s_camCapabilities;

    // Get the Camera Capabilities
    uiCapSize = sizeof(FPROCAP);
    iResult = FPROSensor_GetCapabilities(s_iDeviceHandle, &s_camCapabilities, &uiCapSize);

    if (iResult >= 0)
    {
        // Now that you have the capabilities information, you could populate a GUI with
        // the pertinent information like drop down lists or max image sizes etc.
        // Here we show you how to get the gain tables and set an index
        // Each camera will have a set of gain tables for LDR, HDR, and some may have
        // a Global Gain table.  The fields uiLDRGain, uiHDRGain, uiGlobalGain hold the
        // number of entries in their respective table.  If the entry is 0, the table
        // does not exist for this camera.


        if (s_camCapabilities.uiLDRGain > 0)
        {
            // First make sure you have allocated enough memory for the gain table you
            // would like to retrieve- here we are getting the LDR Table.  Each entry is
            // a uint32_t value so we allocate an array of uint32_t
            uiGainEntries = s_camCapabilities.uiLDRGain;
            pNewTable = new uint32_t[s_camCapabilities.uiLDRGain];
            if (FPROSensor_GetGainTable(s_iDeviceHandle, FPRO_GAIN_TABLE_LDR, pNewTable, &uiGainEntries) >= 0)
            {
                // now that you have the table entries you can process them
                for (uint32_t i = 0; i < uiGainEntries; ++i)
                {
                    // Each gain value is scaled by the camera to produce an integer.  To return the value
                    // to a floating point representation, apply the scale factor
                    fGainValue = (float)pNewTable[i] / (float)FPRO_GAIN_SCALE_FACTOR;
                    gainLDR->addSelVal (std::to_string(fGainValue));
                    // Here you could populate a drop down list for a GUI with the Gain value.
                    // Be aware that the gain values are not in ascending or descending order.
                    // The are in the order provided by the sensor. As such, keep the order they are
                    // received in the FPROSensor_GetGainTable so that the index you use in the
                    // FPROSensor_SetGainIndex() matches the index in which they were received.
                }
            }
        }



    }
}
int FliKepler::setGainIndexHDR()
{
// get each of the modes- and process
    FPROSENSMODE modeInfo;
    uint32_t uiCapSize;
    uint32_t *pNewTable;
    uint32_t uiGainEntries;
    uint32_t uiGainIndex;
    float    fGainValue;
    static FPROCAP        s_camCapabilities;

    // Get the Camera Capabilities
    uiCapSize = sizeof(FPROCAP);
    iResult = FPROSensor_GetCapabilities(s_iDeviceHandle, &s_camCapabilities, &uiCapSize);

    if (iResult >= 0)
    {
        // Now that you have the capabilities information, you could populate a GUI with
        // the pertinent information like drop down lists or max image sizes etc.
        // Here we show you how to get the gain tables and set an index
        // Each camera will have a set of gain tables for LDR, HDR, and some may have
        // a Global Gain table.  The fields uiLDRGain, uiHDRGain, uiGlobalGain hold the
        // number of entries in their respective table.  If the entry is 0, the table
        // does not exist for this camera.


        if (s_camCapabilities.uiHDRGain > 0)
        {
            // First make sure you have allocated enough memory for the gain table you
            // would like to retrieve- here we are getting the LDR Table.  Each entry is
            // a uint32_t value so we allocate an array of uint32_t
            uiGainEntries = s_camCapabilities.uiHDRGain;
            pNewTable = new uint32_t[s_camCapabilities.uiHDRGain];
            if (FPROSensor_GetGainTable(s_iDeviceHandle, FPRO_GAIN_TABLE_HDR, pNewTable, &uiGainEntries) >= 0)
            {
                // now that you have the table entries you can process them
                for (uint32_t i = 0; i < uiGainEntries; ++i)
                {
                    // Each gain value is scaled by the camera to produce an integer.  To return the value
                    // to a floating point representation, apply the scale factor
                    fGainValue = (float)pNewTable[i] / (float)FPRO_GAIN_SCALE_FACTOR;
                    gainHDR->addSelVal (std::to_string(fGainValue));

                    // Here you could populate a drop down list for a GUI with the Gain value.
                    // Be aware that the gain values are not in ascending or descending order.
                    // The are in the order provided by the sensor. As such, keep the order they are
                    // received in the FPROSensor_GetGainTable so that the index you use in the
                    // FPROSensor_SetGainIndex() matches the index in which they were received.
                }
            }
        }


    }
}

FliKepler::FliKepler (int in_argc, char **in_argv):Camera (in_argc, in_argv)

{
       createValue (mode, "MODE", "Preset modes of FLI Kepler", true, RTS2_VALUE_WRITABLE, CAM_WORKING);
       createValue (gainLDR, "GAINLDR", "gain table of LDR of FLI Kepler", true, RTS2_VALUE_WRITABLE, CAM_WORKING);
       createValue (gainHDR, "GAINHDR", "gain table of HDR of FLI Kepler", true, RTS2_VALUE_WRITABLE, CAM_WORKING);
		                                  
}
		                                             
void FliKepler::writeToFitsThread(int *p)
{

    logStream(MESSAGE_INFO)<<"inside thread value parameter is "<<*p<<sendLog;



    int sayac = 0;
    for (unsigned short *imgs : fitsThreadVector)
	    {

		       logStream(MESSAGE_INFO)<<"before create fits "<<sendLog;

		       createFits(imgs,headerThreadVector[sayac],dueTimerThreadVector[sayac]);
                       //for(int k =0;k<2048*2048;k++)
		       //logStream(MESSAGE_INFO)<<"imgs "<<k<<" is "<<(int)imgs[k]<<sendLog;

		        sayac++;
	          }


    std::vector<unsigned short*>::iterator i;
    for (i = fitsThreadVector.begin(); i != fitsThreadVector.end(); ++i)
	    {
	        delete (*i);
         }

    fitsThreadVector.clear();
    headerThreadVector.clear();



    //logStream(MESSAGE_INFO)<<"sayac is  "<<sayac << "size of fits thread vector is "<<fitsThreadVector.size()<<sendLog;

    }
    
   

int main (int argc, char **argv)
{
    FliKepler device (argc, argv);
    return device.run ();

}
