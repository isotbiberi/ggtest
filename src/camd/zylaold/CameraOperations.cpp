#include"CameraOperations.h"
#include <iostream>
#include <string>
//#include"camd.h"
//#include"FitsOperations.h"
#include<ctime>
//#include"LogOperations.h"
using namespace std;
//static methods predefinition
static const char* return_from_error_code(int error_code);

#define THROW_IF_NOT_SUCCESS(command,error_prefix)\
{									\
  int ret_code = command;						\
  if ( AT_SUCCESS != ret_code ){\
  logStream(MESSAGE_DEBUG) << error_prefix << " "<<return_from_error_code(ret_code)<<sendLog; \
}\
}

static const AT_WC* AcquisitionStart = L"AcquisitionStart";
static const AT_WC* AcquisitionStop = L"AcquisitionStop";
static const AT_WC* AOIBinning = L"AOIBinning";
static const AT_WC* AOIHBin = L"AOIHBin";
static const AT_WC* AOIHeight = L"AOIHeight";
static const AT_WC* AOILeft = L"AOILeft";
static const AT_WC* AOIStride = L"AOIStride";
static const AT_WC* AOITop = L"AOITop";
static const AT_WC* AOIVBin = L"AOIVBin";

static const AT_WC* AOIWidth = L"AOIWidth";
static const AT_WC* BitDepth = L"BitDepth";
static const AT_WC* BufferOverflowEvent = L"BufferOverflowEvent";
static const AT_WC* BytesPerPixel = L"BytesPerPixel";
static const AT_WC* CameraAcquiring = L"CameraAcquiring";
static const AT_WC* CameraModel = L"CameraModel";
static const AT_WC* CycleMode = L"CycleMode";
static const AT_WC* DeviceCount = L"DeviceCount";
static const AT_WC* ElectronicShutteringMode = L"ElectronicShutteringMode";
static const AT_WC* ExposureTime = L"ExposureTime";
static const AT_WC* FanSpeed = L"FanSpeed";
static const AT_WC* FirmwareVersion = L"FirmwareVersion";
static const AT_WC* FrameRate = L"FrameRate";
static const AT_WC* FullAOIControl = L"FullAOIControl";
static const AT_WC* ImageSizeBytes = L"ImageSizeBytes";
static const AT_WC* MaxInterfaceTransferRate = L"MaxInterfaceTransferRate";

static const AT_WC* Overlap = L"Overlap";
static const AT_WC* PixelEncoding = L"PixelEncoding";
static const AT_WC* PixelReadoutRate = L"PixelReadoutRate";
static const AT_WC* PreAmpGainControl = L"PreAmpGainControl";
static const AT_WC* ReadoutTime = L"ReadoutTime";
static const AT_WC* SensorCooling = L"SensorCooling";
static const AT_WC* SensorHeight = L"SensorHeight";
static const AT_WC* SensorTemperature = L"SensorTemperature";
static const AT_WC* SensorWidth = L"SensorWidth";
static const AT_WC* SerialNumber = L"SerialNumber";
static const AT_WC* SimplePreAmpGainControl = L"SimplePreAmpGainControl";
static const AT_WC* SoftwareTrigger = L"SoftwareTrigger";

static const AT_WC* SpuriousNoiseFilter = L"SpuriousNoiseFilter";
static const AT_WC* SynchronousTriggering = L"SynchronousTriggering";
static const AT_WC* TargetSensorTemperature = L"TagetSensorTemperature";
static const AT_WC* TemperatureStatus = L"TemperatureStatus";
static const AT_WC* TriggerMode = L"TriggerMode";
static const AT_WC* FrameCount = L"FrameCount";
static const AT_WC* MetadataEnable = L"MetadataEnable";
static const AT_WC* MetadataFrame = L"MetadataFrame"; 
static const AT_WC* MetadataTimestamp = L"MetadataTimestamp";

static const unsigned int MAX_CHARS_INFO_STRING = 64;
static const unsigned int LENGTH_FIELD_SIZE = 4;
static const unsigned int CID_FIELD_SIZE = 4;

int Andor_Camera::getIntSystem(const AT_WC* Feature, AT_64* Value)
{
	return (AT_SUCCESS != AT_GetInt(AT_HANDLE_SYSTEM, Feature, Value));
}

int Andor_Camera::getString(const AT_WC* Feature, AT_WC* String, int StringLength) const
{
	
	return AT_GetString(m_camera_handle, Feature, String, StringLength);
}

void Andor_Camera::getSerialNumber(std::string &o_sn) const
{
	
	AT_WC	the_sn[1024];
	getString(SerialNumber, the_sn, 1024);
	o_sn = WStringToString(the_sn);
}

void Andor_Camera::getFirmwareVersion(std::string &o_fwv) const
{
	
	AT_WC	the_fwv[1024];
	getString(FirmwareVersion, the_fwv, 1024);
	o_fwv = WStringToString(the_fwv);
}


void Andor_Camera::initializeController()
{

        // --- Get available cameras and select the choosen one.^M
        AT_64 numCameras;
        //std::cout << "Get all attached cameras";^M
        THROW_IF_NOT_SUCCESS(getIntSystem(DeviceCount, &numCameras),
                "No camera present!");


        //        logStream(MESSAGE_DEBUG) << "Get all attached cameras"<<sendLog;
        //        logStream(MESSAGE_DEBUG) << "Found " << numCameras << " camera" << ((numCameras>1) ? "s" : "")<<sendLog;
        //        logStream(MESSAGE_DEBUG) << "Try to set current camera to number " << m_camera_number<<sendLog;


 if (m_camera_number < numCameras && m_camera_number >= 0) {
                // Getting the m_camera_handle WITH some error checking :
                THROW_IF_NOT_SUCCESS(AT_Open(m_camera_number, &m_camera_handle),
                        "Cannot get camera handle");
                
        }
        else {
                //std::cout << "Invalid camera number "
                //      << m_camera_number << ", there is "
                //      << numCameras << " available";
                        logStream(MESSAGE_DEBUG) << "Invalid camera number "
                                << m_camera_number << ", there is "
                                << numCameras << " available"<<sendLog;
        }




        // --- Get Camera model (and all other parameters which are not changing during camera setup and usage )
        AT_WC   model[1024];
        THROW_IF_NOT_SUCCESS(getString(CameraModel, model, 1024),
                "Cannot get camera model");

        m_detector_model = WStringToString(std::wstring(model));

                logStream(MESSAGE_DEBUG) << "camera model is " << m_detector_model << sendLog;



  if (m_detector_model != "SIMCAM CMOS") {
                std::string             the_serial, the_fw;

                getSerialNumber(the_serial);
                getFirmwareVersion(the_fw);
                m_real_camera = true;
                m_detector_model += " (SN : " + the_serial + ", firmware " + the_fw + ")";


                        logStream(MESSAGE_DEBUG) << "Camera is ready: type " << m_detector_type << ", model " << m_detector_model << sendLog;

        }
        else {
                m_real_camera = false;
                m_detector_type = std::string("Simulator");


                        logStream(MESSAGE_DEBUG) << "The camera is indeed the SIMULATED camera, all exception for invalid parameter name will be ignored!!!"<<sendLog;

                        logStream(MESSAGE_DEBUG) << "BE VERY CAREFULL : The andor SDK3 camera that you are connected to is a SIMULATED CAMERA !!!"<<sendLog;
}

	

	// Carefully crafting the order, since some are affecting others...
	
	//m_exp_time = 1;
//	std::cout << "exptime is " << m_exp_time <<" shutter mode is"<<m_electronic_shutter_mode<<" triggermode is "<<m_trig_mode<<" simple gain is "<<the_simple_gain<<" adcarate is"<<the_rate<< std::endl;


	//logStream(MESSAGE_INFO) << "exptime is " << m_exp_time << " shutter mode is" << m_electronic_shutter_mode << " triggermode is " << m_trig_mode << " simple gain is " << the_simple_gain << " adcarate is" << the_rate << sendLog;

}


