/*=============================================================================
  Copyright (C) 2013 - 2017 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        FrameObserver.cpp

  Description: The frame observer that is used for notifications from VimbaCPP
               regarding the arrival of a newly acquired frame.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <iostream>
#include <iomanip>
#ifdef WIN32
#include <Windows.h>
#else
#include <time.h>
#endif //WIN32

#include "FrameObserver.h"

#include "TransformImage.h"

#include "app.h"
#include"zyla/FitsOperations.h"




namespace AVT {
namespace VmbAPI {
namespace Examples {

//
// We pass the camera that will deliver the frames to the constructor
//
// Parameters:
//  [in]    pCamera             The camera the frame was queued at
//  [in]    eFrameInfos         Indicates how the frame will be displayed
//  [in]    eColorProcessing    Indicates how color processing is applied
//
FrameObserver::FrameObserver( CameraPtr pCamera, FrameInfos eFrameInfos, ColorProcessing eColorProcessing, bool bRGBValue,std::map<std::string,std::pair<std::string, std::string>> header,long numberOfExposures)
    :   IFrameObserver( pCamera )
    ,   m_eFrameInfos( eFrameInfos )
    ,   m_bRGB( bRGBValue )
    ,   m_eColorProcessing( eColorProcessing )
#ifdef WIN32
    ,   m_dFrequency( 0.0 )
#endif //WIN32
{
#ifdef WIN32
    LARGE_INTEGER nFrequency;
    QueryPerformanceFrequency( &nFrequency );
    m_dFrequency = (double)nFrequency.QuadPart;
#endif //WIN32
m_header=header;
m_numberOfExposures=numberOfExposures;
}

void FrameObserver::getHeader()
{


}



//
// Gets the current timestamp for interval measurement
//
double FrameObserver::GetTime()
{
    double dTime = 0.0;

#ifdef WIN32
    LARGE_INTEGER nCounter;
    QueryPerformanceCounter( &nCounter );
    dTime = ( (double)nCounter.QuadPart ) / m_dFrequency;
#else
    //clock_t nTime = times(NULL);
    //dTime = ((double)(nTime) * 10000.0) / ((double)CLOCKS_PER_SEC);
    struct timespec now;
    clock_gettime( CLOCK_REALTIME, &now );
    dTime = ( (double)now.tv_sec ) + ( (double)now.tv_nsec ) / 1000000000.0;
#endif //WIN32

    return dTime;
}

//
// Prints out frame parameters such as 
// - width
// - height
// - pixel format
//
// Parameters:
//  [in]    pFrame          The frame to work on
//
void PrintFrameInfo( const FramePtr &pFrame )
{
    std::cout<<" Size:";
    VmbUint32_t     nWidth = 0;
    VmbErrorType    res;
    res = pFrame->GetWidth(nWidth);
    if( VmbErrorSuccess == res )
    {
        std::cout<<nWidth;
    }
    else
    {
        std::cout<<"?";
    }

    std::cout<<"x";
    VmbUint32_t nHeight = 0;
    res = pFrame->GetHeight(nHeight);
    if( VmbErrorSuccess == res )
    {
        std::cout<< nHeight;
    }
    else
    {
        std::cout<<"?";
    }

    std::cout<<" Format:";
    VmbPixelFormatType ePixelFormat = VmbPixelFormatMono8;
    res = pFrame->GetPixelFormat( ePixelFormat );
    if( VmbErrorSuccess == res )
    {
        std::cout<<"0x"<<std::hex<<ePixelFormat<<std::dec;
    }
    else
    {
        std::cout<<"?";
    }
}

//
// Prints out frame status codes as readable status messages
//
// Parameters:
//  [in]    eFrameStatus    The error code to be converted and printed out
//
void PrintFrameStatus( VmbFrameStatusType eFrameStatus )
{
    switch( eFrameStatus )
    {
    case VmbFrameStatusComplete:
        std::cout<<"Complete";
        break;

    case VmbFrameStatusIncomplete:
        std::cout<<"Incomplete";
        break;

    case VmbFrameStatusTooSmall:
        std::cout<<"Too small";
        break;

    case VmbFrameStatusInvalid:
        std::cout<<"Invalid";
        break;

    default:
        std::cout<<"unknown frame status";
        break;
    }
}

//
// Prints out details of a frame such as
// - ID
// - receive status
// - width, height, pixel format
// - current frames per second
//
// Parameters:
//  [in]    pFrame          The frame to work on
//
void FrameObserver::ShowFrameInfos( const FramePtr &pFrame ) 
{
    bool                bShowFrameInfos     = false;
    VmbUint64_t         nFrameID            = 0;
    bool                bFrameIDValid       = false;
    VmbFrameStatusType  eFrameStatus        = VmbFrameStatusComplete;
    bool                bFrameStatusValid   = false;
    VmbErrorType        res                 = VmbErrorSuccess;
    double              dFPS                = 0.0;
    bool                bFPSValid           = false;
    VmbUint64_t         nFramesMissing      = 0;
    if( FrameInfos_Show == m_eFrameInfos )
    {
        bShowFrameInfos = true;
    }

    res = pFrame->GetFrameID( nFrameID );
    if( VmbErrorSuccess == res )
    {
        bFrameIDValid = true;

        if( m_FrameID.IsValid() )
        {
            if( nFrameID != ( m_FrameID() + 1 ) )
            {
                nFramesMissing = nFrameID - m_FrameID() - 1;
                if( 1 == nFramesMissing )
                {
                    std::cout<<"1 missing frame detected\n";
                }
                else
                {
                    std::cout<<nFramesMissing<<"missing frames detected\n";
                }
            }
        }

        m_FrameID( nFrameID );
        double dFrameTime = GetTime();
        if(     ( m_FrameTime.IsValid() )
            &&  ( 0 == nFramesMissing ) )
        {
            double dTimeDiff = dFrameTime - m_FrameTime();
            if( dTimeDiff > 0.0 )
            {
                dFPS = 1.0 / dTimeDiff;
                bFPSValid = true;
            }
            else
            {
                bShowFrameInfos = true;
            }
        }

        m_FrameTime( dFrameTime );
    }
    else
    {
        bShowFrameInfos = true;
        m_FrameID.Invalidate();
        m_FrameTime.Invalidate();
    }

    res = pFrame->GetReceiveStatus( eFrameStatus );
    if( VmbErrorSuccess == res )
    {
        bFrameStatusValid = true;

        if( VmbFrameStatusComplete != eFrameStatus )
        {
            bShowFrameInfos = true;
        }
    }
    else
    {
        bShowFrameInfos = true;
    }
    if( bShowFrameInfos )
    {
        std::cout<<"Frame ID:";
        if( bFrameIDValid )
        {
            std::cout<<nFrameID;
        }
        else
        {
            std::cout<<"?";
        }

        std::cout<<" Status:";
        if( bFrameStatusValid )
        {
            PrintFrameStatus( eFrameStatus);
        }
        else
        {
            std::cout<<"?";
        }
        PrintFrameInfo( pFrame );
        
        std::cout<<" FPS:";
        if( bFPSValid )
        {
            std::streamsize s = std::cout.precision();
            std::cout<<std::fixed<<std::setprecision(2)<<dFPS<<std::setprecision(s);
        }
        else
        {
            std::cout<<"?";
        }

        std::cout<<"\n";
    }
    else
    {
        std::cout<<".";
    }
}



//
// This is our callback routine that will be executed on every received frame.
// Triggered by the API.
//
// Parameters:
//  [in]    pFrame          The frame returned from the API
//
void FrameObserver::FrameReceived( const FramePtr pFrame )
{
    VmbErrorType res;
    
    logStream(MESSAGE_INFO)<<"Frame received"<<sendLog;
    VmbUint64_t         nFrameID            = 0;

     res = pFrame->GetFrameID( nFrameID );
     if( VmbErrorSuccess == res )
     {

     logStream(MESSAGE_INFO)<<"frame id is "<<nFrameID<<sendLog;

     }

     else logStream(MESSAGE_INFO)<<"Can not get frame id"<<sendLog;

/*
     if(nFrameID==m_numberOfExposures)
     {

       //Stop streaming
       m_pCamera->StopContinuousImageAcquisition();
     
      // Close camera
      // m_pCamera->Close();
     

     }
*/
    
    VmbUchar_t *pImage = NULL;
    res = pFrame->GetImage( pImage ); 
    if(res == VmbErrorSuccess)

    {
    logStream(MESSAGE_INFO)<<"Got the image "<<sendLog;
    m_header["TIME-OBS"] = std::make_pair(std::to_string(GetTime()),"time from prosilica SDK");

    
    createFits((unsigned short *)pImage,m_header,dueTimerVector);

    
    }
    else logStream(MESSAGE_INFO)<<"could not got the image "<<nFrameID<<sendLog;

