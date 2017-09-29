#ifndef CAMERAOPERATIONS_H
#define CAMERAOPERATIONS_H
// Camera SDK headers :
#include "atcore.h"
#include "atutility.h"
#include "logstream.h"

// System headers :
#include <stdlib.h>
#include <limits>
#include <ostream>

//void WriteLogFile(const char* szString);
class Andor_Camera
{
public:
	enum TrigMode {
		IntTrig, IntTrigMult,
		ExtTrigSingle, ExtTrigMult,
		ExtGate, ExtStartStop, ExtTrigReadout,
	};



	enum Status { Ready, Exposure, Readout, Latency, Fault };
	enum A3_TypeInfo { Unknown, Int, Float, Bool, Enum, String };

	//! @TODO : later on should do a map (string to int and vice-versa) from parsed enum info for the next 3 :
	// In the same order/index as "PreAmpGainControl"
	enum A3_Gain { Gain1 = 0, Gain2 = 1, Gain3 = 2, Gain4 = 3, Gain1_Gain3 = 4, Gain1_Gain4 = 5, Gain2_Gain3 = 6, Gain2_Gain4 = 7 };
	// The "simple" version :
	enum A3_SimpleGain { b11_hi_gain = 0, b11_low_gain = 1, b16_lh_gain = 2, none = 31 };
	// In the same order/index as "ElectronicShutteringMode"
	enum A3_ShutterMode { Rolling = 0, Global = 1 };
	// In the same order/index as "PixelReadoutRate"
	enum A3_ReadOutRate { MHz10 = 0, MHz100 = 1, MHz200 = 2, MHz280 = 3 };
	// In the same order/index as 'BitDepth'
	enum A3_BitDepth { b11 = 0, b16 = 1 };
	// The camera trigger mode (in the enum order) :
	enum A3_TriggerMode { Internal = 0, ExternalLevelTransition = 1, ExternalStart = 2, ExternalExposure = 3, Software = 4, Advanced = 5, External = 6 };
	// The binning system of andor3 :
	enum A3_Binning { B1x1 = 0, B2x2 = 1, B3x3 = 2, B4x4 = 3, B8x8 = 4 };
	// The fan speed
	enum A3_FanSpeed { Off = 0, Low = 1, On = 2 };
	enum A3_PixelEncoding { Mono12 = 0, Mono12Packed = 1, Mono16 = 2, Mono32 = 3 };

	bool                        m_cooler;
	double                      m_temperature_sp;
	bool                        m_temperature_control_available;

	struct SdkFrameDim {
		AT_64 width;
		AT_64 height;
		AT_64 stride;
		double bytes_per_pixel;
		AT_64 size;
		A3_PixelEncoding encoding;
		const AT_WC* input_encoding;
		const AT_WC* output_encoding;
		bool is_encoded;
		bool is_strided;

	};
	SdkFrameDim   the_sdk_frame_dim;
	/*ismail ekledi*/
	double		          m_exp_time;
	int BufferSize;
	AT_64 ImageHeight;
	AT_64 ImageWidth;
	AT_64 ImageStride;
	unsigned char * UserBuffer;
	unsigned char *acqBuffer = NULL;
	unsigned char *acqBuffer1 = NULL;
	unsigned char *acqBuffer2 = NULL;
	unsigned char *acqBuffer3 = NULL;
	unsigned char *acqBuffer4 = NULL;
	unsigned char *acqBuffer5 = NULL;
	unsigned char *acqBuffer6 = NULL;
	unsigned char *acqBuffer7 = NULL;
	unsigned char *acqBuffer8 = NULL;
	unsigned char *acqBuffer9 = NULL;
	unsigned char *acqBuffer10 = NULL;
	unsigned char *acqBuffer11 = NULL;
	unsigned char *acqBuffer12 = NULL;
	unsigned char *acqBuffer13 = NULL;
	unsigned char *acqBuffer14 = NULL;
	unsigned char *acqBuffer15 = NULL;
	unsigned char *acqBuffer16 = NULL;
	unsigned char *acqBuffer17 = NULL;
	unsigned char *acqBuffer18 = NULL;
	unsigned char *acqBuffer19 = NULL;
	unsigned char* QueueBuffer(int _handle);
	void createBuffers(int _handle);
	void deleteBuffers();
	static const int CID_FPGA_TICKS = 1;
	/*ismail ekledi*/
	int getHwBitDepth(int *bit_depth);
	void initialize();
	int getIntSystem(const AT_WC*, AT_64*);