void Andor_Camera::afterInitialization()
{

	A3_BitDepth	the_bd = m_bit_depth;
	A3_SimpleGain 	the_simple_gain = m_simple_gain;
	A3_ReadOutRate	the_rate = m_adc_rate;
       // bool test ;
       // getIsAcquiring(test);
      //  logStream(MESSAGE_INFO)<<"is acquiring is  " <<test<<sendLog;

    //	setExpTime(m_exp_time);
	//logStream(MESSAGE_INFO)<<"setting the exptime to " <<m_exp_time<<sendLog;
        

        setMetaData(true);
	setMetaDataTimeStamp(true);
	
        setElectronicShutterMode(m_electronic_shutter_mode);


//	setTriggerMode(m_trig_mode);//bunu secince setExptime yapamadık
	
       setSimpleGain(the_simple_gain);
	setAdcRate(the_rate);
	setBitDepth(the_bd);
	setCooler(m_cooler);
	setTemperatureSP(m_temperature_sp);
	//setExpTime(m_exp_time);
	setSpuriousNoiseFilter(false);

	AT_64	the_chip_width, the_chip_height;
	getInt(SensorWidth, &the_chip_width);
	getInt(SensorHeight, &the_chip_height);

}
/*!
@brief Preparing the object and memory for an acquisition sequence

This method is called just before the acquisition is started (by startAcq).
It is responsible for setting both the internals of the object and the
internals of the camera's SDK for the planned acquisition sequence.
Most of the work consists in the setting of the frame buffers.
*/
/*!
Maybe later : write a specific SoftBufferAllocMgr or SoftBufferCtrlObj
subclass that better handles :
* the stride of the image
* the image size, the best being to get the value from the SDK
itself though ImageSizeBytes
* possibility to handle the metadata (in particular the
frame timestamp generated on the FPGA clock)
*/

void Andor_Camera::prepareAcq()
{

        //THROW_IF_NOT_SUCCESS(AT_Open(m_camera_number, &m_camera_handle),
        //                "Cannot get camera handle");

	m_image_index = 0;

	// Get from SDK the real frame size with stride (padding columns)
	std::string	the_bit_depth;
//	FrameDim      the_frame_mem_dim;
	int           the_frame_mem_size;
	int           the_alloc_frames;
	

	getSdkFrameDim(the_sdk_frame_dim, false);


	if (!m_real_camera) 
			logStream(MESSAGE_DEBUG) << "BE VERY CAREFULL : The andor SDK3 camera that you are connected to is a SIMULATED CAMERA !!!"<<sendLog;

		
 	logStream(MESSAGE_DEBUG) << "Flushing the queue of the framegrabber"<<sendLog;
	//setEnumString(CycleMode, L"Continuous");
 	setEnumString(CycleMode, L"Fixed");
 //	setInt(FrameCount, 100);

	AT_WC the_string[256];
   	getEnumString(CycleMode, the_string, 256);
    logStream(MESSAGE_DEBUG) << "Tried to set the acq is Fixed mode " << WStringToString(std::wstring(the_string))<<sendLog;
	
    AT_Flush(m_camera_handle);
	createBuffers(m_camera_handle);
        
	unpackedBuffer = new unsigned short[the_sdk_frame_dim.height*the_sdk_frame_dim.width];
	fitsBuffer = new unsigned short*[the_sdk_frame_dim.height*the_sdk_frame_dim.width];

}
unsigned short* Andor_Camera::getBuffer()
{
	return unpackedBuffer;
}
unsigned short * Andor_Camera::getConvertedBuffer()
{

int returnCode= AT_ConvertBuffer(Buffer,
                reinterpret_cast<unsigned char*>(unpackedBuffer),
                the_sdk_frame_dim.width, the_sdk_frame_dim.height,                              the_sdk_frame_dim.stride, L"Mono12Packed", L"Mono16");

return unpackedBuffer;
}
unsigned short* Andor_Camera::getFitsBuffer(int count)
{
	return fitsBuffer[count];

}
void Andor_Camera::startAcq()
{
         int BufferSize = 11059200;
         unsigned char* UserBuffer = new unsigned char[BufferSize];      

         //THROW_IF_NOT_SUCCESS(AT_QueueBuffer(m_camera_handle, UserBuffer, BufferSize),
          //      "Cannot Queue the frame buffer");
         //logStream(MESSAGE_INFO) << "after queue buffer"<<sendLog;



	if (!m_real_camera) {
		//std::cout << "BE VERY CAREFULL : The andor SDK3 camera that you are connected to is a SIMULATED CAMERA !!!";
			logStream(MESSAGE_DEBUG) << "BE VERY CAREFULL : The andor SDK3 camera that you are connected to is a SIMULATED CAMERA !!!"<<sendLog;
	

		logStream(MESSAGE_DEBUG) << "Starting the acquisition by the camera (or triggering when in software trigger mode)"<<sendLog;
	}
        if (0 == m_image_index) {
		// Setting the start timestamp of the buffer :
		//m_buffer_ctrl_obj->getBuffer().setStartTimestamp(Timestamp::now());
		// Sending the start command to the SDK :
		THROW_IF_NOT_SUCCESS(AT_Command(m_camera_handle, L"AcquisitionStart"), "Acquisition can not startted");
		//sendCommand(AcquisitionStart);
			logStream(MESSAGE_DEBUG) << "Acquisition start command run"<<sendLog;
	}

	if (Software == m_trig_mode) {
		// If we are in software trigger mode, the call to startAcq serves as the trigger :
		//sendCommand(SoftwareTrigger);
		THROW_IF_NOT_SUCCESS(AT_Command(m_camera_handle, L"SoftwareTrigger"), "SoftwareTrigger can not be applied");
			logStream(MESSAGE_DEBUG) << "software trigger is applied"<<sendLog;
	}
//unsigned char * Buffer2;
	THROW_IF_NOT_SUCCESS(AT_WaitBuffer(m_camera_handle, &Buffer, &BufferSize, AT_INFINITE), "Cannot Get Frame");
        logStream(MESSAGE_DEBUG) << "image taken in for loop"<<sendLog;

        bool cameraIsAcquiring=false;
        getBool(CameraAcquiring,&cameraIsAcquiring);
        logStream(MESSAGE_DEBUG) <<"before while"<< cameraIsAcquiring<<sendLog;
       while(m_image_index<m_nb_frames_to_collect)
       {

unpackedBuffer = (unsigned short *)Buffer;



	/*Application specific codes*/

/*  
int returnCode=        	   AT_ConvertBuffer(Buffer2,
		reinterpret_cast<unsigned char*>(unpackedBuffer),
		the_sdk_frame_dim.width, the_sdk_frame_dim.height,  		                the_sdk_frame_dim.stride, L"Mono12Packed", L"Mono16");
	
logStream(MESSAGE_INFO) << "convertbuffer return is "<<returnCode;
*/
// unpackedBuffer= (unsigned short*)Buffer2;

         fitsBuffer[m_image_index]=unpackedBuffer;
	 // delete[] Buffer;
	/*Application specific codes*/
	  // if (imageCount == 50)imageCount = 0;
	  // else imageCount++;
	   m_image_index++;

	//delete[] UserBuffer;


	THROW_IF_NOT_SUCCESS(AT_QueueBuffer(m_camera_handle, UserBuffer, BufferSize),"Cannot Queue the frame buffer");

        logStream(MESSAGE_DEBUG) << "after queue buffer image index is"<<(long long)m_image_index<<sendLog;

        getBool(CameraAcquiring,&cameraIsAcquiring);
       }//while acquiring

		logStream(MESSAGE_DEBUG) << "Done, the acquisition is now started and the frame retrieving should take place in parallel in a second thread"<<sendLog;
		getBool(CameraAcquiring,&cameraIsAcquiring);
		logStream(MESSAGE_DEBUG) << cameraIsAcquiring<<sendLog;
		stopAcq();
		getBool(CameraAcquiring,&cameraIsAcquiring);
		 logStream(MESSAGE_DEBUG) <<"after stopAcq"<< cameraIsAcquiring<<sendLog;
}
void Andor_Camera::stopAcq()
{
bool cameraIsAcquiring;
getBool(CameraAcquiring,&cameraIsAcquiring);//belki kullanicam
sendCommand(AcquisitionStop);

}


