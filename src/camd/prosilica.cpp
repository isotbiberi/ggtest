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
#include <ApiController.h>
#include <Bitmap.h>

/*
#define OPT_WIDTH        OPT_LOCAL + 1
#define OPT_HEIGHT       OPT_LOCAL + 2
#define OPT_DATA_SIZE    OPT_LOCAL + 3
#define OPT_CHANNELS     OPT_LOCAL + 4
#define OPT_REMOVE_TEMP  OPT_LOCAL + 5
#define OPT_INFOSLEEP    OPT_LOCAL + 6
#define OPT_READSLEEP    OPT_LOCAL + 7
#define OPT_FRAMETRANS   OPT_LOCAL + 8

*/
namespace rts2camd
{

/**
 * Class for a dummy camera.
 *
 * @author Petr Kubanek <petr@kubanek.net>
 */
class Prosilica:public Camera
{
	public:
		Prosilica (int in_argc, char **in_argv):Camera (in_argc, in_argv)
		{
			
                     /********prosilica******/

    bool            bPrintHelp  = false;            // Output help?
    int             i;                              // Counter for some iteration
    char *          pParameter;                     // The command line parameter
//        logStream (MESSAGE_INFO) << " camera constructor Data type is  " << getDataType()<<sendLog;


                   /*******prosilicai*****************/



                 }

		virtual ~Prosilica (void)
		{
			//readoutSleep = NULL;
		      apiController.ShutDown(); 	
                      delete[] written;
		}

		virtual int processOption (int in_opt)
		{
			switch (in_opt)
			{

				default:
					return Camera::processOption (in_opt);
			}
			return 0;
		}