if(nFrameID==m_numberOfExposures)
	     {

		 //Stop streaming
	         m_pCamera->StopContinuousImageAcquisition();
	        logStream(MESSAGE_INFO)<<"captured images is "<<nFrameID<<" stopping "<<sendLog; 
               return; 	 
	        }
			    


/*
    if(! SP_ISNULL( pFrame ) )
    {
        if( FrameInfos_Off != m_eFrameInfos )
        {
            ShowFrameInfos( pFrame);
        }
        VmbFrameStatusType status;
        VmbErrorType Result;
        Result = SP_ACCESS( pFrame)->GetReceiveStatus( status);
        if( VmbErrorSuccess == Result && VmbFrameStatusComplete == status)
        {
            std::vector<VmbUchar_t> TransformedData;
            if ( m_bRGB )
            {
                switch( m_eColorProcessing )
                {
                default:
                    Result = VmbErrorBadParameter;
                    logStream(MESSAGE_INFO)<<"unknown color processing parameter"<<sendLog;
                    break;
                case ColorProcessing_Off:
                    Result = TransformImage( pFrame, TransformedData, "RGB24" );
                    break;
                case ColorProcessing_Matrix:
                    {
                        logStream(MESSAGE_INFO)<<"Color Transform"<<sendLog;
                        const VmbFloat_t Matrix[] = {   0.6f, 0.3f, 0.1f, 
                                                        0.6f, 0.3f, 0.1f, 
                                                        0.6f, 0.3f, 0.1f};
                        Result = TransformImage( pFrame, TransformedData,"BGR24", Matrix );
                    }
                    break;

                }
                if( VmbErrorSuccess == Result && TransformedData.size() >=3 )
                {
                    char old_fill_char = std::cout.fill('0');
                    logStream(MESSAGE_INFO)<<std::hex <<"R = 0x"<<std::setw(2)<<(int)TransformedData[0]<<" "
                                        <<"G = 0x"<<std::setw(2)<<(int)TransformedData[1]<<" "
                                        <<"B = 0x"<<std::setw(2)<<(int)TransformedData[2]<<std::dec<<sendLog;
                    std::cout.fill( old_fill_char );
                }
                else
                {
                    logStream(MESSAGE_INFO)<<"Transformation failed."<<sendLog;
                }
            }
        }
        else
        {
            logStream(MESSAGE_INFO)<<"frame incomplete"<<sendLog;
        }
    }
    else
    {
        logStream(MESSAGE_INFO)<<" frame pointer NULL"<<sendLog;
    }
*/
    m_pCamera->QueueFrame( pFrame );
}
}}} // namespace AVT::VmbAPI::Examples