void Andor_Camera::deleteBuffers()
{
	delete[] Buffer;
	delete[] UserBuffer;
	delete[]acqBuffer;
	acqBuffer = NULL;
	delete[]acqBuffer1;
	acqBuffer1 = NULL;
	delete[]acqBuffer2;
	acqBuffer2 = NULL;
	delete[]acqBuffer3;
	acqBuffer3 = NULL;
	delete[]acqBuffer4;
	acqBuffer4 = NULL;

	delete[]acqBuffer5;
	acqBuffer5 = NULL;
	delete[]acqBuffer6;
	acqBuffer6 = NULL;
	delete[]acqBuffer7;
	acqBuffer7 = NULL;
	delete[]acqBuffer8;
	acqBuffer8 = NULL;
	delete[]acqBuffer9;
	acqBuffer9 = NULL;
	delete[]acqBuffer10;
	acqBuffer10 = NULL;
	delete[]acqBuffer11;
	acqBuffer11 = NULL;
	delete[]acqBuffer12;
	acqBuffer12 = NULL;
	delete[]acqBuffer13;
	acqBuffer13 = NULL;
	delete[]acqBuffer14;
	acqBuffer14 = NULL;
	delete[]acqBuffer15;
	acqBuffer15 = NULL;
	delete[]acqBuffer16;
	acqBuffer16 = NULL;
	delete[]acqBuffer17;
	acqBuffer17 = NULL;
	delete[]acqBuffer18;
	acqBuffer18 = NULL;
	delete[]acqBuffer19;
	acqBuffer19 = NULL;
}
void Andor_Camera::createBuffers(int _handle)
{
	acqBuffer = QueueBuffer(_handle);
	acqBuffer1 = QueueBuffer(_handle);
	acqBuffer2 = QueueBuffer(_handle);
	acqBuffer3 = QueueBuffer(_handle);
	acqBuffer4 = QueueBuffer(_handle);
	acqBuffer5 = QueueBuffer(_handle);
	acqBuffer6 = QueueBuffer(_handle);
	acqBuffer7 = QueueBuffer(_handle);
	acqBuffer8 = QueueBuffer(_handle);
	acqBuffer9 = QueueBuffer(_handle);
	acqBuffer10 = QueueBuffer(_handle);
	acqBuffer11 = QueueBuffer(_handle);
	acqBuffer12 = QueueBuffer(_handle);
	acqBuffer13 = QueueBuffer(_handle);
	acqBuffer14 = QueueBuffer(_handle);
	acqBuffer15 = QueueBuffer(_handle);
	acqBuffer16 = QueueBuffer(_handle);
	acqBuffer17 = QueueBuffer(_handle);
	acqBuffer18 = QueueBuffer(_handle);
	acqBuffer19 = QueueBuffer(_handle);
}

unsigned char* Andor_Camera::QueueBuffer(int _handle) {
	//int iError;
	unsigned char* acqBuffer = NULL;
	// Get the number of bytes required to store one frame
	//AT_64 ImageSizeBytes = 0;
	//iError = AT_GetInt(_handle, L"ImageSizeBytes", &ImageSizeBytes);
	//if (iError != AT_SUCCESS) {
//		cout << "AT_GetInt failed - ImageSizeBytes - return code " << iError << endl;
//	}

	//if (iError == AT_SUCCESS) {
		// Allocate a memory buffer to store one frame
		acqBuffer = new unsigned char[the_sdk_frame_dim.size];
		// Make sure the data is 64-bit aligned, necessary for bitflow SDK
		//unsigned char* alignedBuffer = reinterpret_cast<unsigned char*>((reinterpret_cast<unsigned long>(acqBuffer) + 7) & ~7);
		// Pass this buffer to the SDK
	//	iError = AT_QueueBuffer(_handle, acqBuffer, static_cast<int>(the_sdk_frame_dim.size));
	//	if (iError != AT_SUCCESS) {
	//		cout << "AT_QueueBuffer failed - Image Size Bytes - return code " << iError << endl;
	//	}

		THROW_IF_NOT_SUCCESS(AT_QueueBuffer(_handle, acqBuffer, static_cast<int>(the_sdk_frame_dim.size)),
			"Cannot Queue the frame buffer");



	//}
	return acqBuffer;
}

void Andor_Camera::setNumberOfFrames(size_t totalFrames)
{
	m_nb_frames_to_collect = totalFrames;
}
size_t Andor_Camera::getNumberOfFrames()
{
	return m_nb_frames_to_collect;
}

void Andor_Camera::getSdkFrameDim(SdkFrameDim &frame_dim, bool last)
{
	

	SdkFrameDim the_frame_dim;

	if (!last)
	{
		// Retrieving useful information from the camera :

		//std::cout << "Getting the basic information from the camera : image size (in bytes) and pixel encoding.";

			logStream(MESSAGE_DEBUG) << "Getting the basic information from the camera : image size (in bytes) and pixel encoding."<<sendLog;
		getInt(ImageSizeBytes, &the_frame_dim.size);
		getEnumIndex(PixelEncoding, (int *)&the_frame_dim.encoding);
		getFloat(BytesPerPixel, &the_frame_dim.bytes_per_pixel);
		int the_bit_depth_index;
		getHwBitDepth(&the_bit_depth_index);
	//	std::cout << "The image size in bytes is " << the_frame_dim.size
	//		<< ", the pixel encoding index is " << the_frame_dim.encoding
	//		<< " and finally byte per pixel " << the_frame_dim.bytes_per_pixel;

			logStream(MESSAGE_DEBUG) << "The image size in bytes is " << the_frame_dim.size << ", the pixel encoding index is " << the_frame_dim.encoding << " and finally byte per pixel " << the_frame_dim.bytes_per_pixel<<sendLog;
		the_frame_dim.is_encoded = the_frame_dim.encoding == Mono12Packed;

		// Indeed it seems LIMA is not having the difference between width and stride.
		// So we have to compute the width provided to LIMA from the stride and the
		// byte per pixel settings.

		getInt(AOIWidth, &the_frame_dim.width);
		getInt(AOIHeight, &the_frame_dim.height);
		getInt(AOIStride, &the_frame_dim.stride);

		the_frame_dim.is_strided = (the_frame_dim.width * the_frame_dim.bytes_per_pixel) != the_frame_dim.stride;

		switch (the_frame_dim.encoding)
		{
		case Mono12Packed:
			the_frame_dim.input_encoding = L"Mono12Packed";
			the_frame_dim.output_encoding = L"Mono16";
			break;
		case Mono12:
			the_frame_dim.input_encoding = L"Mono12";
			the_frame_dim.output_encoding = L"Mono16";
			break;
		case Mono16:
			the_frame_dim.input_encoding = L"Mono16";
			the_frame_dim.output_encoding = L"Mono16";
			break;
		case Mono32:
			the_frame_dim.input_encoding = L"Mono32";
			the_frame_dim.output_encoding = L"Mono32";
			break;
		}
		m_sdk_frame_dim = the_frame_dim;
	}

	frame_dim = m_sdk_frame_dim;
}