	void getSerialNumber(std::string &) const;
	void getFirmwareVersion(std::string &) const;
	void initializeController();
	void setElectronicShutterMode(A3_ShutterMode iMode);  // � exporter (avec le get)
	void setTriggerMode(A3_TriggerMode iMode);
	void getTriggerMode(A3_TriggerMode &oMode) const;
	void getTriggerModeString(std::string &oMode) const;
	void setSimpleGain(A3_SimpleGain i_gain);
	void getSimpleGain(A3_SimpleGain &i_gain) const;
	void getSimpleGainString(std::string &i_gain) const;
	void setAdcRate(A3_ReadOutRate iRate);
	void getAdcRate(A3_ReadOutRate &oRateString) const;
	void getAdcRateString(std::string &oRateString) const;
	// AdcGain is deprecated, should use SimpleGain (Zyla does not have AdcGain feature).
	void setAdcGain(A3_Gain iGain);
	void getAdcGain(A3_Gain &oGain) const;
	void getAdcGainString(std::string &oGainString) const;

	void getCycleModeString(std::string &oModeString) const;

	void getBitDepthString(std::string &oDepthString) const;
	void getBitDepth(A3_BitDepth &oMode) const;
	void setBitDepth(A3_BitDepth iMode);

        void getPxEncodingString(std::string &oPxEncodingString) const;
	void getPxEncoding(A3_PixelEncoding &oPxEncoding) const;
	
        void getIsAcquiring(bool isAcquiring);
        void setCooler(bool flag);
	void setTemperatureSP(double temp);
	bool propImplemented(const AT_WC * iPropName) const;
	void setExpTime(double  exp_time);
	void getExpTime(double  &exp_time)const;
	void setMetaDataTimeStamp(bool  metaDataTimeStamp);
	void getMetaDataTimeStamp(bool& metaDataTimeStamp)const;
	AT_64 GetTimeStamp(unsigned char* pBuf);
	void setMetaData(bool  metaData);
	void getMetaData(bool& metaData)const;
	void setSpuriousNoiseFilter(bool i_filter);
	int setBool(const AT_WC* Feature, bool Value);
//	void setAdcGain(A3_Gain iGain);
	void getElectronicShutterMode(A3_ShutterMode &oMode) const;
	void prepareAcq();
	void startAcq();
	void stopAcq();
	void getSdkFrameDim(SdkFrameDim &frame_dim, bool last);
	//void startAcq();
	unsigned short * getBuffer();
        unsigned short * getConvertedBuffer();
	unsigned short * getFitsBuffer(int);
	void afterInitialization();
	void setNumberOfFrames(size_t);
	size_t getNumberOfFrames();
	int imageCount;
	Andor_Camera(int);
//	~Andor_Camera();
private:
	class _AcqThread;
	class _BufferCtrlObj;
	friend class _AcqThread;

	// -- Members
	// LIMA / Acquisition (thread) related :
//	SoftBufferCtrlObj*          m_buffer_ctrl_obj;
//	SoftBufferCtrlObj*          m_temp_buffer_ctrl_obj;
	// Pure thread and signals :
//	_AcqThread*                 m_acq_thread;		   // The thread retieving frame buffers from the SDK
//	Cond                        m_cond;		   // Waiting condition for inter thread signaling
//	volatile bool               m_acq_thread_waiting;    // The m_acq_thread is waiting (main uses it to tell it to stop waiting)
//	volatile bool               m_acq_thread_running;	   // The m_acq_thread is running (main uses it to accept stopAcq)
//	volatile bool	          m_acq_thread_should_quit;// The main thread signals to m_acq_thread that it should quit.