		virtual int initHardware ()
		{
       //AVT::VmbAPI::Examples::ApiController apiController;
       std::cout << "Vimba Version V" << apiController.GetVersion() << "\n";
     // VmbFrameStatusType status = VmbFrameStatusIncomplete;
      // VmbErrorType    err         = VmbErrorSuccess;
      // char *          pCameraID   = NULL;  
       err = apiController.StartUp();
       std::cout<<" init hardware started up api error value is " <<err<<" error success is " <<VmbErrorSuccess<<std::endl;
       //std::cout << "inithardware getdatatype is  " << getDataType() <<std::endl;
      
        if ( VmbErrorSuccess != err )
        {       
            std::string strError = apiController.ErrorCodeToMessage( err );
            std::cout << "\nAn error occurred: " << strError.c_str() << "\n";

         }
            
       
        if ( VmbErrorSuccess == err )
        {
            
             //std::string strCameraID;
            std::cout << "pCamera oncesi" <<std::endl;
            if ( NULL == pCameraID )
            {
                AVT::VmbAPI::CameraPtrVector cameras = apiController.GetCameraList();
           std::cout<<"getcameralist sonrası " <<std::endl;
                if ( cameras.size() <= 0 )
                {
                    err = VmbErrorNotFound;
                }
                else
                {
                    err = cameras[0]->GetID( strCameraID );
                }
            }
            else
            {
                strCameraID = pCameraID;
            }

            if ( VmbErrorSuccess == err )
            {
                std::cout << "Camera ID:" << strCameraID.c_str() << "\n\n";
            }
                if ( VmbErrorSuccess != err )
        {
            std::string strError = apiController.ErrorCodeToMessage( err );
            std::cout << "\nAn error occurred: " << strError.c_str() << "\n";

         }

               }
                       apiController.ShutDown();
                        return initChips ();
		}
		virtual int initChips ()
		{
			
	 
             // std::cout<< "init chips getDataYpe is  " << getDataType() << std::endl;

//	setSize (3000, 2600, 0, 0);      
	
              setSize (4872,3248, 0, 0);


	
                 return Camera::initChips ();
		}
		virtual int info ()
		{
			return Camera::info ();
		}
		virtual int startExposure ()
		{

                         logStream (MESSAGE_ERROR) << " startExposure getDataType is " << getDataType() << sendLog;
 
                         apiController.StartUp();
                         err = apiController.AcquireSingleImage( strCameraID, pFrame );

/*

                   if ( VmbErrorSuccess == err )
                {
                    logStream (MESSAGE_ERROR) << "Exposure is started status is " << status << sendLog;

                    err = pFrame->GetReceiveStatus( status );
                    if (    VmbErrorSuccess == err
                         && VmbFrameStatusComplete == status )
                    {
                        VmbPixelFormatType ePixelFormat = VmbPixelFormatMono8;
                        err = pFrame->GetPixelFormat( ePixelFormat );
                        if ( VmbErrorSuccess == err )
                        {
                            if (    ( VmbPixelFormatMono8 != ePixelFormat )
                                &&  ( VmbPixelFormatRgb8 != ePixelFormat ))
                            {
                                err = VmbErrorInvalidValue;
                            }
                            else
                            {
                                VmbUint32_t nImageSize = 0;
                                err = pFrame->GetImageSize( nImageSize );
                                if ( VmbErrorSuccess == err )
                                {
                                    //VmbUint32_t nWidth = 0;
                                    err = pFrame->GetWidth( nWidth );
                                    if ( VmbErrorSuccess == err )
                                    {
                                       // VmbUint32_t nHeight = 0;
                                        err = pFrame->GetHeight( nHeight );
                                        if ( VmbErrorSuccess == err )
                                        {
                                            //VmbUchar_t *pImage = NULL;
                                            err = pFrame->GetImage( pImage );
                                            if ( VmbErrorSuccess == err )
                                            {
                                                AVTBitmap bitmap;

                                                if ( VmbPixelFormatRgb8 == ePixelFormat )
                                                {
                                                    bitmap.colorCode = ColorCodeRGB24;
                                                }
                                                else
                                                {
                                                    bitmap.colorCode = ColorCodeMono8;
                                                }

                                                bitmap.bufferSize = nImageSize;
                                                bitmap.width = nWidth;
                                                bitmap.height = nHeight;

                                                // Create the bitmap
                                                if ( 0 == AVTCreateBitmap( &bitmap, pImage ))
                                                {
                                                    std::cout << "Could not create bitmap.\n";
                                                    err = VmbErrorResources;
                                                }
                                                else
                                                {
                                                    // Save the bitmap
                                                    if ( 0 == AVTWriteBitmapToFile( &bitmap, pFileName ))

                                                      {
                                                        std::cout << "Could not write bitmap to file.\n";
                                                        err = VmbErrorOther;
                                                    }
                                                    else
                                                    {
                                                        std::cout << "Bitmap successfully written to file \"" << pFileName << "\"\n" ;
                                                        // Release the bitmap's buffer
                                                        if ( 0 == AVTReleaseBitmap( &bitmap ))
                                                        {
                                                            std::cout << "Could not release the bitmap.\n";
                                                            err = VmbErrorInternalFault;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                             }
                           }
                         }
                      }




*/

/*



                if ( VmbErrorSuccess == err )
                {
                     logStream (MESSAGE_ERROR) << "Exposure is started status is " << status << sendLog;
  
return 0;
                }
  */      

/* 
	else{
		
                logStream (MESSAGE_ERROR) << "Cannot expose " << status << sendLog;
		return -1;
	   }   

  */          


                       
                       // apiController.ShutDown();
			return 0;//callReadout->getValueBool () ? 0 : 1;
		}

		virtual long isExposing ()
		{
			//long ret = Camera::isExposing ();
			//if (ret == -2)
			//	return (callReadout->getValueBool () == true) ? -2 : -4;
		        //  return -2;	
                        //return ret;
                long ret;
               //Apn_Status status;
               

               err = pFrame->GetReceiveStatus( status );


/*
               if (    VmbErrorSuccess == err
                       && VmbFrameStatusComplete == status )


*/

  //logStream (MESSAGE_ERROR) << "is exposing started status is  " << status << sendLog;


               ret = Camera::isExposing ();
               if (ret > 0)
               return ret;

               //status = alta->read_ImagingStatus ();
               err = pFrame->GetReceiveStatus( status );
            
               if (status < 0)
              {
                logStream (MESSAGE_ERROR) << "During check for ImagingStatus, return < 0: " << status << sendLog;
                return -2;
              }
            
              // if (status != Apn_Status_ImageReady)
           //logStream (MESSAGE_ERROR) << "after get receive status, status is  " << status << sendLog;

             if (    VmbErrorSuccess == err
                       && VmbFrameStatusComplete != status )  { //logStream (MESSAGE_ERROR) << "During check for ImagingStatus, return < 0: " << status << sendLog;

                return 200;}
        // exposure has ended..
                  //logStream (MESSAGE_ERROR) << "before end of is exposing status is  " << status << sendLog;

               return -2;

     
		}
/*
		virtual size_t suggestBufferSize ()
		{
			if (dataSize < 0)
				return Camera::suggestBufferSize ();
			return dataSize;

		}
*/
		virtual int doReadout ();