void Andor_Camera::setElectronicShutterMode(A3_ShutterMode iMode)
{
	//DEB_MEMBER_FUNCT();
	int the_mode;
	THROW_IF_NOT_SUCCESS(setEnumIndex(ElectronicShutteringMode, iMode),"electronic shutter mode cant be set");
	THROW_IF_NOT_SUCCESS(getEnumIndex(ElectronicShutteringMode, &the_mode),"electronic shutter mode cant be get");
	m_electronic_shutter_mode = static_cast<A3_ShutterMode>(the_mode);
	if (m_electronic_shutter_mode != iMode) {
//		std::cout << "Proof-reading the electronic shutter mode :"
//			<< "\n\tGot " << m_electronic_shutter_mode << " back,"
//			<< "\n\twhile requesting " << iMode;

			logStream(MESSAGE_INFO) << "Proof-reading the electronic shutter mode :"
						<< "\n\tGot " << m_electronic_shutter_mode << " back,"
						<< "\n\twhile requesting " << iMode<<sendLog;
	int		the_gain, the_rate;

	THROW_IF_NOT_SUCCESS(getEnumIndex(SimplePreAmpGainControl, &the_gain),"simplepreamp gain control cant be get");
	THROW_IF_NOT_SUCCESS(getEnumIndex(PixelReadoutRate, &the_rate),"pixel read out rate cant be get");
	setSimpleGain(static_cast<A3_SimpleGain>(the_gain));
	setAdcRate(static_cast<A3_ReadOutRate>(the_rate));
}

}
/*!
@brief Setting the trigger mode of the camera through the SDK settings.
@param iMode : the mode to select
*/
void Andor_Camera::setTriggerMode(A3_TriggerMode iMode)
{
	
	if (m_real_camera) {
		int the_mode;
		setEnumIndex(TriggerMode, static_cast<int>(iMode));
		getEnumIndex(TriggerMode, &the_mode);
		m_trig_mode = static_cast<A3_TriggerMode>(the_mode);
		if (m_trig_mode != iMode) {
		//	std::cout << "Proof-reading the trigger mode :"
		//		<< "\n\tGot " << m_trig_mode << " back,"
		//		<< "\n\twhile requesting " << iMode;

				logStream(MESSAGE_INFO) << "Proof-reading the trigger mode :"
					<< "\n\tGot " << m_trig_mode << " back,"
					<< "\n\twhile requesting " << iMode<<sendLog;
		}
	}
	else { // Simulated camera -> setting it forcibly to �Advanced�
		int the_mode;
		setEnumIndex(TriggerMode, 5);
		getEnumIndex(TriggerMode, &the_mode);
		m_trig_mode = static_cast<A3_TriggerMode>(the_mode);
		//std::cout << "The SIMCAM has only one trigger-mode setting (Advanced), making sure that's what we are doing now";

			logStream(MESSAGE_INFO) << "The SIMCAM has only one trigger-mode setting (Advanced), making sure that's what we are doing now"<<sendLog;

	}
}


void Andor_Camera::setSimpleGain(A3_SimpleGain i_gain)
{
	
	if (propImplemented(SimplePreAmpGainControl)) {
		THROW_IF_NOT_SUCCESS(setEnumIndex(SimplePreAmpGainControl, i_gain),"simple preampgain control cant get");
	}
	else {
			logStream(MESSAGE_DEBUG) << "SimplePreAmpGainControl not implemented, emulating it in software"<<sendLog;

		A3_ShutterMode		the_shutter;

		getElectronicShutterMode(the_shutter);

		switch (i_gain) {
		case b11_low_gain:
			setAdcGain(Gain1);
			break;

		case b11_hi_gain:
			if (Rolling == the_shutter)
				setAdcGain(Gain4);
			else
				setAdcGain(Gain3);
			break;

		case b16_lh_gain:
			if (Rolling == the_shutter)
				setAdcGain(Gain1_Gain4);
			else
				setAdcGain(Gain1_Gain3);
			break;

		default:
		//	std::cout << "Not performing any settings since you provided an unavailable value";
				logStream(MESSAGE_INFO) << "Not performing any settings since you provided an unavailable value"<<sendLog;
			

			break;
		}
	}
}

void Andor_Camera::setAdcRate(A3_ReadOutRate iRate)
{
	
	if (m_real_camera) {
		int the_rate;
		THROW_IF_NOT_SUCCESS(setEnumIndex(PixelReadoutRate, iRate),"cant set pixelreadout rate");
		getEnumIndex(PixelReadoutRate, &the_rate);
		m_adc_rate = static_cast<A3_ReadOutRate>(the_rate);
		if (m_adc_rate != iRate) {
		//	std::cout << "Proof-reading the ADC readout rate :"
		//		<< "\n\tGot " << m_adc_rate << " back,"
		//		<< "\n\twhile requesting " << iRate;

				logStream(MESSAGE_DEBUG) << "Proof-reading the ADC readout rate :"
					<< "\n\tGot " << m_adc_rate << " back,"
					<< "\n\twhile requesting " << iRate<<sendLog;



		}
	}
	else {
		int the_rate;
		THROW_IF_NOT_SUCCESS(setEnumIndex(PixelReadoutRate, 0),"cant set pixel read out rate");
		getEnumIndex(PixelReadoutRate, &the_rate);
		m_adc_rate = static_cast<A3_ReadOutRate>(the_rate);
			logStream(MESSAGE_DEBUG) << "The SIMCAM has only one rate setting (550MHz), making sure that's what we are doing now"<<sendLog;
	}
}
void Andor_Camera::setBitDepth(A3_BitDepth iMode)
{
	
	int the_mode;
	THROW_IF_NOT_SUCCESS(setEnumIndex(BitDepth, static_cast<int>(iMode)),"cant set bit depth");
	getHwBitDepth(&the_mode);
	m_bit_depth = static_cast<A3_BitDepth>(the_mode);
	if (m_bit_depth != iMode) {
	//	std::cout << "Proof-reading the image bit-depth :"
	//		<< "\n\tGot " << m_bit_depth << " back,"
	//		<< "\n\twhile requesting " << iMode;

		logStream(MESSAGE_DEBUG) <<"Proof-reading the image bit-depth :"
						<< "\n\tGot " << m_bit_depth << " back,"
						<< "\n\twhile requesting " << iMode<<sendLog;
	}

	// Making sure that the pixel encoding is a predictable one, depending on the bit-depth.
	switch (m_bit_depth) {
	case b11:
		THROW_IF_NOT_SUCCESS(setEnumString(PixelEncoding, L"Mono12"),"cant set mono 12");
		break;
	case b16:
		THROW_IF_NOT_SUCCESS(setEnumString(PixelEncoding, L"Mono16"),"cant set mono16");
		break;
	default:
		break;
	}
}