													   // A bit more general :
	size_t                      m_nb_frames_to_collect;  // The number of frames to collect in current sequence
	size_t                      m_image_index;	   // The index in the current sequence of the next image to retrieve
	bool                        m_buffer_ringing;	   // Should the buffer be considered as a ring buffer rather than a single use buffer.
	Status                      m_status;	           // The current status of the camera

													   // LIMA / Not directly acquisition related :
	bool                        m_real_camera;	   // Set to false for CameraModel == "SIMCAM CMOS"
	std::string                 m_detector_model;
	std::string                 m_detector_type;
	std::string		  m_detector_serial;
	//Size			  m_detector_size;



    int                         m_camera_number;
	AT_H                        m_camera_handle;
//	std::string                 m_detector_model;
//	bool                        m_real_camera;	   // Set to false for CameraModel == "SIMCAM CMOS"
//	std::string                 m_detector_type;
//	std::string		  m_detector_serial;
//	double		          m_exp_time;

	// -- andor3 SDK stuff
	std::string                 m_bitflow_path;
	
	
	A3_Gain                     m_adc_gain;
	A3_SimpleGain               m_simple_gain;
	A3_ReadOutRate	          m_adc_rate;
	A3_ShutterMode		  m_electronic_shutter_mode;
	A3_BitDepth                 m_bit_depth;
	A3_TriggerMode              m_trig_mode;

	unsigned short* unpackedBuffer;
	unsigned short** fitsBuffer;
	
	//Size			  m_detector_size;


	// std::map<TrigMode, int>     m_trig_mode_maps;

	static int  sAndorSDK3InittedCounter;

	bool  m_maximage_size_cb_active;
	SdkFrameDim m_sdk_frame_dim;

//	size_t                      m_image_index;	   // The index in the current sequence of the next image to retrieve

	unsigned char * Buffer; //created for single frame acquisition

	int setInt(const AT_WC* Feature, AT_64 Value);
	int getInt(const AT_WC* Feature, AT_64* Value) const;
	int getIntMax(const AT_WC* Feature, AT_64* MaxValue) const;
	int getIntMin(const AT_WC* Feature, AT_64* MinValue) const;

	int setFloat(const AT_WC* Feature, double Value);
	int getFloat(const AT_WC* Feature, double* Value) const;
	int getFloatMax(const AT_WC* Feature, double* MaxValue) const;
	int getFloatMin(const AT_WC* Feature, double* MinValue) const;

	
	int getBool(const AT_WC* Feature, bool* Value) const;

	int setEnumIndex(const AT_WC* Feature, int Value);
	int setEnumString(const AT_WC* Feature, const AT_WC* String);
	int getEnumIndex(const AT_WC* Feature, int* Value) const;
	int getEnumString(const AT_WC* Feature, AT_WC* String, int StringLength) const;
	int getEnumCount(AT_H Hndl, const  AT_WC* Feature, int* Count) const;
	int isEnumIndexAvailable(const AT_WC* Feature, int Index, bool* Available) const;
	int isEnumIndexImplemented(const AT_WC* Feature, int Index, bool* Implemented) const;
	int getEnumStringByIndex(const AT_WC* Feature, int Index, AT_WC* String, int StringLength) const;
	int getEnumIndexByString(const AT_WC* Feature, AT_WC* String, int *Index) const;

	int setString(const AT_WC* Feature, const AT_WC* String);
	int getString(const AT_WC* Feature, AT_WC* String, int StringLength) const;
	void getImageSize(int &o_frame_size) const;

	int sendCommand(const AT_WC* Feature);


};

inline std::string atBoolToString(AT_BOOL iBool)
{
	return (iBool) ? std::string("true") : std::string("false");
}

inline std::string WStringToString(const std::wstring & iStr)
{
	
	
	// Should use wcstombs
	char			tmpStringBuf[1024];

	//bzero(tmpStringBuf, 1024);
	wcstombs(tmpStringBuf, iStr.c_str(), 1023);
	return std::string(tmpStringBuf);
	
	//std::string st = "isots";
	//return st;

	/*    std::ostringstream			theSStream;
	theSStream << iStr.c_str();
	return theSStream.str();
	*/
}
#endif
