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
#include"zyla/FitsOperations.cpp"
#include <unistd.h>
#include <time.h>
#include <pthread.h>
extern "C"
{
#include <Common/PrintVimbaVersion.h>
#include <Common/DiscoverGigECameras.h>
#include <VimbaC/Include/VmbCommonTypes.h>
}



namespace rts2camd
{


class Prosilica:public Camera
{
public:

    

    Prosilica (int in_argc, char **in_argv):Camera (in_argc, in_argv)
    {
        createValue (camera1, "CAMERA1", "first camera", true, RTS2_VALUE_WRITABLE);
        createValue (camera2, "CAMERA2", "second camera", true, RTS2_VALUE_WRITABLE);
        createValue (camera3, "CAMERA3", "third camera", true, RTS2_VALUE_WRITABLE);
        createValue (camera4, "CAMERA4", "fourth camera", true, RTS2_VALUE_WRITABLE);
    }

    virtual ~Prosilica (void)
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

            setSize(4872,3248,0,0);
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

         logStream(MESSAGE_INFO)<<"is exposing calisti"<<sendLog;

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

    VmbError_t          err             = VmbErrorSuccess;
    VmbCameraInfo_t *   pCameras        = NULL;
    VmbUint32_t         i               = 0;
    VmbUint32_t         nCount          = 0;
    VmbUint32_t         nFoundCount     = 0;


    const VmbUint32_t   nTimeout            = 2000;             // Timeout for Grab
    VmbAccessMode_t     cameraAccessMode    = VmbAccessModeFull;// We open the camera with full access
    VmbHandle_t         cameraHandle        = NULL;             // A handle to our camera
    VmbBool_t           bIsCommandDone      = VmbBoolFalse;     // Has a command finished execution
    VmbFrame_t          frame;                                  // The frame we capture
    const char*         pPixelFormat        = NULL;             // The pixel format we use for acquisition
    const char*         exposureMode	    = NULL;
    const char*		acquisitionMode     = NULL;
    VmbInt64_t          nPayloadSize        = 0;                // The size of one frame

    unsigned char *img;//this is used to get image which will be used in read out
    VmbUint32_t sizeOfImg;
    const char* pCamera1ID;
    const char* pCamera2ID;
    const char* pCamera3ID;
    const char* pCamera4ID;




   //std::vector<std::map<std::string,std::pair<std::string,std::string>>> headerVector;
   //std::vector<std::pair<std::string,std::string>> dueTimerVector;
   std::map<std::string,std::pair<std::string,std::string>> headerVector;
   std::pair<std::string,std::string> dueTimerVector;



    rts2core::ValueString *camera1;
    rts2core::ValueString *camera2;
    rts2core::ValueString *camera3;
    rts2core::ValueString *camera4;





    typedef enum FrameInfos
    {
              FrameInfos_Off,
              FrameInfos_Show,
              FrameInfos_Automatic
    } FrameInfos;




};//end of class

};//end of namespace

using namespace rts2camd;

int Prosilica::setValue (rts2core::Value *old_value, rts2core::Value *new_value)
{

    return Camera::setValue (old_value, new_value);
}