void Andor_Camera::getIsAcquiring(bool isAcquiring)
{

getBool(CameraAcquiring,&isAcquiring);
logStream(MESSAGE_INFO) << "getIsAcquiring value is " << isAcquiring <<sendLog;

}


//-----------------------------------------------------
// @brief	start or Stop the cooler    // DONE
// @param	flag true-on, false-off
//
//-----------------------------------------------------
void Andor_Camera::setCooler(bool flag)
{
	
	setBool(SensorCooling, flag);
	getBool(SensorCooling, &m_cooler);
	if (flag != m_cooler) {

			logStream(MESSAGE_DEBUG) << "Failed to properly set the cooler : requested " << flag << ", but got " << m_cooler<<sendLog;
	}
}


//-----------------------------------------------------
// @brief	set the temperature set-point // DONE
// @param	temperature in centigrade
//
//-----------------------------------------------------
void Andor_Camera::setTemperatureSP(double temp)
{
	
	// Zyla-5.5 camera is supposed to not have this feature, but propImplemented returned True !!!
	if (propImplemented(TargetSensorTemperature) && m_detector_model.find("ZYLA5.5") == std::string::npos) {
		setFloat(TargetSensorTemperature, temp);
		getFloat(TargetSensorTemperature, &m_temperature_sp);
		if ((m_temperature_sp - temp) > 0.1 || (m_temperature_sp - temp) < -0.1) {

				logStream(MESSAGE_DEBUG) << "Proof-reading temperature set-point : "
					<< "\n\tproof-read = " << m_temperature_sp
					<< "\n\twhile asked to be = " << temp
				     << "Failed on setting temperature set-point"<<sendLog;

		}
	}
	else {
      	logStream(MESSAGE_DEBUG) << "This camera has no temperature set-point control !"<<sendLog;

		m_temperature_control_available = false;
	}
}

bool Andor_Camera::propImplemented(const AT_WC * iPropName) const
{

	AT_BOOL		b_exists;

	int ret_code;
	if (AT_SUCCESS != (ret_code = AT_IsImplemented(m_camera_handle, iPropName, &b_exists))) {
			logStream(MESSAGE_DEBUG) << "Error in printInfoForProp : " << return_from_error_code(ret_code)<<sendLog;

		return false;
	}
		logStream(MESSAGE_DEBUG) << "\tIsImplemented = " << atBoolToString(b_exists)<<sendLog;
	return b_exists;
}

void Andor_Camera::setMetaData(bool  metaData)
{
	THROW_IF_NOT_SUCCESS(setBool(MetadataEnable, metaData),
		"setMetaData encountered a problem");

}
void Andor_Camera::getMetaData(bool& metaData) const
{

	bool	the_meta_data;
	getBool(MetadataEnable, &the_meta_data);
	metaData = the_meta_data;
}

AT_64 Andor_Camera::GetTimeStamp(unsigned char* pBuf)
{

	typedef unsigned long   AT_U32;

	int imageSize = 0;
	getImageSize(imageSize);
		
	AT_64 i64_timestamp = 0;

	bool foundTimestamp = false;
	AT_U8* puc_metadata = pBuf + static_cast<int>(imageSize); //start at end of buffer
	do {
		//move pointer to length field
		puc_metadata -= LENGTH_FIELD_SIZE;
		AT_U32 featureSize = *(reinterpret_cast<AT_U32*>(puc_metadata));

		//move pointer to Chunk identifier
		puc_metadata -= CID_FIELD_SIZE;
		AT_U32 cid = *(reinterpret_cast<AT_U32*>(puc_metadata));

		//move pointer to start of data
		puc_metadata -= (featureSize - CID_FIELD_SIZE);

		if (CID_FPGA_TICKS == cid) {
			i64_timestamp = *(reinterpret_cast<AT_64*>(puc_metadata));
			foundTimestamp = true;
		}
	} while (!foundTimestamp && puc_metadata > pBuf);

	return i64_timestamp;
}




void Andor_Camera::setMetaDataTimeStamp(bool  metaDataTimeStamp)
{
	THROW_IF_NOT_SUCCESS(setBool(MetadataTimestamp, metaDataTimeStamp),
		"setMetaDatatimestamp encountered a problem");

	
}
void Andor_Camera::getMetaDataTimeStamp(bool& metaDataTimeStamp) const
{
	bool	the_meta_data_time_stamp;
	getBool(MetadataTimestamp, &the_meta_data_time_stamp);
	metaDataTimeStamp = the_meta_data_time_stamp;
}
void Andor_Camera::setExpTime(double  exp_time)
{
	THROW_IF_NOT_SUCCESS(setFloat(ExposureTime, exp_time),
		"SetExpTime function has encountered an error");

}
void Andor_Camera::getExpTime(double& exp_time) const
{

	double		the_exp_time;
	getFloat(ExposureTime, &the_exp_time);
	exp_time = the_exp_time;
}
int Andor_Camera::getFloat(const AT_WC* Feature, double* Value) const
{
	
	return AT_GetFloat(m_camera_handle, Feature, Value);
}

void Andor_Camera::setSpuriousNoiseFilter(bool i_filter)
{
	
	setBool(SpuriousNoiseFilter, i_filter);
	bool		the_bool;
	getBool(SpuriousNoiseFilter, &the_bool);
	if (i_filter != the_bool) {
			logStream(MESSAGE_DEBUG) << "Failed to properly set the spurious noise filter mode : requested " << i_filter << ", but got " << the_bool<<sendLog;
}
}

void Andor_Camera::setAdcGain(A3_Gain iGain)
{
	
	if (m_real_camera) {
		int the_gain;
		// This can automatically update the PixelEncoding and BitDepth to values corresponding to the gain
		// Hence we proof-read the new values and update the cache accordingly :
		int	the_bit_depth; // Pixel encoding will be reset appropriately in the setBitDepth method
		getHwBitDepth(&the_bit_depth);
		setEnumIndex(PreAmpGainControl, iGain);
		getEnumIndex(PreAmpGainControl, &the_gain);
		m_adc_gain = static_cast<A3_Gain>(the_gain);
		if (m_adc_gain != iGain) {
				logStream(MESSAGE_INFO) <<  "Proof-reading the ADC readout gain :"
					<< "\n\tGot " << m_adc_gain << " back,"
					<< "\n\twhile requesting " << iGain<<sendLog;

		}
		setBitDepth(static_cast<A3_BitDepth>(the_bit_depth));
	}
	else {
		//std::cout << "Setting the gain is not possible for the SIMCAM. Skipping this request (value requested was : " << iGain << ").";
			logStream(MESSAGE_INFO) << "Setting the gain is not possible for the SIMCAM. Skipping this request (value requested was : " << iGain << ")."<<sendLog;
	}
}

void Andor_Camera::getElectronicShutterMode(A3_ShutterMode &oMode) const
{
	
	//  int the_mode;
	//  getEnumIndex(andor3::ElectronicShutteringMode, &the_mode);
	//  oMode = static_cast<A3_ShutterMode>(the_mode);
	oMode = m_electronic_shutter_mode;
}


Andor_Camera::Andor_Camera(int camera_number) :
	m_camera_number(camera_number),
	m_detector_model("un-inited"),

	//m_acq_thread(NULL),
	//m_cond(),
   //	m_acq_thread_waiting(true),
	 //m_acq_thread_running(false),
