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

#include "VimbaCPP/Include/VimbaCPP.h"
#include "ApiController.h"

#include"fliKepler/libflipro.h"


namespace rts2camd
{


class Prosilica:public Camera
{
public:



    Prosilica (int in_argc, char **in_argv):Camera (in_argc, in_argv)
    {

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
        err = pFrame->GetReceiveStatus( status );

        if (status < 0)
        {
            logStream (MESSAGE_ERROR) << "During check for ImagingStatus, return < 0: " << status << sendLog;
            return -2;
        }


        if (VmbErrorSuccess == err && VmbFrameStatusComplete != status )
        {
            logStream (MESSAGE_ERROR) << "Exposing " << status << sendLog;

            return 200;
        }

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
    AVT::VmbAPI::Examples::ApiController apiController;
    AVT::VmbAPI::Examples::ProgramConfig Config;
    AVT::VmbAPI::FramePtr pFrame;
    std::string strCameraID;
    VmbErrorType err;
    VmbUchar_t *pImage = NULL;
    VmbUint32_t nImageSize = 0;
    VmbFrameStatusType status = VmbFrameStatusIncomplete;
    unsigned char *img;//this is used to get image which will be used in read out

//std::map<std::string,std::pair<std::string,std::string>> headerVector;
    std::vector<std::map<std::string,std::pair<std::string,std::string>>> headerVector;

    std::pair<std::string,std::string> dueTimerVector;
    std::map<std::string,std::pair<std::string,std::string>> header;



};//end of class

};//end of namespace

using namespace rts2camd;

int Prosilica::setValue (rts2core::Value *old_value, rts2core::Value *new_value)
{

    return Camera::setValue (old_value, new_value);
}

int Prosilica::initHardware()
{



    // Print out version of Vimba
    logStream(MESSAGE_INFO)<<"Vimba C++ API Version "<<apiController.GetVersion()<<sendLog;

    // Startup Vimba
    err = apiController.StartUp();

    if ( VmbErrorSuccess == err )
    {
        if( Config.getCameraID().empty() )
        {
            AVT::VmbAPI::CameraPtrVector cameras = apiController.GetCameraList();
            if( cameras.empty() )
            {
                err = VmbErrorNotFound;
            }
            else
            {
                // std::string strCameraID;
                err = cameras[0]->GetID( strCameraID );
                if( VmbErrorSuccess == err )
                {
                    Config.setCameraID( strCameraID );
                }
            }

        }


    }

    apiController.ShutDown();


    logStream(MESSAGE_INFO)<<"inithardware bitti"<<sendLog;

    return initChips();
}
int Prosilica::startExposure()
{   apiController.ShutDown();

    err = apiController.StartUp();
    if ( VmbErrorSuccess == err )
    {
        err = apiController.AcquireSingleImage( strCameraID, pFrame,exposure->getValueDouble()*1000000 );
        if ( VmbErrorSuccess == err )
        {
            logStream(MESSAGE_INFO)<<"captured single image"<<sendLog;

        }
        else
            logStream(MESSAGE_INFO)<<"Cannot acquire single image error is "<<err<<sendLog;
    }

    return 0;
}
int Prosilica::doReadout ()
{
    logStream(MESSAGE_INFO)<<"readout basladi"<<sendLog;
    err = pFrame->GetImage(pImage);

    if(VmbErrorSuccess !=err)
    {
        return -1;
    }

    pFrame->GetImageSize( nImageSize );

    uint16_t *tes = new uint16_t[nImageSize];
    //     unsigned char *buff = new unsigned char [nImageSize*2];
    for(int k = 0; k<=nImageSize; k++)
    {
        tes[k]=0x0000;
        tes[k] = (uint16_t)pImage[k];
        tes[k] = tes[k] <<8;

    }

    logStream (MESSAGE_INFO) << " Data type before sendreadoutdaata is " << getDataType()<<sendLog;
    int ret =  sendReadoutData((char*)tes,getWriteBinaryDataSize());
    //int ret = sendReadoutData((char*)img,15824256);//sizeOfImg);
    if(ret<0)
    {
        logStream(MESSAGE_INFO)<<"send sendReadoutData yapilamadi"<<sendLog;
        return -1;
    }

    logStream(MESSAGE_INFO)<<"readout bitti"<<sendLog;



    return -2;	 // imediately send new data
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


    }

    if(conn->isCommand("fast"))
    {

        if(getState () == BOP_EXPOSURE)
            return 0;

        logStream(MESSAGE_INFO)<<"Beginning of fast  "<<sendLog;
        double exptime;
        long numberOfExposures;
        if (conn->paramNextDouble (&exptime) || conn->paramNextLong (&numberOfExposures) || !conn->paramEnd ())
            return -2;

        blockExposure();//exposure should be disabled

        logStream(MESSAGE_INFO)<<"After Block exposure  "<<sendLog;



        // Print out version of Vimba
        logStream(MESSAGE_INFO)<<"Vimba C++ API Version "<<apiController.GetVersion()<<sendLog;
        apiController.ShutDown();
        // Startup Vimba


        VmbErrorType err = apiController.StartUp();
        if ( VmbErrorSuccess == err )
        {
            if( Config.getCameraID().empty() )
            {
                AVT::VmbAPI::CameraPtrVector cameras = apiController.GetCameraList();
                if( cameras.empty() )
                {
                    err = VmbErrorNotFound;
                }
                else
                {
                    std::string strCameraID;
                    err = cameras[0]->GetID( strCameraID );
                    if( VmbErrorSuccess == err )
                    {
                        Config.setCameraID( strCameraID );
                    }
                }
            }
            if ( VmbErrorSuccess == err )
            {
                logStream(MESSAGE_INFO)<<"Opening camera with ID: "<<Config.getCameraID()<<sendLog;

                header["DATE-OBS"] = std::make_pair("date","pc date");
                //headerVector.push_back(header);
                err = apiController.StartContinuousImageAcquisition( Config,exptime*1000000,header,numberOfExposures );

                if ( VmbErrorSuccess == err )
                {
                    logStream(MESSAGE_INFO)<< "started continuous image acquisiton" ;

                }

                else
		{
		    std::string strError = apiController.ErrorCodeToMessage( err );
		    logStream(MESSAGE_INFO)<<"An error occurred: " << strError <<sendLog;
		}

            }
            clearExposure();

        }


    }

    return Camera::commandAuthorized(conn);

}



int main (int argc, char **argv)
{
    Prosilica device (argc, argv);
    return device.run ();

}