		virtual bool supportFrameTransfer () { return supportFrameT; }
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

		virtual void initDataTypes ()
		{
			Camera::initDataTypes ();
			addDataType (RTS2_DATA_BYTE);
	//		addDataType (RTS2_DATA_SHORT);
		//	addDataType (RTS2_DATA_LONG);
	       //		addDataType (RTS2_DATA_LONGLONG);
		//	addDataType (RTS2_DATA_FLOAT);
		//	addDataType (RTS2_DATA_DOUBLE);
		//	addDataType (RTS2_DATA_SBYTE);
		//	addDataType (RTS2_DATA_ULONG);

			setDataTypeWritable ();
		}

		virtual int setValue (rts2core::Value *old_value, rts2core::Value *new_value);

		virtual int shiftStoreStart (rts2core::Connection *conn, float exptime);

		virtual int shiftStoreShift (rts2core::Connection *conn, int shift, float exptime);

		virtual int shiftStoreEnd (rts2core::Connection *conn, int shift, float exptime);

	private:
	      VmbFrameStatusType status = VmbFrameStatusIncomplete;
              VmbErrorType    err         = VmbErrorSuccess;
              char *          pCameraID   = NULL;   
              AVT::VmbAPI::Examples::ApiController apiController;
              AVT::VmbAPI::FramePtr pFrame;
              std::string strCameraID;
              const char *    pFileName   = "SynchronousGrab2.bmp";       
              VmbUint32_t nWidth = 0;
              VmbUint32_t nHeight = 0;
       
 	       VmbUchar_t *pImage = NULL;
               bool supportFrameT;
		int infoSleep;
		int width;
		int height;

		//long dataSize;

		bool showTemp;

//		void generateImage (size_t pixelsize, int chan);

//		template <typename dt> void generateData (dt *data, size_t pixelsize);

		// data written during readout
		ssize_t *written;
};

};

using namespace rts2camd;

int Prosilica::setValue (rts2core::Value *old_value, rts2core::Value *new_value)
{
	
	return Camera::setValue (old_value, new_value);
}

int Prosilica::shiftStoreStart (rts2core::Connection *conn, float exptime)
{
	return Camera::shiftStoreStart (conn, exptime);
}

int Prosilica::shiftStoreShift (rts2core::Connection *conn, int shift, float exptime)
{
	return Camera::shiftStoreShift (conn, shift, exptime);
}

int Prosilica::shiftStoreEnd (rts2core::Connection *conn, int shift, float exptime)
{
	return Camera::shiftStoreEnd (conn, shift, exptime);
}