//		m_acq_thread_should_quit(false),
m_nb_frames_to_collect(100),
m_image_index(0),
m_buffer_ringing(false),
m_status(Ready),
m_real_camera(false),
//m_detector_model("un-inited"),
m_detector_type("un-inited"),
m_detector_serial("un-inited"),
//	m_detector_size(1, 1),
m_exp_time(5.6),//exp time in seconds
//		m_bitflow_path(bitflow_path),
	//	m_camera_number(camera_number),
	m_camera_handle(AT_HANDLE_UNINITIALISED),
	m_adc_gain(Gain1_Gain4),
	m_simple_gain(b16_lh_gain),
	m_adc_rate(MHz100),
	m_electronic_shutter_mode(Rolling),
	m_bit_depth(b16),
	m_trig_mode(Internal),
	m_cooler(true),
	m_temperature_sp(5.0),
	m_temperature_control_available(true),
	m_maximage_size_cb_active(false),
	imageCount(0)
{
	 
	THROW_IF_NOT_SUCCESS(AT_InitialiseLibrary(),
		"Library initialization failed, check the bitflow path");

	THROW_IF_NOT_SUCCESS(AT_InitialiseUtilityLibrary(),
		"Library Utility initialization failed, check the bitflow path");

/*

	// --- Get available cameras and select the choosen one.
	AT_64 numCameras;
	//std::cout << "Get all attached cameras";
	THROW_IF_NOT_SUCCESS(getIntSystem(DeviceCount, &numCameras),
		"No camera present!");

	//std::cout << "Found " << numCameras << " camera" << ((numCameras>1) ? "s" : "")<<"\n";

		logStream(MESSAGE_INFO) << "Get all attached cameras"<<sendLog;
		logStream(MESSAGE_INFO) << "Found " << numCameras << " camera" << ((numCameras>1) ? "s" : "")<<sendLog;
		logStream(MESSAGE_INFO) << "Try to set current camera to number " << m_camera_number<<sendLog;

*/
/*
	if (m_camera_number < numCameras && m_camera_number >= 0) {
		// Getting the m_camera_handle WITH some error checking :
		THROW_IF_NOT_SUCCESS(AT_Open(m_camera_number, &m_camera_handle),
			"Cannot get camera handle");
		
	}
	else {
		//std::cout << "Invalid camera number "
		//	<< m_camera_number << ", there is "
		//	<< numCameras << " available";
			logStream(MESSAGE_INFO) << "Invalid camera number "
				<< m_camera_number << ", there is "
				<< numCameras << " available"<<sendLog;
	}
	// --- Get Camera model (and all other parameters which are not changing during camera setup and usage )
	AT_WC	model[1024];
	THROW_IF_NOT_SUCCESS(getString(CameraModel, model, 1024),
		"Cannot get camera model");

	m_detector_model = WStringToString(std::wstring(model));

		logStream(MESSAGE_INFO) << "camera model is " << m_detector_model << sendLog;



  if (m_detector_model != "SIMCAM CMOS") {
		std::string		the_serial, the_fw;

		getSerialNumber(the_serial);
		getFirmwareVersion(the_fw);
		m_real_camera = true;
		m_detector_model += " (SN : " + the_serial + ", firmware " + the_fw + ")";


			logStream(MESSAGE_INFO) << "Camera is ready: type " << m_detector_type << ", model " << m_detector_model << sendLog;

	}
	else {
		m_real_camera = false;
		m_detector_type = std::string("Simulator");


			logStream(MESSAGE_INFO) << "The camera is indeed the SIMULATED camera, all exception for invalid parameter name will be ignored!!!"<<sendLog;

			logStream(MESSAGE_INFO) << "BE VERY CAREFULL : The andor SDK3 camera that you are connected to is a SIMULATED CAMERA !!!"<<sendLog;
}
*/	
	//initializeController();


	/*
		// Create the camera buffer
		m_buffer_ctrl_obj = new SoftBufferCtrlObj();
		// temporary buffer when the reconstruction task is active for decoding and destriding
		m_temp_buffer_ctrl_obj = new SoftBufferCtrlObj();
		
		// Initialisation of the atcore library :
		pthread_mutex_lock(&sdkInitMutex);
		if (!sAndorSDK3InittedCounter) {
			if (m_bitflow_path != "") {
				setenv("BITFLOW_INSTALL_DIRS", m_bitflow_path.c_str(), true);
			}
			else {
				setenv("BITFLOW_INSTALL_DIRS", "/usr/share/andor-3-sdk", false);
			}
			try
			{
				THROW_IF_NOT_SUCCESS(AT_InitialiseLibrary(),
					"Library initialization failed, check the bitflow path");

				THROW_IF_NOT_SUCCESS(AT_InitialiseUtilityLibrary(),
					"Library Utility initialization failed, check the bitflow path");
			}
			catch (...)
			{
				pthread_mutex_unlock(&sdkInitMutex);
				throw;
			}
			*/
		//	++sAndorSDK3InittedCounter;

	
}



int Andor_Camera::setInt(const AT_WC* Feature, AT_64 Value)
{
	
	return AT_SetInt(m_camera_handle, Feature, Value);
}

int Andor_Camera::getInt(const AT_WC* Feature, AT_64* Value) const
{
	
	return AT_GetInt(m_camera_handle, Feature, Value);
}

int Andor_Camera::getIntMax(const AT_WC* Feature, AT_64* MaxValue) const
{
	
	return AT_GetIntMax(m_camera_handle, Feature, MaxValue);
}

int Andor_Camera::getIntMin(const AT_WC* Feature, AT_64* MinValue) const
{
	
	return AT_GetIntMin(m_camera_handle, Feature, MinValue);
}

int Andor_Camera::setFloat(const AT_WC* Feature, double Value)
{
	
	return AT_SetFloat(m_camera_handle, Feature, Value);
}



int Andor_Camera::getFloatMax(const AT_WC* Feature, double* MaxValue) const
{
	
	return AT_GetFloatMax(m_camera_handle, Feature, MaxValue);
}

int Andor_Camera::getFloatMin(const AT_WC* Feature, double* MinValue) const
{
	
	return AT_GetFloatMin(m_camera_handle, Feature, MinValue);
}

int Andor_Camera::setBool(const AT_WC* Feature, bool Value)
{
	
	AT_BOOL newBool = Value;
	return AT_SetBool(m_camera_handle, Feature, newBool);
}

int Andor_Camera::getBool(const AT_WC* Feature, bool* Value) const
{
	
	AT_BOOL	newBool;
	int i_Err = AT_GetBool(m_camera_handle, Feature, &newBool);
	*Value = newBool;
	return i_Err;
}

int Andor_Camera::setEnumIndex(const AT_WC* Feature, int Value)
{
	
	return AT_SetEnumIndex(m_camera_handle, Feature, Value);
}

int Andor_Camera::setEnumString(const AT_WC* Feature, const AT_WC* String)
{
	
	return AT_SetEnumString(m_camera_handle, Feature, String);
}

int Andor_Camera::getEnumIndex(const AT_WC* Feature, int* Value) const
{
	
	return AT_GetEnumIndex(m_camera_handle, Feature, Value);
}

int Andor_Camera::getEnumString(const AT_WC* Feature, AT_WC* String, int StringLength) const
{
	
	int Value;
	int i_Err = AT_GetEnumIndex(m_camera_handle, Feature, &Value);
	if (AT_SUCCESS != i_Err)
		return i_Err;
	return AT_GetEnumStringByIndex(m_camera_handle, Feature, Value, String, StringLength);
}

int Andor_Camera::getEnumCount(AT_H m_camera_handle, const  AT_WC* Feature, int* Count) const
{
	return AT_GetEnumCount(m_camera_handle, Feature, Count);
}

