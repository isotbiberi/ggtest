/*=============================================================================
  Copyright (C) 2013 - 2017 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ApiController.cpp

  Description: Implementation file for the ApiController helper class that
               demonstrates how to implement an asynchronous, continuous image
               acquisition with VimbaCPP.

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
#include <sstream>
#include <iostream>

#include "ApiController.h"
#include "Common/StreamSystemInfo.h"
#include "Common/ErrorCodeToMessage.h"

#include "logstream.h"
#include "app.h"

namespace AVT {
namespace VmbAPI {
namespace Examples {

#define NUM_FRAMES 3

ApiController::ApiController()
    // Get a reference to the Vimba singleton
    : m_system ( VimbaSystem::GetInstance() )
{}

ApiController::~ApiController()
{
}

//
// Translates Vimba error codes to readable error messages
//
// Parameters:
//  [in]    eErr        The error code to be converted to string
//
// Returns:
//  A descriptive string representation of the error code
//
std::string ApiController::ErrorCodeToMessage( VmbErrorType eErr ) const
{
    return AVT::VmbAPI::Examples::ErrorCodeToMessage( eErr );
}

//
// Starts the Vimba API and loads all transport layers
//
// Returns:
//  An API status code
//
VmbErrorType ApiController::StartUp()
{
    return m_system.Startup();
}

//
// Shuts down the API
//
void ApiController::ShutDown()
{
    // Release Vimba
    m_system.Shutdown();
}

//
// Opens the given camera
// Sets the maximum possible Ethernet packet size
// Adjusts the image format
// Sets up the observer that will be notified on every incoming frame
// Calls the API convenience function to start image acquisition
// Closes the camera in case of failure
//
// Parameters:
//  [in]    Config      A configuration struct including the camera ID and other settings
//
// Returns:
//  An API status code
//
VmbErrorType ApiController::StartContinuousImageAcquisition( const ProgramConfig& Config ,double expTime, std::map<std::string,std::pair<std::string, std::string>> header,long numberOfExposures)
{
    logStream(MESSAGE_INFO)<<"Continuous image acquisition started"<<sendLog;	
    // Open the desired camera by its ID
    VmbErrorType res = m_system.OpenCameraByID( Config.getCameraID().c_str(), VmbAccessModeFull, m_pCamera );
    if ( VmbErrorSuccess == res )
    {
        // Set the GeV packet size to the highest possible value
        // (In this example we do not test whether this cam actually is a GigE cam)
        FeaturePtr pCommandFeature;
        if ( VmbErrorSuccess == m_pCamera->GetFeatureByName( "GVSPAdjustPacketSize", pCommandFeature ))
        {
            if ( VmbErrorSuccess == pCommandFeature->RunCommand() )
            {
                bool bIsCommandDone = false;
                do
                {
                    if ( VmbErrorSuccess != pCommandFeature->IsCommandDone( bIsCommandDone ))
                    {
                        break;
                    }
                } while ( false == bIsCommandDone );
            }
        }

        if ( VmbErrorSuccess == res )
        {
            // set camera so that transform algorithms will never fail
            res = PrepareCamera(expTime,numberOfExposures);
            if ( VmbErrorSuccess == res )
            {
                // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
                m_pFrameObserver = new FrameObserver( m_pCamera, Config.getFrameInfos(), Config.getColorProcessing(), Config.getRGBValue(),header,numberOfExposures);
                // Start streaming
                res = m_pCamera->StartContinuousImageAcquisition( NUM_FRAMES, IFrameObserverPtr( m_pFrameObserver ));
            }
        }
        if ( VmbErrorSuccess != res )
        {
            // If anything fails after opening the camera we close it
            m_pCamera->Close();
        }
    }

   return res;
}

/**setting a feature to maximum value that is a multiple of 2*/
VmbErrorType SetIntFeatureValueModulo2( const CameraPtr &pCamera, const char* const& Name )
{
    VmbErrorType    result;
    FeaturePtr      feature;
    VmbInt64_t      value_min,value_max;
    result = SP_ACCESS( pCamera )->GetFeatureByName( Name, feature );
    if( VmbErrorSuccess != result )
    {
        return result;
    }
    result = SP_ACCESS( feature )->GetRange( value_min, value_max );
    if( VmbErrorSuccess != result )
    {
        return result;
    }
    value_max =( value_max>>1 )<<1;
    result = SP_ACCESS( feature )->SetValue ( value_max );
    return result;
}
/**prepare camera so that the delivered image will not fail in image transform*/
VmbErrorType ApiController::PrepareCamera(double expTime,long numberOfExposures)
{
    VmbErrorType result;
    result = SetIntFeatureValueModulo2( m_pCamera, "Width" );
    if( VmbErrorSuccess != result )
    {
        return result;
    }
    result = SetIntFeatureValueModulo2( m_pCamera, "Height" );
    if( VmbErrorSuccess != result )
    {
        return result;
    }
    
    
    FeaturePtr pFormatFeature;
    double val;
    result = m_pCamera->GetFeatureByName( "ExposureTimeAbs", pFormatFeature );
    if ( VmbErrorSuccess == result)
    {
  	result=pFormatFeature->GetValue(val);     
	
	if(VmbErrorSuccess==result)
	logStream(MESSAGE_INFO)<<"Exposure time is "<<val<<sendLog;

    } 
     
    result=pFormatFeature->SetValue(expTime);
    if ( VmbErrorSuccess == result)
      {
        result=pFormatFeature->GetValue(val);  
			             
       if(VmbErrorSuccess==result)
       logStream(MESSAGE_INFO)<<"Exposure time is "<<val<<sendLog;
        else   logStream(MESSAGE_INFO)<<"Can not get exposure time to  "<<expTime<<result<<sendLog;
      } 
      else
	      logStream(MESSAGE_INFO)<<"can not set exposure time"<<sendLog;


   VmbInt64_t acqMode;
   result = m_pCamera->GetFeatureByName("AcquisitionMode",pFormatFeature);
   if(VmbErrorSuccess == result)
   {
	  result = pFormatFeature->GetValue(acqMode);
	  if(VmbErrorSuccess == result)
	  logStream(MESSAGE_INFO)<<"Acquisition mode is " << acqMode<<sendLog;
	  else logStream(MESSAGE_INFO)<<"can not getValue acquision mode"<<result<<sendLog;
    }
   else logStream(MESSAGE_INFO)<<"Acquisition mode feature can not be get "<<result<<sendLog; 

   result = pFormatFeature->SetValue("MultiFrame");
   if(VmbErrorSuccess == result)
	   logStream(MESSAGE_INFO)<<"acquisition mode is set to multi frame"<<sendLog;
   else
	   logStream(MESSAGE_INFO)<<"Cannot set acquisition mode to multi " <<result <<sendLog;
 
 


  VmbInt64_t acqFrameCount;
  result = m_pCamera->GetFeatureByName("AcquisitionFrameCount",pFormatFeature);
  if(VmbErrorSuccess == result)
    {
	 result = pFormatFeature->GetValue(acqFrameCount);
	 if(VmbErrorSuccess == result)
         logStream(MESSAGE_INFO)<<"Acquisition frame count is  " << acqFrameCount<<sendLog;
         else logStream(MESSAGE_INFO)<<"can not get acquisition frame count"<<result<<sendLog;
     }
         else logStream(MESSAGE_INFO)<<"Acquisition Frame Count  feature count can not be get "<<result<<sendLog;

   result = pFormatFeature->SetValue((int)numberOfExposures);
   if(VmbErrorSuccess == result)
   {
	  result = pFormatFeature->GetValue(acqFrameCount);
	  logStream(MESSAGE_INFO)<<"acq frame count is set"<<sendLog;
	  if(result ==VmbErrorSuccess) logStream(MESSAGE_INFO)<<"frame count is " <<acqFrameCount<<sendLog;
   }
   else  logStream(MESSAGE_INFO)<<"can not set acquisition frame count "<<result<<sendLog; 

    VmbInt64_t triggerSource;
    result = m_pCamera->GetFeatureByName( "TriggerSource", pFormatFeature );
    if ( VmbErrorSuccess == result)
		    {
			result=pFormatFeature->GetValue(triggerSource);

		        if(VmbErrorSuccess==result)
				logStream(MESSAGE_INFO)<<"Trigger Source is  "<<triggerSource<<sendLog;

		     }



    return result;
}