int Prosilica::doReadout ()
{
               //logStream (MESSAGE_ERROR) << "started do Readout status is " << status << sendLog;

       //getDataBuffer(0);
       int ret;
 
      err = pFrame->GetReceiveStatus( status );
      //logStream (MESSAGE_ERROR) << "started do Readout pframe status is " << status << sendLog;
      //char *buffffer=getDataBuffer(0);
      // logStream (MESSAGE_ERROR) << "address of bufferr is " << (int*)&buffffer[0] << sendLog;
     // pImage=(unsigned char *)buffffer;
      
     //  logStream (MESSAGE_ERROR) << "address of pImage before getImage is " << (int*)&pImage[0] << sendLog;
     // char *buff = new char[31648512] ;
       // logStream (MESSAGE_ERROR) << "sizeof buff is after creation "<<strlen(buff)<<sendLog;

              if (    VmbErrorSuccess == err
                         && VmbFrameStatusComplete == status )
                    {
                     // pImage = (unsigned char*)getDataBuffer(0);
                     // err = pFrame->GetImage(pImage);
                      err = pFrame->GetImage(pImage);
   
                    }
                    if(VmbErrorSuccess !=err)
			{
                        //apiController.ShutDown();
			return -1;
			}

                 //for(int imgcounter=0;imgcounter<15000;imgcounter++)
		//std::cout<<"image is "<<pImage[1000000]<<std::endl;  
            //      logStream (MESSAGE_ERROR) << "SendReadOut yapiliyor binary data size is " <<getWriteBinaryDataSize()<< sendLog;
  //logStream (MESSAGE_ERROR) << "address of pImage after GetImage is  "<<(int*)&pImage[0] <<" value of pimage is "<< pImage[150]<<" address of buffer is "<<(int*)&buffffer[0]<<" value of buffer is "<<buffffer[150]<<" getwritebinarydatasize is " <<getWriteBinaryDataSize()<< sendLog;
             
//logStream (MESSAGE_ERROR) << "pImage is data value of 1000 is " <<pImage[100]<<" integer value is "<<(short)pImage[1000]  << " size is " <<sizeof(pImage)<<" char value is "<< (char)pImage[1000]  << sendLog;
              //    char *dest_top=getDataBuffer(0);
                //  dest_top = (char *) pImage;
                //char *testtir = getDataBuffer(0);
    //            logStream (MESSAGE_ERROR) << "address of testtir is "<<(int*)&testtir[0]<<"dest_top is "<<pImage[1000]<<" getdatabuffer 1000 is " <<testtir[1000]<<sendLog;
               // getDataBuffer(0) = (char*)pImage;
               //buff = (char*)pImage;
              // strncopy(buff,pImage);
              //memcpy(buff, pImage, 3000000);
               // logStream (MESSAGE_ERROR) << "sizeof buff is "<<strlen(buff)<<" size buff 0 is " <<(int)buff[0]<<" pImage 0 is " <<(int)pImage[0]<<sendLog;
               //ret =  sendReadoutData(buff,getWriteBinaryDataSize(),0);
                unsigned   int   nImageSize;
                pFrame->GetImageSize( nImageSize );
                  //logStream (MESSAGE_ERROR) << "Size of pImage " << strlen((char*)pImage) << " vimba ssisze is " << nImageSize <<" getwritebinarydatasize is " << getWriteBinaryDataSize()<<sendLog;
                 
		uint16_t *tes = new uint16_t[nImageSize];
            //     unsigned char *buff = new unsigned char [nImageSize*2];
                 for(int k = 0;k<=nImageSize;k++)
               {
                        tes[k]=0x0000;
   			tes[k] = (uint16_t)pImage[k];
                        tes[k] = tes[k] <<8;   			
    
               // tes[k] = (char)pImage[k];
                
                } 
                 //char *buff = reinterpret_cast<char *>(pImage);

              //    memcpy(buff,pImage,nImageSize);
              
                 // std::string str( u_array, u_array + sizeof u_array / sizeof u_array[0] );
    		 // std::cout << str << std::endl;
                //  std::string str(reinterpret_cast<char*>(pImage), nImageSize);

                // logStream (MESSAGE_ERROR) << "Size of buff is  " <<sizeof(buff)<<sendLog;
                 logStream (MESSAGE_INFO) << " Data type before sendreadoutdaata is " << getDataType()<<sendLog;
                 ret =  sendReadoutData((char*)tes,getWriteBinaryDataSize());
                   
           //     ret =  sendReadoutData((char*)buff,15824256);


              //logStream (MESSAGE_ERROR) << "Size of pImage " << strlen((char*)pImage) << sendLog;
                  
                 
		if(ret<0)
		{
		
		logStream (MESSAGE_ERROR) << "Send read out data yapilamadi " << status << sendLog;
               //apiController.ShutDown(); 
               return -1;
		}
       // apiController.ShutDown();
          //logStream (MESSAGE_ERROR) << "SendReadOut yapildi " << sendLog;

	return -2;					 // imediately send new data
}
/*
void Prosilica::generateImage (size_t pixelsize, int chan)
{

}

template <typename dt> void Prosilica::generateData (dt *data, size_t pixelSize)
{
}
*/
int main (int argc, char **argv)
{
	Prosilica device (argc, argv);
	return device.run ();
/*
 AVT::VmbAPI::Examples::ApiController apiController;
        std::cout << "Vimbanin Version V" << apiController.GetVersion() << "\n";
       VmbFrameStatusType status = VmbFrameStatusIncomplete;
         VmbErrorType    err         = VmbErrorSuccess;
         char *          pCameraID   = NULL;
       err = apiController.StartUp();
       std::cout<<"error value is " <<err<<" errorsuccess is " <<VmbErrorSuccess<<std::endl;
 if ( VmbErrorSuccess != err )
        {
         std::string strError = apiController.ErrorCodeToMessage( err );
         std::cout << "\nAn error occurred: " << strError.c_str() << "\n";
	}
 

apiController.ShutDown();

*/

}