int Andor_Camera::isEnumIndexAvailable(const AT_WC* Feature, int Index, bool* Available) const
{
	
	AT_BOOL  isAvailable;
	int i_Err = AT_IsEnumIndexAvailable(m_camera_handle, Feature, Index, &isAvailable);
	*Available = isAvailable;
	return i_Err;
}
int Andor_Camera::isEnumIndexImplemented(const AT_WC* Feature, int Index, bool* Implemented) const
{
	
	AT_BOOL  isImplemented;
	int i_Err = AT_IsEnumIndexAvailable(m_camera_handle, Feature, Index, &isImplemented);
	*Implemented = isImplemented;
	return i_Err;
}

int Andor_Camera::getEnumStringByIndex(const AT_WC* Feature, int Index, AT_WC* String, int StringLength) const
{
	return AT_GetEnumStringByIndex(m_camera_handle, Feature, Index, String, StringLength);
}

int Andor_Camera::getEnumIndexByString(const AT_WC* Feature, AT_WC* String, int *Index) const
{
	

	int         i_Err;
	int   		i_enumCount;
	int     	i_enumIndex;
	const int i_maxStringLen = 1024;
	AT_WC   	wcs_enumString[i_maxStringLen + 5];

	if (AT_SUCCESS != (i_Err = getEnumCount(m_camera_handle, Feature, &i_enumCount))) {
		logStream(MESSAGE_INFO) << "Failed to get Enum Count" << " : error code = " << return_from_error_code(i_Err)<<sendLog;
		return i_Err;
	}
	for (i_enumIndex = 0; i_enumCount != i_enumIndex; ++i_enumIndex) {
		if (AT_SUCCESS != getEnumStringByIndex(Feature, i_enumIndex, wcs_enumString, i_maxStringLen)) {
			logStream(MESSAGE_INFO) << "Failed to get Enum String" << " : error code = " << return_from_error_code(i_Err)<<sendLog;
			return i_Err;
		}
		if (!wcscmp(wcs_enumString, String)) {
			break;
		}
	}
	if (i_enumCount == i_enumIndex) {
		
		std::cout << "hata yapilamadi" << std::endl;
		//std::cout << "Unable to find index of enum string '" << WStringToString(String) << "' in '" << WStringToString(Feature) << "' : no such choice.";
		*Index = -1;
		i_Err = AT_ERR_INDEXNOTAVAILABLE;
	}
	else {
		*Index = i_enumIndex;
		i_Err = AT_SUCCESS;
	}
	return i_Err;
}

void Andor_Camera::getImageSize(int &o_frame_size) const
{
	
	AT_64 		the_size;
	getInt(ImageSizeBytes, &the_size);
	o_frame_size = static_cast<int>(the_size);
}


int Andor_Camera::setString(const AT_WC* Feature, const AT_WC* String)
{
	
	return AT_SetString(m_camera_handle, Feature, String);
}


int Andor_Camera::sendCommand(const AT_WC* Feature)
{
	
	return AT_Command(m_camera_handle, Feature);
}

int Andor_Camera::getHwBitDepth(int *bit_depth)
{
	
	int the_err_code = getEnumIndex(BitDepth, bit_depth);
	if ((the_err_code != AT_ERR_NOTIMPLEMENTED) || m_real_camera)
		return the_err_code;

	logStream(MESSAGE_DEBUG) << "Get BitDepth not implemented for simulated camera: "
		<< "fixing b16"<<sendLog;
	*bit_depth = b16;
	return AT_SUCCESS;
}

void Andor_Camera::getAdcRate(A3_ReadOutRate &oRate) const
{
	oRate = m_adc_rate;
}
void Andor_Camera::getAdcRateString(std::string &oRateString) const
{
	AT_WC the_string[256];
	getEnumString(PixelReadoutRate, the_string, 255);
	oRateString = WStringToString(std::wstring(the_string));

}

void Andor_Camera::getBitDepth(A3_BitDepth &oMode) const
{
	
	oMode = m_bit_depth;
}

void Andor_Camera::getBitDepthString(std::string &oDepthString) const
{
	AT_WC		the_string[256];
	getEnumStringByIndex(BitDepth, m_bit_depth, the_string, 255);
	oDepthString = WStringToString(std::wstring(the_string));
}

void Andor_Camera::getPxEncodingString(std::string &oPxEncodingString) const
{
	AT_WC		the_string[256];
	getEnumString(PixelEncoding, the_string, 255);
	oPxEncodingString = WStringToString(std::wstring(the_string));
}

void Andor_Camera::getPxEncoding(A3_PixelEncoding &oPxEncoding) const
{
	
	int the_mode;
	getEnumIndex(PixelEncoding, &the_mode);
	oPxEncoding = static_cast<A3_PixelEncoding>(oPxEncoding);
}


void Andor_Camera::getTriggerMode(A3_TriggerMode &oMode) const
{

	oMode = m_trig_mode;
}

void Andor_Camera::getTriggerModeString(std::string &oModeString) const
{
	AT_WC		the_string[256];
	getEnumStringByIndex(TriggerMode, m_trig_mode, the_string, 255);
	oModeString = WStringToString(std::wstring(the_string));
}
void Andor_Camera::getCycleModeString(std::string &oModeString) const
{
	AT_WC          the_string[256];
	getEnumString(CycleMode, the_string, 255);
	oModeString =WStringToString(std::wstring(the_string));

}
void Andor_Camera::getSimpleGain(A3_SimpleGain &o_gain) const
{
	

	//  (0) "Gain 1 (11 bit)"; implemented = true; available = true.
	//  (1) "Gain 2 (11 bit)"; implemented = true; available = true.
	//  (2) "Gain 3 (11 bit)"; implemented = true; available = true.
	//  (3) "Gain 4 (11 bit)"; implemented = true; available = true.
	//  (4) "Gain 1 Gain 3 (16 bit)"; implemented = true; available = true.
	//  (5) "Gain 1 Gain 4 (16 bit)"; implemented = true; available = true.
	//  (6) "Gain 2 Gain 3 (16 bit)"; implemented = true; available = true.
	//  (7) "Gain 2 Gain 4 (16 bit)"; implemented = true; available = true.
	if (propImplemented(SimplePreAmpGainControl)) {
		int			the_gain;
		getEnumIndex(SimplePreAmpGainControl, &the_gain);
		o_gain = static_cast<A3_SimpleGain>(the_gain);
	}
	else {
		//DEB_TRACE() << "SimplePreAmpGainControl not implemented, emulating it in software";

		A3_Gain					the_gain;
		A3_ShutterMode	the_shutter;

		getAdcGain(the_gain);
		getElectronicShutterMode(the_shutter);

		if (Rolling == the_shutter) {
			switch (the_gain) {
			case Gain1:
				o_gain = b11_low_gain;
				break;
			case Gain4:
				o_gain = b11_hi_gain;
				break;
			case Gain1_Gain4:
				o_gain = b16_lh_gain;
				break;

			default:
				o_gain = none;
				break;
			}
		}
		else {
			switch (the_gain) {
			case Gain1:
				o_gain = b11_low_gain;
				break;
			case Gain3:
				o_gain = b11_hi_gain;
				break;
			case Gain1_Gain3:
				o_gain = b16_lh_gain;
				break;

			default:
				o_gain = none;
				break;
			}
		}
	}
}