//
// Calls the API convenience function to stop image acquisition
// Closes the camera
//
// Returns:
//  An API status code
//
VmbErrorType ApiController::StopContinuousImageAcquisition()
{
    // Stop streaming
    m_pCamera->StopContinuousImageAcquisition();

    // Close camera
    return  m_pCamera->Close();
}

//
// Gets all cameras known to Vimba
//
// Returns:
//  A vector of camera shared pointers
//
CameraPtrVector ApiController::GetCameraList() const
{
    CameraPtrVector cameras;
    // Get all known cameras
    if ( VmbErrorSuccess == m_system.GetCameras( cameras ))
    {
        // And return them
        return cameras;
    }
    return CameraPtrVector();
}

//
// Gets the version of the Vimba API
//
// Returns:
//  The version as string
//
std::string ApiController::GetVersion() const
{
    std::ostringstream  os;
    os<<m_system;
    return os.str();
}


//
// Opens the given camera
// Sets the maximum possible Ethernet packet size
// Adjusts the image format
// Calls the API convenience function to start single image acquisition
// Closes the camera in case of failure
//
// Parameters:
//  [in]    rStrCameraID        The ID of the camera to work on
//  [out]   rpFrame             The frame that will be filled. Does not need to be initialized.
//
// Returns:
//  An API status code
//
VmbErrorType ApiController::AcquireSingleImage( const std::string &rStrCameraID, FramePtr &rpFrame,double expTime )
{
    // Open the desired camera by its ID
    VmbErrorType res = m_system.OpenCameraByID( rStrCameraID.c_str(), VmbAccessModeFull, m_pCamera );
    if ( VmbErrorSuccess == res )
    {
        // Set the GeV packet size to the highest possible value
        // (In this example we do not test whether this cam actually is a GigE cam)
        FeaturePtr pCommandFeature;
        if ( VmbErrorSuccess == m_pCamera->GetFeatureByName( "GVSPAdjustPacketSize", pCommandFeature ))
        {
            if ( VmbErrorSuccess == pCommandFeature->RunCommand() )
            {
                bool bIsCommandDone = false;
                do
                {
                    if ( VmbErrorSuccess != pCommandFeature->IsCommandDone( bIsCommandDone ))
                    {
                        break;
                    }
                } while ( false == bIsCommandDone );
            }
        }
        FeaturePtr pFormatFeature;
        // Set pixel format. For the sake of simplicity we only support Mono and BGR in this example.
        res = m_pCamera->GetFeatureByName( "PixelFormat", pFormatFeature );
        if ( VmbErrorSuccess == res )
        {
            // Try to set BGR
            res = pFormatFeature->SetValue( VmbPixelFormatRgb8 );
            if ( VmbErrorSuccess != res )
            {
                // Fall back to Mono
                res = pFormatFeature->SetValue( VmbPixelFormatMono8 );
            }

             FeaturePtr pFormatFeature;
	     double val;
             res = m_pCamera->GetFeatureByName( "ExposureTimeAbs", pFormatFeature );
             if ( VmbErrorSuccess == res)
	     {
		res=pFormatFeature->GetValue(val);
	        if(VmbErrorSuccess==res)
	        logStream(MESSAGE_INFO)<<"Exposure time is "<<val<<sendLog;
             }

             res=pFormatFeature->SetValue(expTime);
             if ( VmbErrorSuccess == res)
	     {
                res=pFormatFeature->GetValue(val);

                if(VmbErrorSuccess==res)
               logStream(MESSAGE_INFO)<<"Exposure time is "<<val<<sendLog;
                else   logStream(MESSAGE_INFO)<<"Can not set exposure time to  "<<expTime<<res<<sendLog;
            }

            if ( VmbErrorSuccess == res )
            {
                // Acquire
                res = m_pCamera->AcquireSingleImage( rpFrame, 5000 );
            }
        }

        m_pCamera->Close();
    }

    return res;
}





}}} // namespace AVT::VmbAPI::Examples