int Prosilica::initHardware()
{

        err = VmbStartup();  //initialize vimba api

        if ( VmbErrorSuccess == err )
        {
            DiscoverGigECameras();
            err = VmbCamerasList( NULL, 0, &nCount, sizeof *pCameras );                         // Get the amount of known cameras
            if (    VmbErrorSuccess == err && nCount          != 0 )
            {
                logStream(MESSAGE_INFO)<< "Cameras found:"<<nCount<<sendLog;

                pCameras = (VmbCameraInfo_t*)malloc( sizeof *pCameras * nCount );
                if ( NULL != pCameras )
                {
                    err = VmbCamerasList( pCameras, nCount, &nFoundCount, sizeof *pCameras );   // Query all static details of all known cameras
                    // Without having to open the cameras
                    if(     VmbErrorSuccess == err
                            ||  VmbErrorMoreData == err )
                    {
                        if( nFoundCount < nCount)                                               // If a new camera was connected since we queried
                        {   // the amount of cameras, we can ignore that one
                            nCount = nFoundCount;
                        }
                        std::stringstream camStrings;

                        for ( i=0; i<nCount; ++i )                                              // And print them out
                        {
                            logStream(MESSAGE_INFO)<< "/// Camera Name:"<<pCameras[i].cameraName<<"/// Model Name:"<<pCameras[i].modelName<<"/// Camera ID:"<< pCameras[i].cameraIdString<<"/// Serial Number:"<<pCameras[i].serialString<<"/// @ Interface ID:"<<pCameras[i].interfaceIdString<<sendLog;

                            camStrings << pCameras[i].cameraName<<" "<<pCameras[i].modelName<<" "<<pCameras[i].cameraIdString<<" "<<pCameras[i].serialString<<" "<<pCameras[i].interfaceIdString;


                            if(i==0) {
                                camera1->setValueString(camStrings.str());
                                pCamera1ID=pCameras[i].cameraIdString;
                            }

                            if(i==1)camera2->setValueString(pCameras[i].cameraName);
                            if(i==2)camera3->setValueString(pCameras[i].cameraName);
                            if(i==3)camera4->setValueString(pCameras[i].cameraName);




                        }
                    }
                    else
                    {
                        logStream(MESSAGE_INFO)<<"Could not retrieve camera list. Error code:"<< err<<sendLog;
			return -1;
                    }
                    free( pCameras );
                    pCameras = NULL;
                }
                else
                {
                    logStream(MESSAGE_INFO)<< "Could not allocate camera list."<<sendLog;
		    return -1;
                }
            }

            else
            {
                logStream(MESSAGE_INFO)<<"Could not list cameras or no cameras present. Error code:"<<err<<sendLog;
		return -1;
            }

            VmbShutdown();                                                                      // Close Vimba
        }
        else
        {
            logStream(MESSAGE_INFO)<< "Could not start system. Error code:"<<err;
	    return -1;
        }


      return initChips();
}
int Prosilica::startExposure()
{

        err = VmbStartup();  //initialize vimba api
        DiscoverGigECameras();

        // Open camera
        //err = VmbCameraOpen( pCamera1ID, cameraAccessMode, &cameraHandle );
	err = VmbCameraOpen(  "DEV_000F310191B3", cameraAccessMode, &cameraHandle );
        if ( VmbErrorSuccess == err )
        {
            logStream(MESSAGE_INFO)<<"Camera ID:"<<pCamera1ID<<sendLog;




	    
            // Set the GeV packet size to the highest possible value
            // (In this example we do not test whether this cam actually is a GigE cam)
            if ( VmbErrorSuccess == VmbFeatureCommandRun( cameraHandle, "GVSPAdjustPacketSize" ))
            {
                do
                {
                    if ( VmbErrorSuccess != VmbFeatureCommandIsDone(cameraHandle,
                            "GVSPAdjustPacketSize",
                            &bIsCommandDone ))
                    {
                        break;
                    }
                } while ( VmbBoolFalse == bIsCommandDone );
            }
        
            if ( VmbErrorSuccess == err )
            {
                // Set pixel format. For the sake of simplicity we only support Mono and RGB in this example.
                err = VmbFeatureEnumSet( cameraHandle, "PixelFormat", "RGB8Packed" );
                if ( VmbErrorSuccess != err )
                {
                    // Fall back to Mono
                    err = VmbFeatureEnumSet( cameraHandle, "PixelFormat", "Mono12" );
                }


                // Read back pixel format
                VmbFeatureEnumGet( cameraHandle, "PixelFormat", &pPixelFormat );
                logStream (MESSAGE_INFO)<<"Pixel format is : "<<pPixelFormat <<sendLog;

                if ( VmbErrorSuccess == err )
                {
                    //get exposure information
		    if ( VmbErrorSuccess == VmbFeatureEnumGet ( cameraHandle , "ExposureMode", &exposureMode ))
		    {
			    logStream (MESSAGE_INFO)<<"Exposure mode is : "<<exposureMode <<sendLog;
		    }
		    else   logStream (MESSAGE_INFO)<<"Cannot get exposure mode "<<sendLog;

                    


		    
		    //set exposure time
		    err = VmbFeatureFloatSet(cameraHandle, "ExposureTimeAbs",  exposure->getValueDouble()*1000000 );
                    if(VmbErrorSuccess==err)
		    logStream(MESSAGE_INFO)<<"exposure time is set"<<sendLog;
		    else logStream(MESSAGE_INFO)<<" Can not set exposure time "<<err<<sendLog;


                     //get exposure time
		     double exposureTime;
		     err=VmbFeatureFloatGet(cameraHandle,"ExposureTimeAbs",&exposureTime);
		     if(VmbErrorSuccess==err)
		     logStream(MESSAGE_INFO)<<"exposure time is get"<<exposureTime<<sendLog;
		     else logStream(MESSAGE_INFO)<<" Can not get exposure time "<<err<<sendLog;

                    //get acquisition mode
		    if(VmbErrorSuccess == VmbFeatureEnumGet(cameraHandle,"AcquisitionMode",&acquisitionMode))
	             {
		       logStream (MESSAGE_INFO)<<"acquisiton mode is : "<<acquisitionMode <<sendLog;
		     }
		     else   logStream (MESSAGE_INFO)<<"Cannot get acquisition mode "<<sendLog;


                   //get device temperature
		   double deviceTemperature;
		   err= VmbFeatureFloatGet(cameraHandle,"DeviceTemperature",&deviceTemperature);
		   if(VmbErrorSuccess == err)
	                     {
		               logStream (MESSAGE_INFO)<<"Temperature is : "<<deviceTemperature <<sendLog;
		              }
                     else   logStream (MESSAGE_INFO)<<"Cannot get temperature "<<err <<sendLog;

		     
		     
		    // Evaluate frame size
                    err = VmbFeatureIntGet( cameraHandle, "PayloadSize", &nPayloadSize );
                    if ( VmbErrorSuccess == err )
                    {
                        frame.buffer        = (unsigned char*)malloc( (VmbUint32_t)nPayloadSize );
                        frame.bufferSize    = (VmbUint32_t)nPayloadSize;

                        // Announce Frame
                        err = VmbFrameAnnounce( cameraHandle, &frame, (VmbUint32_t)sizeof( VmbFrame_t ));
                        if ( VmbErrorSuccess == err )
                        {
                            // Start Capture Engine
                            err = VmbCaptureStart( cameraHandle );
                            if ( VmbErrorSuccess == err )
                            {
                                // Queue Frame
                                err = VmbCaptureFrameQueue( cameraHandle, &frame, NULL );
                                if ( VmbErrorSuccess == err )
                                {
                                    // Start Acquisition
                                    err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStart" );
                                    if ( VmbErrorSuccess == err )
                                    {
                                        // Capture one frame synchronously
                                        err = VmbCaptureFrameWait( cameraHandle, &frame, nTimeout );
                                        if ( VmbErrorSuccess == err )
                                        {
                                            // Convert the captured frame to a bitmap and save to disk
                                            if ( VmbFrameStatusComplete == frame.receiveStatus )
                                            {
                                                
					       
						
						img = (unsigned char*)malloc(frame.imageSize);
						memcpy(img,frame.buffer,frame.imageSize);				
						sizeOfImg=frame.imageSize;    
					
						
 						logStream(MESSAGE_INFO)<<"size of Img is "<<sizeOfImg<<" image value is "<<(int)img[0]<<" " <<(int)img[100]<<" frame width " <<frame.width<<" frame height "<<frame.height<<" pixel format is "<<pPixelFormat<<sendLog;

						/*
                                                bitmap.bufferSize = frame.imageSize;
                                                                                          bitmap.width = frame.width;
                                                                                          bitmap.height = frame.height;
                                                                                          // We only support Mono and RGB in this example
                                                                                          if ( 0 == strcmp( "RGB8Packed", pPixelFormat ))
                                                                                          {
                                                                                              bitmap.colorCode = ColorCodeRGB24;
                                                                                          }
                                                                                          else
                                                                                          {
                                                                                              bitmap.colorCode = ColorCodeMono8;
                                                                                          }

                                                                                          // Create the bitmap
                                                                                          if ( 0 == AVTCreateBitmap( &bitmap, frame.buffer ))
                                                                                          {
                                                                                              printf( "Could not create bitmap.\n" );
                                                                                          }
                                                                                          else
                                                                                          {
                                                                                              // Save the bitmap
                                                                                              if ( 0 == AVTWriteBitmapToFile( &bitmap, pFileName ))
                                                                                              {
                                                                                                  printf( "Could not write bitmap to file.\n" );
                                                                                              }
                                                                                              else
                                                                                              {
                                                                                                  printf( "Bitmap successfully written to file \"%s\"\n", pFileName );
                                                                                                  // Release the bitmap's buffer
                                                                                                  if ( 0 == AVTReleaseBitmap( &bitmap ))
                                                                                                  {
                                                                                                      printf( "Could not release the bitmap.\n" );
                                                                                                  }
                                                                                                  else
                                                                                                  {
                                                                                                      // Save the bitmap
                                                                                                      if ( 0 == AVTWriteBitmapToFile( &bitmap, pFileName ))
                                                                                                      {
                                                                                                          printf( "Could not write bitmap to file.\n" );
                                                                                                      }
                                                                                                      else
                                                                                                      {
                                                                                                          printf( "Bitmap successfully written to file \"%s\"\n", pFileName );
                                                                                                          // Release the bitmap's buffer
                                                                                                          if ( 0 == AVTReleaseBitmap( &bitmap ))
                                                                                                          {
                                                                                                              printf( "Could not release the bitmap.\n" );
                                                                                                          }
                                                                                                      }
                                                                                                  }

                                                				     	}

                                                				    else
                                                                                              {
                                                                                                  printf( "Frame not successfully received. Error code: %d\n", frame.receiveStatus );
                                                                                              }
                                                                                          }
                                                                                          else
                                                                                          {
                                                                                              printf( "Could not capture frame. Error code: %d\n", err );
                                                                                          }
                                                				*/
                                                // Stop Acquisition
                                                err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStop" );
                                                if ( VmbErrorSuccess != err )
                                                {
                                                    logStream(MESSAGE_INFO)<< "Could not stop acquisition. Error code:"<<err<<sendLog;
                                                    return -1;
						}
                                            }
                                            else
                                            {
                                                logStream(MESSAGE_INFO)<< "Could not start acquisition. Error code:"<<err<<sendLog;
                                                return -1;
					    }
                                        }
                                        else
                                        {
                                            logStream(MESSAGE_INFO)<< "Could not queue frame. Error code:"<<err<<sendLog ;
					    return -1;
                                        }

                                        // Stop Capture Engine
                                        err = VmbCaptureEnd( cameraHandle );
                                        if ( VmbErrorSuccess != err )
                                        {
                                            logStream(MESSAGE_INFO)<<"Could not end capture. Error code:"<<err<<sendLog;
					    return -1;
                                        }
                                    }
                                    else
                                    {
                                        logStream(MESSAGE_INFO)<< "Could not start capture. Error code:"<<err<<sendLog;
					return -1;
                                    }

                                    // Revoke frame
                                    err = VmbFrameRevoke( cameraHandle, &frame );
                                    if ( VmbErrorSuccess != err )
                                    {
                                        logStream(MESSAGE_INFO)<<"Could not revoke frame. Error code: "<<err<<sendLog;
					return -1;
                                    }
                                }
                                else
                                {
                                    logStream(MESSAGE_INFO)<<"Could not announce frame. Error code:"<<err <<sendLog;
				    return -1;
                                }

                                free( frame.buffer );
                                frame.buffer = NULL;
                            }
                        }
                        else
                        {
                            logStream(MESSAGE_INFO)<< "Could not set pixel format to either RGB or Mono. Error code:"<<err<<sendLog;
			    return -1;
                        }
                    }
                    else
                    {
                        logStream(MESSAGE_INFO)<< "Could not adjust packet size. Error code:"<<err<<sendLog;
			return -1;
                    }

                    err = VmbCameraClose ( cameraHandle );
                    if ( VmbErrorSuccess != err )
                    {
                        logStream(MESSAGE_INFO) <<"Could not close camera. Error code:"<<err<<sendLog;
			return -1;
                    }
                }
                else
                {
                    logStream(MESSAGE_INFO)<< "Could not open camera. Error code:"<<err<<sendLog;
		    return -1;
                }
            }
            VmbShutdown();
        }
        else
        {
            logStream(MESSAGE_INFO)<< "Could not start system. Error code:"<<err<<sendLog;
	    return -1;
        }


        return 0;
    }
int Prosilica::doReadout ()
{
    logStream(MESSAGE_INFO)<<"readout basladi"<<sendLog;

 
    int ret = sendReadoutData((char*)img,15824256);//sizeOfImg);
    if(ret<0)
    {
     logStream(MESSAGE_INFO)<<"send sendReadoutData yapilamadi"<<sendLog;
     return -1;
    }

     logStream(MESSAGE_INFO)<<"readout bitti"<<sendLog;

    
    return -2;					 // imediately send new data
}


int Prosilica::stopExposure ()
{
	 logStream(MESSAGE_INFO)<<"stop exposure calisti"<<sendLog;
	        return Camera::stopExposure ();

}


int Prosilica::commandAuthorized(rts2core::Connection * conn)
{

if(conn->isCommand("single"))
{
startExposure();
createFits((unsigned short *)img,headerVector,dueTimerVector);


}

if(conn->isCommand("fast"))
{


}

return Camera::commandAuthorized(conn);

}


	
int main (int argc, char **argv)
{
    Prosilica device (argc, argv);
    return device.run ();

}