void Andor_Camera::getSimpleGainString(std::string &o_gainString) const
{

	if (propImplemented(SimplePreAmpGainControl)) {
		AT_WC		the_string[256];
		getEnumString(SimplePreAmpGainControl, the_string, 255);
		o_gainString = WStringToString(std::wstring(the_string));
	}
	else {
		//DEB_TRACE() << "The camera has no simple gain control setting... Do nothing !";
		//o_gainString = "not implemented";
	}
}
/*
void Andor_Camera::setAdcGain(A3_Gain iGain)
{
	
	if (m_real_camera) {
		int the_gain;
		// This can automatically update the PixelEncoding and BitDepth to values corresponding to the gain
		// Hence we proof-read the new values and update the cache accordingly :
		int	the_bit_depth; // Pixel encoding will be reset appropriately in the setBitDepth method
		getHwBitDepth(&the_bit_depth);
		setEnumIndex(PreAmpGainControl, iGain);
		getEnumIndex(PreAmpGainControl, &the_gain);
		m_adc_gain = static_cast<A3_Gain>(the_gain);
		if (m_adc_gain != iGain) {
		//	DEB_ERROR() << "Proof-reading the ADC readout gain :"
		//		<< "\n\tGot " << m_adc_gain << " back,"
		//		<< "\n\twhile requesting " << iGain;
		}
		setBitDepth(static_cast<A3_BitDepth>(the_bit_depth));
	}
	else {
		//DEB_TRACE() << "Setting the gain is not possible for the SIMCAM. Skipping this request (value requested was : " << iGain << ").";
	}
}
*/
void Andor_Camera::getAdcGain(A3_Gain &oGain) const
{
	
	//  int the_gain;
	//  getEnumIndex(andor3::PreAmpGainControl, &the_gain);
	//  oGain = static_cast<A3_Gain>(the_gain);
	oGain = m_adc_gain;
}

void Andor_Camera::getAdcGainString(std::string &oGainString) const
{
	AT_WC		the_string[256];
	getEnumString(PreAmpGainControl, the_string, 255);
	oGainString = WStringToString(std::wstring(the_string));
}

//-----------------------------------------------------
// @brief just error codes to string
//-----------------------------------------------------

static const char* return_from_error_code(int error_code)
{
	const char *error;
	switch (error_code)
	{
	case AT_SUCCESS:			error = "'AT_SUCCESS' : Function call has been successful";							 break;
	case AT_ERR_NOTINITIALISED:		error = "'AT_ERR_NOTINITIALISED' : Function called with an uninitialised handle";				 break;
	case AT_ERR_NOTIMPLEMENTED:		error = "'AT_ERR_NOTIMPLEMENTED' : Feature has not been implemented for the chosen camera";			 break;
	case AT_ERR_READONLY:			error = "'AT_ERR_READONLY' : Feature is read only";								 break;
	case AT_ERR_NOTREADABLE:		error = "'AT_ERR_NOTREADABLE' : Feature is currently not readable";						 break;
	case AT_ERR_NOTWRITABLE:		error = "'AT_ERR_NOTWRITABLE' : Feature is currently not writable";						 break;
	case AT_ERR_OUTOFRANGE:		error = "'AT_ERR_OUTOFRANGE' : Value is outside the maximum and minimum limits";				 break;
	case AT_ERR_INDEXNOTAVAILABLE:	error = "'AT_ERR_INDEXNOTAVAILABLE' : Index is currently not available";					 break;
	case AT_ERR_INDEXNOTIMPLEMENTED:	error = "'AT_ERR_INDEXNOTIMPLEMENTED' : Index is not implemented for the chosen camera";			 break;
	case AT_ERR_EXCEEDEDMAXSTRINGLENGTH:	error = "'AT_ERR_EXCEEDEDMAXSTRINGLENGTH' : String value provided exceeds the maximum allowed length";		 break;
	case AT_ERR_CONNECTION:		error = "'AT_ERR_CONNECTION' : Error connecting to or disconnecting from hardware";				 break;
	case AT_ERR_NODATA:			error = "AT_ERR_NODATA";											 break;
	case AT_ERR_INVALIDHANDLE:		error = "AT_ERR_INVALIDHANDLE";											 break;
	case AT_ERR_TIMEDOUT:			error = "'AT_ERR_TIMEDOUT' : The AT_WaitBuffer function timed out while waiting for data arrive in output queue"; break;
	case AT_ERR_BUFFERFULL:		error = "'AT_ERR_BUFFERFULL' : The input queue has reached its capacity";					 break;
	case AT_ERR_INVALIDSIZE:		error = "'AT_ERR_INVALIDSIZE' : The size of a queued buffer did not match the frame size";			 break;
	case AT_ERR_INVALIDALIGNMENT:		error = "'AT_ERR_INVALIDALIGNMENT' : A queued buffer was not aligned on an 8-byte boundary";			 break;
	case AT_ERR_COMM:			error = "'AT_ERR_COMM' : An error has occurred while communicating with hardware";				 break;
	case AT_ERR_STRINGNOTAVAILABLE:	error = "'AT_ERR_STRINGNOTAVAILABLE' : Index / String is not available";					 break;
	case AT_ERR_STRINGNOTIMPLEMENTED:	error = "'AT_ERR_STRINGNOTIMPLEMENTED' : Index / String is not implemented for the chosen camera";		 break;
	case AT_ERR_NULL_FEATURE:		error = "AT_ERR_NULL_FEATURE";											 break;
	case AT_ERR_NULL_HANDLE:		error = "AT_ERR_NULL_HANDLE";											 break;
	case AT_ERR_NULL_IMPLEMENTED_VAR:	error = "AT_ERR_NULL_IMPLEMENTED_VAR";										 break;
	case AT_ERR_NULL_READABLE_VAR:	error = "AT_ERR_NULL_READABLE_VAR";										 break;
	case AT_ERR_NULL_READONLY_VAR:	error = "AT_ERR_NULL_READONLY_VAR";										 break;
	case AT_ERR_NULL_WRITABLE_VAR:	error = "AT_ERR_NULL_WRITABLE_VAR";										 break;
	case AT_ERR_NULL_MINVALUE:		error = "AT_ERR_NULL_MINVALUE";											 break;
	case AT_ERR_NULL_MAXVALUE:		error = "AT_ERR_NULL_MAXVALUE";											 break;
	case AT_ERR_NULL_VALUE:		error = "AT_ERR_NULL_VALUE";											 break;
	case AT_ERR_NULL_STRING:		error = "AT_ERR_NULL_STRING";											 break;
	case AT_ERR_NULL_COUNT_VAR:		error = "AT_ERR_NULL_COUNT_VAR";										 break;
	case AT_ERR_NULL_ISAVAILABLE_VAR:	error = "AT_ERR_NULL_ISAVAILABLE_VAR";										 break;
	case AT_ERR_NULL_MAXSTRINGLENGTH:	error = "AT_ERR_NULL_MAXSTRINGLENGTH";										 break;
	case AT_ERR_NULL_EVCALLBACK:		error = "AT_ERR_NULL_EVCALLBACK";										 break;
	case AT_ERR_NULL_QUEUE_PTR:		error = "AT_ERR_NULL_QUEUE_PTR";										 break;
	case AT_ERR_NULL_WAIT_PTR:		error = "AT_ERR_NULL_WAIT_PTR";											 break;
	case AT_ERR_NULL_PTRSIZE:		error = "AT_ERR_NULL_PTRSIZE";											 break;
	case AT_ERR_NOMEMORY:			error = "AT_ERR_NOMEMORY";											 break;
	case AT_ERR_DEVICEINUSE:		error = "AT_ERR_DEVICEINUSE";											 break;
	case AT_ERR_HARDWARE_OVERFLOW:	error = "AT_ERR_HARDWARE_OVERFLOW";										 break;
	default:
		error = "Unknown"; break;
	}
	return error;
}
